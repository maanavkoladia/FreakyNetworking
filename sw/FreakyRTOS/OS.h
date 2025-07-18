#ifndef OS_H
#define OS_H
/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
/* ================================================== */
/*            GLOBAL VARIABLE DECLARATIONS            */
/* ================================================== */
#define SYSCLK_PERIOD 80000000

#define TIME_1MS    80000          
#define TIME_2MS    (2*TIME_1MS)  
#define TIME_500US  (TIME_1MS/2)  
#define TIME_250US  (TIME_1MS/4)  

#define NUMTHREADS 10 // maximum number of threads
#define NUMPROCESSES 5
#define STACKSIZE 256 // number of 32-bit words in stack

#define PRIORITY_RANGE 8

#define MAX_SLEEP_MS 0x7FFFFFFF
//#define  CALCULATE_TASK_JITTER

#define OS_FIFO_MAXSIZE 256

#define SVC_VECTOR_SIZE 32

#define Heap_Malloc Malloc
#define Heap_Calloc Calloc
#define Heap_Realloc Realloc
#define Heap_Free Free

typedef struct PCB_t{
    void* codeSection;
    void* dataSection;
    int threadCount;
    int PID;
}PCB_t;

struct TCB_t{
    uint32_t* stackPtr;         // Pointer to the stack
    struct TCB_t* nextTCBptr;   // Pointer to the next TCB
    struct TCB_t* prevTCBPtr;   // Pointer to the previous TCB
    uint8_t id;
    uint32_t timeToWakeUpMS;    //ticks left in sleep pool
    uint8_t priority;           // Task priority
    //PCB_t* parentProcess;
    bool isDead;
};
typedef struct TCB_t TCB_t;

typedef struct {
    int32_t Value;   // >0 means free, otherwise means busy
    TCB_t* blockList[PRIORITY_RANGE];
    uint32_t activeListsidx_BlockingList;
}sema4_t;

typedef struct heap_stats {
  uint32_t size;   // heap size (in bytes)
  uint32_t used;   // number of bytes used/allocated
  uint32_t free;   // number of bytes available to allocate
} heap_stats_t;

typedef struct OS_FIFO_t{
    int16_t head_OSFIFO, tail_OSFIFO, size_OSFIFO;
    uint8_t* OS_FIFO;
    uint8_t elem_size;
    sema4_t* mutex;
    sema4_t* DataRoomAvailable;
    sema4_t* DataAvailable;
}OS_FIFO_t;

typedef enum{
    OS_FAIL,
    OS_SUCCESS,
    WAIT_REJECTED,
}OS_Return_t;
/* ================================================== */
/*                 FUNCTION PROTOTYPES                */
/* ================================================== */

/**
 * @details  Initialize operating system, disable interrupts until OS_Launch.
 * Initialize OS controlled I/O: serial, ADC, systick, LaunchPad I/O and timers.
 * Interrupts not yet enabled.
 * @param  none
 * @return none
 * @brief  Initialize OS
 */
void OS_Init(void); 

//triggers the first context switch
void StartOS(void);

//kills the OS
void OSStartHang(void);

//******** OS_AddThread *************** 
// add a foregound thread to the scheduler
// Inputs: pointer to a void/void foreground task
//         number of bytes allocated for its stack
//         priority, 0 is highest, 5 is the lowest
// Outputs: 1 if successful, 0 if this thread can not be added
// stack size must be divisable by 8 (aligned to double word boundary)
// In Lab 2, you can ignore both the stackSize and priority fields
// In Lab 3, you can ignore the stackSize fields
int OS_AddThread(void(*task)(void), uint32_t stackSize, 
                 uint32_t priority);

//******** OS_Id *************** 
// returns the thread ID for the currently running thread
// Inputs: none
// Outputs: Thread ID, number greater than zero 
uint32_t OS_Id(void);

