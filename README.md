# μC/OS-II for Visual Studio 2017

Modified from Micrium's official website to adapt to the VS2017 source code, and added the experimental example in the transplantation code of "Embedded Real-time Operating System μC/OS Principles and Practice" by Lu Youliang.

## Included MICRIUM product version

- uC/OS-II  v2.92.13 
- uC/OS-III v3.06.02
- uC/CPU    v1.31.01
- uC/LIB    v1.38.02
 
## IDE Environmental requirements

- Visual Studio v2017
 
## VSSolution file path

- Microsoft/Windows/Kernel/OS2/VS/OS2.sln
- Microsoft/Windows/Kernel/OS3/VS/OS3.sln
 
## Instructions for use

1. Open the file in the above path in Visual Studio
2. Right-click on the project to clean up the compiled files
3. Modify `Windows SDK Version` to the available version, such as `10.0.18362.0`
4. Compile and run

![vs2017 sdk](./media/vs2017_sdk.png)

## Original link

<https://www.micrium.com/download/micrium_win32_kernel/>


## Overview of porting implementation

> Note that the following code has been extensively deleted, and only the main line part is retained

### Time slice

The time slice relies on the timer to periodically wake up the response code.
The source of the timer of the traditional hardware platform is the hardware timer, and the source of the software analog timer is the `timeSetEvent` in the service provided by the Windows Multimedia API.
The entrance of Windows Multimedia API is in `winmm.dll`, so the project environment should introduce the dependency of this dynamic library.
`OSInitHookBegin()` located in `os_cpu_c.c`

```c
void  OSInitHookBegin (void)
{

    ...

// Bind OSTickW32 to a thread and write the thread ID to OSTick_ThreadId
    OSTick_Thread = CreateThread(NULL, 0, OSTickW32, 0, CREATE_SUSPENDED, &OSTick_ThreadId);

    //This thread can be run first in the entire process, and the time slice service can be run in time
    SetThreadPriority(OSTick_Thread, THREAD_PRIORITY_HIGHEST);

    //The set OSTick_TimerCap should be greater than or equal to the maximum time accuracy provided by winmm (minimum time interval)
    if (OSTick_TimerCap.wPeriodMin <WIN_MM_MIN_RES) {
        OSTick_TimerCap.wPeriodMin = WIN_MM_MIN_RES;
    }

    // set this precision
    timeBeginPeriod(OSTick_TimerCap.wPeriodMin);

    // Create an event, which can be understood as a notification, and set the thread waiting for this event/notification as runnable through setEvent
    OSTick_SignalPtr = CreateEvent(NULL, TRUE, FALSE, NULL);

    // Set the cycle, precision, and OSTick_SignalPtr of the cyclic timing trigger
    OSTick_TimerId = timeSetEvent((UINT )(1000u / OS_TICKS_PER_SEC),
                                (UINT) OSTick_TimerCap.wPeriodMin,
                                (LPTIMECALLBACK) OSTick_SignalPtr,
                                (DWORD_PTR) NULL,
                                (UINT )(TIME_PERIODIC | TIME_CALLBACK_EVENT_SET));

    ...

}
```

In the above code, the `OSTick_Thread` is created, and the loop timing trigger provided by winmm is set up, and the time slice service function waits for the timing trigger event.
In order for `OSTick_Thread` to wake up regularly, there should be an endless loop in `OSTickW32()` and waiting for `OSTick_SignalPtr` event.

