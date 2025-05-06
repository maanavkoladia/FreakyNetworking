;/*****************************************************************************/
;/* OSasm.s: low-level OS commands, written in assembly                       */
;/* derived from uCOS-II                                                      */
;/*****************************************************************************/
;Jonathan Valvano, OS Lab2/3/4/5, 1/12/20
;Students will implement these functions as part of EE445M/EE380L.12 Lab

        AREA |.text|, CODE, READONLY, ALIGN=2
        THUMB
        REQUIRE8
        PRESERVE8

        EXTERN  RunPt            ; currently running thread
		EXTERN nextPtCS
		;EXTERN SVC_Vector_Table
		
        EXPORT  StartOS
        EXPORT  ContextSwitch
        EXPORT  PendSV_Handler
        ;EXPORT  SVC_Handler
		EXPORT  OSStartHang
		EXPORT 	GetxPSR
	

		EXPORT FirstOpenPriorityList

NVIC_INT_CTRL   EQU     0xE000ED04                              ; Interrupt control state register.
NVIC_SYSPRI14   EQU     0xE000ED22                              ; PendSV priority register (position 14).
NVIC_SYSPRI15   EQU     0xE000ED23                              ; Systick priority register (position 15).
NVIC_LEVEL14    EQU           0xCF                              ; Systick priority value (second lowest).
NVIC_LEVEL15    EQU           0xFF                              ; PendSV priority value (lowest).
NVIC_PENDSVSET  EQU     0x10000000                              ; Value to trigger PendSV exception.
PD3             EQU     0x40007020                              ; pd3


StartOS
    ;LDR R0, =NVIC_SYSPRI14
    ;LDR R1, =NVIC_LEVEL15
    ;str R1, [R0]

    ;LDR R0, =NVIC_SYSPRI15
    ;LDR R1, =NVIC_LTEVEL14
    ;STR R1, [R0]

    LDR R0, =RunPt         ; Load the address of RunPt (pointer to the current running thread) into R0
    LDR R1, [R0]           ; Load the value stored at RunPt (the pointer to the current TCB) into R1
    LDR SP, [R1]           ; Load the stack pointer from the current TCB into the processor's SP register (SP = RunPt->sp)
    POP {R4-R11}           ; Restore registers R4-R11 from the stack
    POP {R0-R3}            ; Restore registers R0-R3 from the stack
    POP {R12}              ; Restore register R12 from the stack
    ADD SP, SP, #4         ; Adjust stack pointer to discard the saved LR (Link Register) from the initial stack
    POP {LR}               ; Load the actual LR (return address) from the stack
    ADD SP, SP, #4         ; Adjust stack pointer to discard the saved PSR (Program Status Register)
    CPSIE I                ; Enable interrupts at the processor level
    BX LR                  ; Branch to the address in LR to start executing the first thread



OSStartHang
    B       OSStartHang        ; Should never get here



;//********************************************************************************************************
;//                                 FIND NUMBER OF LEADING ZEROS RALTIVE TO A LIST SIZE
;//int FirstOpenPriorityList(uint32_t leading0s);
;//shoud generatea an index to access at
;//********************************************************************************************************
;.thumb_func
FirstOpenPriorityList
    RBIT R0, R0 ;// reverse the int because pri 0 is highest
    CLZ R0, R0 ;//get the numbe rof leading 0s
    BX LR

;********************************************************************************************************
;                               PERFORM A CONTEXT SWITCH (From task level)
;                                           void ContextSwitch(void)
;
; Note(s) : 1) ContextSwitch() is called when OS wants to perform a task context switch.  This function
;              triggers the PendSV exception which is where the real work is done.
;********************************************************************************************************

ContextSwitch
; edit this code
    LDR R0, =NVIC_INT_CTRL
    LDR R1, =NVIC_PENDSVSET
    STR R1, [R0]
    BX  LR
    