//******** OS_AddPeriodicThread *************** 
// add a background periodic task
// typically this function receives the highest priority
// Inputs: pointer to a void/void background function
//         period given in system time units (12.5ns)
//         priority 0 is the highest, 5 is the lowest
// Outputs: 1 if successful, 0 if this thread can not be added
// You are free to select the time resolution for this function
// It is assumed that the user task will run to completion and return
// This task can not spin, block, loop, sleep, or kill
// This task can call OS_Signal  OS_bSignal   OS_AddThread
// This task does not have a Thread ID
// In lab 2, this command will be called 0 or 1 times
// In lab 2, the priority field can be ignored
// In lab 3, this command will be called 0 1 or 2 times
// In lab 3, there will be up to four background threads, and this priority field 
//           determines the relative priority of these four threads
int OS_AddPeriodicThread(void(*task)(void), uint32_t period, 
                         uint32_t priority);

// ******** OS_Sleep ************
// place this thread into a dormant state
// input:  number of msec to sleep
// output: none
// You are free to select the time resolution for this function
// OS_Sleep(0) implements cooperative multitasking
void OS_Sleep(uint32_t sleepTime); 

// ******** OS_Kill ************
// kill the currently running thread, release its TCB and stack
// input:  none
// output: none
void OS_Kill(void); 

// ******** OS_Suspend ************
// suspend execution of currently running thread
// scheduler will choose another thread to execute
// Can be used to implement cooperative multitasking 
// Same function as OS_Sleep(0)
// input:  none
// output: none
void OS_Suspend(void);

// ******** OS_Suspend ************
// Prevent context switches (foreground thread scheduling); background interrupts still allowed.
// Returns previous scheduler lock state.
unsigned long OS_LockScheduler(void);

// Restore previous scheduler lock state, re-enabling thread scheduling if appropriate.
void OS_UnLockScheduler(unsigned long previous);
 
// ******** OS_InitSemaphore ************
// initialize semaphore 
// input:  pointer to a semaphore
// output: none
void OS_InitSemaphore(sema4_t *semaPt, int32_t value); 

// ******** OS_Wait ************
// decrement semaphore 
// Lab2 spinlock
// Lab3 block if less than zero
// input:  pointer to a counting semaphore
// output: none
int OS_Wait(sema4_t *semaPt);

void OS_bWait(sema4_t *semaPt);
// ******** OS_Signal ************
// increment semaphore 
// Lab2 spinlock
// Lab3 wakeup blocked thread if appropriate 
// input:  pointer to a counting semaphore
// output: none
void OS_Signal(sema4_t *semaPt); 
void OS_bSignal(sema4_t *semaPt); 

/**
 * @details Allocate memory, data not initialized
 * @param  desiredBytes: desired number of bytes to allocate
 * @return void* pointing to the allocated memory or will return NULL
 *         if there isn't sufficient space to satisfy allocation request
 * @brief  Allocate memory
 */
void* Heap_Malloc(size_t desiredBytes);


/**
 * @details Allocate memory, allocated memory is initialized to 0 (zeroed out)
 * @param  desiredBytes: desired number of bytes to allocate
 * @return void* pointing to the allocated memory block or will return NULL
 *         if there isn't sufficient space to satisfy allocation request
 * @brief Zero-allocate memory
 */
void* Heap_Calloc(size_t desiredBytes);


/**
 * @details Reallocate buffer to a new size. The given block may be 
 *          unallocated and its contents copied to a new block
 * @param  oldBlock: pointer to a block
 * @param  desiredBytes: a desired number of bytes for a new block
 * @return void* pointing to the new block or will return NULL
 *         if there is any reason the reallocation can't be completed
 * @brief  Grow/shrink memory
 */
void* Heap_Realloc(void* oldBlock, size_t desiredBytes);


/**
 * @details Return a block to the heap
 * @param  pointer to memory to unallocate
 * @return 0 if everything is ok, non-zero in case of error (e.g. invalid pointer
 *         or trying to unallocate memory that has already been unallocated)
 * @brief  Free memory
 */
int32_t Heap_Free(void* pointer);


/**
 * @details Return the current usage status of the heap
 * @param  reference to a heap_stats_t that returns the current usage of the heap
 * @return 0 in case of success, non-zeror in case of error (e.g. corrupted heap)
 * @brief  Get heap usage
 */
int32_t Heap_Stats(heap_stats_t *stats);