```c
static  DWORD  WINAPI  OSTickW32 (LPVOID  p_arg)
{
    CPU_BOOLEAN  terminate;
    CPU_BOOLEAN  suspended;
    HANDLE       wait_signal[2];
    CPU_SR_ALLOC();

    // Waiting for the OSTick_SignalPtr event, and the event waiting for the system to terminate
    wait_signal[0] = OSTerminate_SignalPtr;
    wait_signal[1] = OSTick_SignalPtr;

    terminate = DEF_FALSE;
    while (!terminate) {
        switch (WaitForMultipleObjects(2, wait_signal, FALSE, INFINITE)) {
            // OSTick_SignalPtr triggered
            case WAIT_OBJECT_0 + 1u:
                // Reset this event to prepare for the next trigger
                ResetEvent(OSTick_SignalPtr);
                // Change process status, turn off interrupt
                CPU_CRITICAL_ENTER();
                suspended = OSIntCurTaskSuspend();
                // Some things to complete the time slice update
                if (suspended == DEF_TRUE) {
                    OSIntEnter();
                    OSTimeTick();
                    OSIntExit();
                    OSIntCurTaskResume();
                }
                CPU_CRITICAL_EXIT();
                break;
        }
    }
}
```

### Process management

Process management depends on the underlying context switching.
The context switching of traditional hardware platforms is basically registering the stack to the backup site, and the task stack popping to restore the site.
The context switching here depends on the in-process thread switching provided by the operating system, and the thread switching comes with context switching.
In order to manage the processes in μC/OS to the operating system as threads, it needs to be wrapped.  

```c
typedef  struct  os_task_stk {
void *TaskArgPtr; /* Task parameter array pointer */
    INT16U TaskOpt; /* Task option */
    void (*Task)(void*); /* Task function entry */
    HANDLE ThreadHandle; /* Task thread handle */
    DWORD ThreadID; /* Thread ID of Task thread */
    volatile OS_TASK_STATE TaskState; /* State of Task thread */
    HANDLE SignalPtr; /* Synchronization signal of Task thread */
    HANDLE InitSignalPtr; /* Task thread creation signal */
    CPU_BOOLEAN Terminate; /* End flag of Task thread */
    OS_TCB *OSTCBPtr; /* Task TCB pointer */
} OS_TASK_STK;
```

In this data structure, various information about the thread wrapped by the TCB is stored. Since this information is stored in its task stack, its initial value is completed in the task stack initialization of the TCB initialization.

```c
OS_STK  *OSTaskStkInit (void  (*task)(void  *pd), void  *p_arg, OS_STK  *ptos, INT16U  opt)
{
    OS_TASK_STK  *p_stk;

    /* Load stack pointer */
    p_stk                = (OS_TASK_STK *)((char *)ptos - sizeof(OS_TASK_STK));
    p_stk->TaskArgPtr    =  p_arg;
    p_stk->TaskOpt       =  opt;
    p_stk->Task          =  task;
    p_stk->ThreadHandle  =  NULL;
    p_stk->ThreadID      =  0u;
    p_stk->TaskState     =  STATE_NONE;
    p_stk->SignalPtr     =  NULL;
    p_stk->InitSignalPtr =  NULL;
    p_stk->Terminate     =  DEF_FALSE;
    p_stk->OSTCBPtr      =  NULL;

    return ((OS_STK *)p_stk);
}
```

In the `OS_TCBInit()` that is executed after `OSTaskStkInit()`, the package of the task is further initialized with the help of `OSTCBInitHook(ptcb);`.

```c
void  OSTCBInitHook (OS_TCB  *p_tcb)
{
    OS_TASK_STK  *p_stk;
    p_stk = (OS_TASK_STK *)p_tcb->OSTCBStkPtr;

    // The synchronization signal of the Task thread, actually the thread that switches to this task is notified by this event
    p_stk->SignalPtr = CreateEvent(NULL, FALSE, FALSE, NULL);

    // The creation of Task thread is completed signal,
    p_stk->InitSignalPtr = CreateEvent(NULL, TRUE, FALSE, NULL);

    // The handle of the Task thread, bind the task wrapper function OSTaskW32 to the thread
    p_stk->ThreadHandle = CreateThread(NULL, 0, OSTaskW32, p_tcb, CREATE_SUSPENDED, &p_stk->ThreadID);

    // Set the following current state, created but not run
    p_stk->TaskState = STATE_CREATED;
    p_stk->OSTCBPtr  = p_tcb;

}
```