;********************************************************************************************************
;                                         HANDLE PendSV EXCEPTION
;                                     void OS_CPU_PendSVHandler(void)
;
; Note(s) : 1) PendSV is used to cause a context switch.  This is a recommended method for performing
;              context switches with Cortex-M.  This is because the Cortex-M3 auto-saves half of the
;              processor context on any exception, and restores same on return from exception.  So only
;              saving of R4-R11 is required and fixing up the stack pointers.  Using the PendSV exception
;              this way means that context saving and restoring is identical whether it is initiated from
;              a thread or occurs due to an interrupt or exception.
;
;           2) Pseudo-code is:
;              a) Get the process SP, if 0 then skip (goto d) the saving part (first context switch);
;              b) Save remaining regs r4-r11 on process stack;
;              c) Save the process SP in its TCB, OSTCBCur->OSTCBStkPtr = SP;
;              d) Call OSTaskSwHook();
;              e) Get current high priority, OSPrioCur = OSPrioHighRdy;
;              f) Get current ready thread TCB, OSTCBCur = OSTCBHighRdy;
;              g) Get new process SP from TCB, SP = OSTCBHighRdy->OSTCBStkPtr;
;              h) Restore R4-R11 from new process stack;
;              i) Perform exception return which will restore remaining context.
;
;           3) On entry into PendSV handler:
;              a) The following have been saved on the process stack (by processor):
;                 xPSR, PC, LR, R12, R0-R3
;              b) Processor mode is switched to Handler mode (from Thread mode)
;              c) Stack is Main stack (switched from Process stack)
;              d) OSTCBCur      points to the OS_TCB of the task to suspend
;                 OSTCBHighRdy  points to the OS_TCB of the task to resume
;
;           4) Since PendSV is set to lowest priority in the system (by OSStartHighRdy() above), we
;              know that it will only be run when no other exception or interrupt is active, and
;              therefore safe to assume that context being switched out was using the process stack (PSP).
;********************************************************************************************************

PendSV_Handler
; put your code here
	CPSID I              ; Disable interrupts to prevent race conditions during context switching
    PUSH {R4-R11}        ; Save the registers R4-R11 (callee-saved registers) onto the stac;
    LDR R0, =RunPt       ; Load the address of RunPt (pointer to the current running task)
    LDR R1, [R0]         ; Load RunPt (current running task's control block)
    STR SP, [R1]         ; Save the current task's stack pointer to its control bloc;
    LDR R1, =nextPtCS    ; Load the address of the next TCB
    LDR R1, [R1]         ; load next TCB
    STR R1, [R0]         ; Update RunPt to point to the next task (switching context)
    LDR SP, [R1]         ; Load the next task's stack pointe;
    POP {R4-R11}         ; Restore registers R4-R11 from the new task's stack
    CPSIE I              ; Re-enable interrupts
    BX LR                ; Return from exception (context switch complete)
 
    

;********************************************************************************************************
;                                         HANDLE SVC EXCEPTION
;                                     void OS_CPU_SVCHandler(void)
;
; Note(s) : SVC is a software-triggered exception to make OS kernel calls from user land. 
;           The function ID to call is encoded in the instruction itself, the location of which can be
;           found relative to the return address saved on the stack on exception entry.
;           Function-call paramters in R0..R3 are also auto-saved on stack on exception entry.
;********************************************************************************************************

        IMPORT    OS_Id
        IMPORT    OS_Kill
        IMPORT    OS_Sleep
        IMPORT    OS_Time
        IMPORT    OS_AddThread

;SVC_Handler
;; put your Lab 5 code here


;    ;BX      LR                   ; Return from exception
;	LDR     R12, [SP, #24]         ; Load the stacked PC (from exception entry) into R12.
;	LDRH    R12, [R12, #-2]        ; Read the halfword containing the SVC instruction.
;	ANDS    R12, R12, #0xFF        ; Extract the SVC number (lowest 8 bits).
;	PUSH    {R11}                  ; Save callee-saved register R11.
;	LDR     R11, =SVC_Vector_Table ; Load the base address of the SVC vector table.
;	LDR     R11, [R11, R12, LSL #2] ; Index into the table to get the handler address.
;	PUSH    {LR}                  ; Save return address.
;	BLX     R11                   ; Branch to the SVC handler; result returned in R0.
;	POP     {LR}                  ; Restore return address.
;	POP     {R11}                 ; Restore R11.
;	STR     R0, [SP]              ; Optionally, overwrite the stacked R0 with the return value.
;	BX      LR                    ; Return from exception.




GetxPSR
        MRS     R0, xPSR         ; Move the value of xPSR into R0.
        BX      LR               ; Return to caller.
    ALIGN
    END
