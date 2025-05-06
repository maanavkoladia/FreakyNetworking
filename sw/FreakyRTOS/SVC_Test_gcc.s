// this is a file to test svc calls, didnt want to putit in statrup
    .syntax unified   // Unified assembly mode
    .cpu cortex-m4    // Ensure the assembler knows the correct CPU
    .thumb            // Enable Thumb mode


//uint32_t SVC_OS_Id(void);
//void SVC_OS_Kill(void);
//void SVC_OS_Sleep(uint32_t t);
//uint32_t SVC_OS_Time(void);
//int SVC_OS_AddThread(void(*t)(void), uint32_t s, uint32_t p);
    .global SVC_OS_Id
    .global SVC_OS_Kill
    .global SVC_OS_Sleep
    .global SVC_OS_Time
    .global SVC_OS_AddThread

.thumb_func
SVC_OS_Id:
    SVC #0
    BX LR

.thumb_func
SVC_OS_Kill:
    SVC #1
    BX LR

.thumb_func
SVC_OS_Sleep:
    SVC #2
     BX LR

.thumb_func
SVC_OS_Time:
    SVC #3
    BX LR

.thumb_func
SVC_OS_AddThread:
    SVC #4
    BX LR