Almost, the internal function of the task can start running. After the task scheduling starts, complete the following work.
The order is: `OSTCBInitHook()`->`OSCtxSw()`->`OSTaskW32()`->`OSCtxSw()`->`OSTaskW32()`->END

```c
static  DWORD  WINAPI  OSTaskW32 (LPVOID  p_arg)
{
    OS_TASK_STK  *p_stk;
    OS_TCB       *p_tcb;
    p_tcb = (OS_TCB      *)p_arg;
    p_stk = (OS_TASK_STK *)p_tcb->OSTCBStkPtr;

    // Wait for the task to be scheduled for the first time, and then execute it later
    // This wait makes it return to the thread that executed OSCtxSw before
    p_stk->TaskState = STATE_SUSPENDED;
    WaitForSingleObject(p_stk->SignalPtr, INFINITE);

    // Set a name for the thread to facilitate IDE debugging
    OSSetThreadName(p_stk->ThreadID, p_tcb->OSTCBTaskName);

    // Waiting, the marking task has been initialized and is ready to be officially scheduled
    p_stk->TaskState = STATE_RUNNING;
    SetEvent(p_stk->InitSignalPtr);

    // Function to execute task
    p_stk->Task(p_stk->TaskArgPtr);

    // If the above function returns after execution, you must delete yourself
    OSTaskDel(p_tcb->OSTCBPrio); 
    return (0u);
}
```

```c
void  OSCtxSw (void)
{
    OS_TASK_STK  *p_stk;
    OS_TASK_STK  *p_stk_new;
    CPU_SR_ALLOC();

    p_stk = (OS_TASK_STK *)OSTCBCur->OSTCBStkPtr;

    OSTaskSwHook();

    OSTCBCur  = OSTCBHighRdy;
    OSPrioCur = OSPrioHighRdy;

    // The current status of the old task is changed to suspended, but there is no stacking operation, because the saving context is done by the operating system that performs thread switching
    if (p_stk->TaskState == STATE_RUNNING) {
        p_stk->TaskState = STATE_SUSPENDED;
    }

    p_stk_new = (OS_TASK_STK *)OSTCBHighRdy->OSTCBStkPtr;
    switch (p_stk_new->TaskState) {
        case STATE_CREATED:
            // Just created, OSTaskW32() is needed for further initialization
            ResumeThread(p_stk_new->ThreadHandle);
            // Wait for the event that the initialization of this task is complete, and then set the SignalPtr event to make the thread of this task ready
            // Prototype: SignalObjectAndWait(hObjectToSignal, hObjectToWaitOn, dwMilliseconds, bAlertable);
            SignalObjectAndWait(p_stk_new->SignalPtr, p_stk_new->InitSignalPtr, INFINITE, FALSE);
            break;


        case STATE_SUSPENDED:
            // Set the SignalPtr event, the thread of this task is ready
            p_stk_new->TaskState = STATE_RUNNING;
            SetEvent(p_stk_new->SignalPtr);
            break;


        case STATE_INTERRUPTED:
            p_stk_new->TaskState = STATE_RUNNING;
            ResumeThread(p_stk_new->ThreadHandle);
            break;

        default:
            return;
    }

    if (p_stk->Terminate == DEF_TRUE) {
        OSTaskTerminate(p_stk);

        CPU_CRITICAL_EXIT();

        /* ExitThread() never returns. */
        ExitThread(0u);
        return;
    }
    CPU_CRITICAL_EXIT();
    // Who is waiting for the SignalPtr event, isn’t it doing some switching?
    WaitForSingleObject(p_stk->SignalPtr, INFINITE);
    CPU_CRITICAL_ENTER();
}
```

Thread switching must give control to the thread waiting for this model. Why do you wait for yourself?
So I looked at the function call stack of the thread:

![thread-stack](./media/tcb-thread-stack.png)

This is waiting for other threads to set the `SignalPtr` of your own thread and let your own thread continue to run.