// ******** OS_Time ************
// return the system time 
// Inputs:  none
// Outputs: time in 12.5ns units, 0 to 4294967295
// The time resolution should be less than or equal to 1us, and the precision 32 bits
// It is ok to change the resolution and precision of this function as long as 
//   this function and OS_TimeDifference have the same resolution and precision 
uint32_t OS_Time(void);

// ******** OS_TimeDifference ************
// Calculates difference between two times
// Inputs:  two times measured with OS_Time
// Outputs: time difference in 12.5ns units 
// The time resolution should be less than or equal to 1us, and the precision at least 12 bits
// It is ok to change the resolution and precision of this function as long as 
//   this function and OS_Time have the same resolution and precision 
uint32_t OS_TimeDifference(uint32_t start, uint32_t stop);

// ******** OS_ClearMsTime ************
// sets the system time to zero (from Lab 1)
// Inputs:  none
// Outputs: none
// You are free to change how this works
void OS_ClearMsTime(void);

// ******** OS_MsTime ************
// reads the current time in msec (from Lab 1)
// Inputs:  none
// Outputs: time in ms units
// You are free to select the time resolution for this function
// It is ok to make the resolution to match the first call to OS_AddPeriodicThread
uint32_t OS_MsTime(void);

//******** OS_Launch *************** 
// start the scheduler, enable interrupts
// Inputs: number of 12.5ns clock cycles for each time slice
//         you may select the units of this parameter
// Outputs: none (does not return)
// In Lab 2, you can ignore the theTimeSlice field
// In Lab 3, you should implement the user-defined TimeSlice field
// It is ok to limit the range of theTimeSlice to match the 24-bit SysTick
void OS_Launch(uint32_t theTimeSlice);

//these funcs return the jitter for the two periodic tasks
//you must define CALCULATE_TASK_JITTER at the top
#ifdef CALCULATE_TASK_JITTER
int32_t GetPeriodicTask1Jitter(void);
int32_t GetPeriodicTask2Jitter(void);
void Print_JitterInfo(uint8_t taskNUM);
#endif

//******** OS_AddSW1Task *************** 
// add a background task to run whenever the SW1 (PF4) button is pushed
// Inputs: pointer to a void/void background function
//         priority 0 is the highest, 5 is the lowest
// Outputs: 1 if successful, 0 if this thread can not be added
// It is assumed that the user task will run to completion and return
// This task can not spin, block, loop, sleep, or kill
// This task can call OS_Signal  OS_bSignal   OS_AddThread
// This task does not have a Thread ID
// In labs 2 and 3, this command will be called 0 or 1 times
// In lab 2, the priority field can be ignored
// In lab 3, there will be up to four background threads, and this priority field 
//           determines the relative priority of these four threads
int OS_AddSW1Task(void(*task)(void), uint32_t priority);

//******** OS_AddSW2Task *************** 
// add a background task to run whenever the SW2 (PF0) button is pushed
// Inputs: pointer to a void/void background function
//         priority 0 is highest, 5 is lowest
// Outputs: 1 if successful, 0 if this thread can not be added
// It is assumed user task will run to completion and return
// This task can not spin block loop sleep or kill
// This task can call issue OS_Signal, it can call OS_AddThread
// This task does not have a Thread ID
// In lab 2, this function can be ignored
// In lab 3, this command will be called will be called 0 or 1 times
// In lab 3, there will be up to four background threads, and this priority field 
//           determines the relative priority of these four threads
int OS_AddSW2Task(void(*task)(void), uint32_t priority);

// ******** OS_Fifo_Init ************
// Initialize the Fifo to be empty
// Inputs: size
// Outputs: none 
// In Lab 2, you can ignore the size field
// In Lab 3, you should implement the user-defined fifo size
// In Lab 3, you can put whatever restrictions you want on size
//    e.g., 4 to 64 elements
//    e.g., must be a power of 2,4,8,16,32,64,128
OS_Return_t OS_Fifo_Init(uint32_t size, OS_FIFO_t* fifo, uint8_t* buffer, uint8_t elem_size, sema4_t semaphores[3]);

// ******** OS_Fifo_Put ************
// Enter one data sample into the Fifo
// Called from the background, so no waiting 
// Inputs:  data
// Outputs: true if data is properly saved,
//          false if data not saved, because it was full
// Since this is called by interrupt handlers 
//  this function can not disable or enable interrupts
OS_Return_t OS_Fifo_Put(uint8_t *data, OS_FIFO_t* fifo);

// ******** OS_Fifo_Get ************
// Remove one data sample from the Fifo
// Called in foreground, will spin/block if empty
// Inputs:  none
// Outputs: data 
uint8_t OS_Fifo_Get(uint8_t* dataOut, OS_FIFO_t* fifo);

// ******** OS_Fifo_Size ************
// Check the status of the Fifo
// Inputs: none
// Outputs: returns the number of elements in the Fifo
//          greater than zero if a call to OS_Fifo_Get will return right away
//          zero or less than zero if the Fifo is empty 
//          zero or less than zero if a call to OS_Fifo_Get will spin or block
int32_t OS_Fifo_Size(OS_FIFO_t* fifo);

void OS_Fifo_Print(OS_FIFO_t* fifo);

// ******** OS_MailBox_Init ************
// Initialize communication channel
// Inputs:  none
// Outputs: none
void OS_MailBox_Init(void);

// ******** OS_MailBox_Send ************
// enter mail into the MailBox
// Inputs:  data to be sent
// Outputs: none
// This function will be called from a foreground thread
// It will spin/block if the MailBox contains data not yet received 
void OS_MailBox_Send(uint32_t data);

// ******** OS_MailBox_Recv ************
// remove mail from the MailBox
// Inputs:  none
// Outputs: data received
// This function will be called from a foreground thread
// It will spin/block if the MailBox is empty 
uint32_t OS_MailBox_Recv(void);

void Interpreter(void);

/**
 * @details open the file for writing, redirect stream I/O (printf) to this file
 * @note if the file exists it will append to the end<br>
 If the file doesn't exist, it will create a new file with the name
 * @param  name file name is an ASCII string up to seven characters
 * @return 0 if successful and 1 on failure (e.g., can't open)
 * @brief  redirect printf output into this file (Lab 4)
 */

int OS_RedirectToFile(const char *name);

/**
 * @details close the file for writing, redirect stream I/O (printf) back to the UART
 * @param  none
 * @return 0 if successful and 1 on failure (e.g., trouble writing)
 * @brief  Stop streaming printf to file (Lab 4)
 */
int OS_EndRedirectToFile(void);

/**
 * @details redirect stream I/O (printf) to the UART0
 * @return 0 if successful and 1 on failure 
 * @brief  redirect printf output to the UART0
 */
int OS_RedirectToUART(void);

/**
 * @details redirect stream I/O (printf) to the ST7735 LCD
 * @return 0 if successful and 1 on failure 
 * @brief  redirect printf output to the ST7735
 */
 int OS_RedirectToST7735(void);

//******DisableInterrupts************
// sets the I bit in the PRIMASK to disable interrupts
// Inputs: none
// Outputs: none
extern void DisableInterrupts(void); // Disable interrupts

//******EnableInterrupts************
// clears the I bit in the PRIMASK to enable interrupts
// Inputs: none
// Outputs: none
extern void EnableInterrupts(void);  // Enable interrupts

//******StartCritical************
// StartCritical saves a copy of PRIMASK and disables interrupts
// Code between StartCritical and EndCritical is run atomically
// Inputs: none
// Outputs: copy of the PRIMASK (I bit) before StartCritical called
extern long StartCritical(void);    

//******EndCritical************
// EndCritical sets PRIMASK with value passed in
// Code between StartCritical and EndCritical is run atomically
// Inputs: PRIMASK (I bit) before StartCritical called
// Outputs: none
extern void EndCritical(long sr);    // restore I bit to previous value

//******WaitForInterrupt************
// enters low power sleep mode waiting for interrupt (WFI instruction)
// processor sleeps until next hardware interrupt
// returns after ISR has been run
// Inputs: none
// Outputs: none
extern void WaitForInterrupt(void);  

void _putchar(char character);

void *npfputc(char c, void *ctx);

#endif
