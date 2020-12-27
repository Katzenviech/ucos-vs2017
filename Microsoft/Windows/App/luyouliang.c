/*	 
*
*	
*	Last modified:	 2012-04-25 
*   written by Lu Youliang
*   for book: Analysis and Practice of Embedded Real-time Operating System μCOS
*   2011-2012 At University of Electronic Science and Technology of China
*   
*   
*/

#include <os.h>
#include "luyouliang.h"

BOOLEAN g_FlagEn = 1;		// Add a global variable as a sign of clock scheduling

OS_EVENT  *MyEventSem;

int a, b, c;
char * buffer;
/*Code used in the experiment 1*/
int FirstTask(void *pParam)
{
	int i, j = 0;
	int abc = 123;
	OSTimeDly(100);// Does not affect the maximum idle count value obtained when the statistical task is initialized
	printf("welcome to embeded system\n");
	printf("welcome to ucos\n");
	printf("This is a simple task for experiment 1!\n");

	for (;;)
	{
		for (i = 0; i<99999999; i++);
		//OSTimeDly(100);
		printf("Task delay=%d\n", j++);
	}
	return(0);
}
/* Code used in the experiment 2*/
void E2_task1(void *pParam)
{
	int i = 0;
	OSTimeDly(100);
	printf("Two tasks run alternately, E2task1, priority %d\n", OSPrioCur);
	for (;;)
	{
		printf("Task output with priority %d\n", OSPrioCur, i++);
		OSTimeDly(100);
	}
}

void E2_task2(void *pParam)
{
	int i = 0;
	OSTimeDly(200);
	printf("Two tasks run alternately, E2task2, priority %d\n", OSPrioCur);
	for (;;)
	{
		printf("Task output with priority %d\n", OSPrioCur, i++);
		OSTimeDly(200);
	}
}

/* Code used in the experiment 3*/
void E3_task0(void *pParam)
{
	int i = 0;
	OSTimeDly(100);
	printf("Task 0 priority %d, responsible for suspending and resuming collection tasks and data processing tasks\n", OSPrioCur);
	for (;;)
	{
		printf("Task 0 suspend task 1, resume task 2, suspend time 1 second\n");
		OSTaskSuspend(6);
		OSTaskResume(7);
		OSTimeDly(100);

		printf("Task 0 suspends task 2, resumes task 1, and suspends for 1 second\n");
		OSTaskSuspend(7);
		OSTaskResume(6);
		OSTimeDly(100);
	}
}

void E3_task1(void *pParam)
{
	int i = 0;
	OSTimeDly(100);
	printf("Task 1 priority %d\n", OSPrioCur);
	for (;;)
	{
		printf("Task 1 output %d\n", i++);
		OSTimeDly(20);
	}
}
void E3_task2(void *pParam)
{
	int i = 100;
	OSTimeDly(100);
	printf("Task 2 priority %d\n", OSPrioCur);
	for (;;)
	{
		printf("Task 2 output %d\n", i++);
		OSTimeDly(30);
	}
}
/*end Code used in the experiment 3*/


int add1(int *p1, int *p2)
{
	int a, b, c;
	a = *p1;
	b = *p2;
	OSTimeDly(OS_TICKS_PER_SEC);
	c = a + b;
	return(c);
}

int add2(int p1, int p2)
{
	//int a,b;
	a = p1;
	b = p2;
	OSTimeDly(OS_TICKS_PER_SEC);
	c = a + b;
	return(c);
}

void usertask(void *pParam)
{
	//INT8U i=0;
	// for(;;){
	//	printf("%d\n",i);
	//	i++;
	//	OSTimeDly(100);
	//}
	INT8U i = (INT8U)pParam;
	//printf("Delay %d seconds after output of tasks with priority %d\n",OSPrioCur,OSPrioCur);

	for (;;)
	{
		printf("Task output %d with priority %d\n", OSPrioCur, i++);
		OSTimeDly(100);
	}
}
void usertask1(void *pParam)
{
	int sum = 1;
	for (;;) {
		//printf("\ntask%d call add2(1,2)\n",1);
		//sum=add2(1,2);
		OSTimeDly(OS_TICKS_PER_SEC);
		printf("task5 after sleep, The number of times this task is scheduled %d, g_FlagEn=%d\n", sum++, g_FlagEn);
		//printf("\ntask%d call add2(1,2) solution is %d\n",1,sum);
	}//for(;;)
}
void usertask2(void *pParam)
{

	int sum = 1;
	for (;;) {
		//printf("\n\rtask%d call add2(100,200)\n",2);

		OSTimeDly(OS_TICKS_PER_SEC);
		//c=a+b;//sum=add2(100,200);
		printf("task6 after sleep, The number of times this task is scheduled %d, g_FlagEn=%d\n", sum++, g_FlagEn);//printf("\ntask%d call add2(100,200) solution is %d\n",2,sum);
	}//for(;;)
}

void UsrCouPri()
{
	int crt;
	crt = 0;
	for (;;) {
		//	sum=add2(100,200);	

	}
}

// Examples of semaphore management
void UserTaskSemA(void *pParam)
{
	/* The task SemA creates a semaphore and then periodically accesses the resource R */
	/* Create semaphore */
	INT8U     *perr;
	INT8U err;
	//INT8U i;
	OS_SEM_DATA mySemData;
	err = 0;
	perr = &err;
	MyEventSem = OSSemCreate(2);
	if (MyEventSem == (OS_EVENT *)0)
	{
		printf("Task A failed to create semaphore\n");
		OSTaskDel(OS_PRIO_SELF);
		return;
	}
	OSSemQuery(MyEventSem, &mySemData);
	printf("Time: %d, task A creates a semaphore. Current semaphore value=%d\n", OSTimeGet(), mySemData.OSCnt);
	while (1)
	{
		OSSemQuery(MyEventSem, &mySemData);
		printf("Time: %d, task A starts to request semaphore! Current semaphore value=%d\n", OSTimeGet(), mySemData.OSCnt);
		OSSemPend(MyEventSem, 0, perr);
		if (err != OS_ERR_NONE)
		{
			printf("Task A request semaphore failed\n");
			printf("Error number %d\n", err);
			continue;
		}
		OSSemQuery(MyEventSem, &mySemData);
		printf("Time: %d, task A gets the semaphore. Current semaphore value=%d, task A starts to operate on R\n", OSTimeGet(), mySemData.OSCnt);
		OSTimeDly(1000); /* Analog operation resource, it takes 10 seconds, 1000 clock ticks */
		printf("Time: %d, task A ends resource operation and submits semaphore! \n", OSTimeGet());
		OSSemPost(MyEventSem);
		OSSemQuery(MyEventSem, &mySemData);
		printf("Time: %d, the semaphore submitted by task A is completed, the current semaphore value=%d, task A will be blocked with a delay of 1000 ticks\n", OSTimeGet(), mySemData.OSCnt);
		OSTimeDly(1000);
	}
}

void UserTaskSemB(void *pParam)
{
	/* The task SemA creates a semaphore and then periodically accesses the resource R */
	INT8U     *perr;
	INT8U err;
	OS_SEM_DATA mySemData;
	err = 0;
	perr = &err;
	printf("Time: %d, task B starts to delay 300 clock ticks", OSTimeGet());
	OSTimeDly(300);/* Task B is delayed for 3 seconds */
	if (MyEventSem == (OS_EVENT *)0)
	{
		printf("Task A failed to create semaphore\n");
		OSTaskDel(OS_PRIO_SELF);
		return;
	}
	while (1)
	{
		OSSemQuery(MyEventSem, &mySemData);
		printf("Time: %d, task B starts to request semaphore! Current semaphore value=%d\n", OSTimeGet(), mySemData.OSCnt);
		OSSemPend(MyEventSem, 0, perr);
		if (err != OS_ERR_NONE)
		{
			printf("Task B request semaphore failed\n");
			printf("Error number %d\n", err);
			continue;
		}
		OSSemQuery(MyEventSem, &mySemData);
		printf("Time: %d, task B gets the semaphore. Current semaphore value=%d, task B starts to operate on R, it needs 1000 clock ticks\n", OSTimeGet(), mySemData.OSCnt);
		OSTimeDly(1000); /*Ä£Äâ²Ù×÷×ÊÔ´,ÐèÒª10Ãë£¬1000¸öÊ±ÖÓàÖàª*/
		printf("Time: %d, task B ends resource operation and submits semaphore!\n", OSTimeGet());
		OSSemPost(MyEventSem);
		OSSemQuery(MyEventSem, &mySemData);
		printf("Time: %d, the semaphore submitted by task B is completed, the current semaphore value=%d, task B will be blocked with a delay of 1000 ticks\n", OSTimeGet(), mySemData.OSCnt);
		OSTimeDly(1000);
	}
}


void UserTaskSemC(void *pParam)
{
	/* The task SemA creates a semaphore and then periodically accesses the resource R */
	INT8U     *perr;
	INT8U err;
	OS_SEM_DATA mySemData;
	err = 0;
	perr = &err;
	printf("Time: %d, the start of task C is delayed by 400 clock ticks", OSTimeGet());
	OSTimeDly(400);/*Task C first delay 4 seconds*/
	if (MyEventSem == (OS_EVENT *)0)
	{
		printf("Task A failed to create semaphore\n");
		OSTaskDel(OS_PRIO_SELF);
		return;
	}
	while (1)
	{
		OSSemQuery(MyEventSem, &mySemData);
		printf("Time: %d, task C starts to request semaphore! Current semaphore value=%d\n", OSTimeGet(), mySemData.OSCnt);
		OSSemPend(MyEventSem, 0, perr);
		if (err != OS_ERR_NONE)
		{
			printf("Task C request semaphore failed\n");
			printf("Error number %d\n", err);
			continue;
		}
		OSSemQuery(MyEventSem, &mySemData);
		printf("Time: %d, task C gets the semaphore. Current semaphore value=%d, task C starts to operate on R, it takes 1000 clock ticks\n", OSTimeGet(), mySemData.OSCnt);
		OSTimeDly(1000); /*Ä£Äâ²Ù×÷×ÊÔ´,ÐèÒª10Ãë£¬1000¸öÊ±ÖÓàÖàª*/
		printf("Time: %d, task C ends resource operation and submits semaphore\n", OSTimeGet());
		OSSemPost(MyEventSem);
		printf("Time: %d, the semaphore submitted by task C is completed, the current semaphore value=%d, task C will be blocked with a delay of 1000 ticks\n", OSTimeGet(), mySemData.OSCnt);
		OSTimeDly(1000);
	}
}

// Examples of event flag groups
INT8U IO[4][10];
OS_FLAG_GRP  * pFlagGroupDataProcess;
void TaskDataProcess(void *pParam)
{
	INT8U     *perr;
	INT8U err, i;
	INT16U SUM;
	OS_FLAGS processflag, retflag;
	err = OS_ERR_NONE;
	perr = &err;
	processflag = 0x0F;
	/* Create an event flag group, the initial value of the event flag is 0, no event occurs */
	pFlagGroupDataProcess = OSFlagCreate(0, perr);
	/*Omit to check whether the creation is successful */
	while (1)
	{
		printf("Time: %d, the task TaskDataProcess starts requesting event flags-----------!\n", OSTimeGet());
		retflag = OSFlagPend(pFlagGroupDataProcess,
			processflag,
			OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME,
			0,
			perr);

		if (retflag == processflag)
		{
			SUM = 0;
			printf("Time: %d, the task TaskDataProcess request event flag is successful, start processing data! -----------!\n", OSTimeGet());
			for (i = 0; i<10; i++)
			{
				SUM += IO[0][i] + IO[1][i] + IO[2][i] + IO[3][i];
			}
			printf("Time: %d, task TaskDataProcess processing data is completed, the result is %d\n", OSTimeGet(), SUM);
		}
	}
}
void TaskIO1(void *pParam)
{
	INT8U     *perr;
	INT8U err, i;

	OS_FLAGS rdyflag;
	//OS_FLAG_GRP  * pFlagGroup;
	err = OS_ERR_NONE;
	perr = &err;
	rdyflag = 0;

	while (1)
	{
		OSTimeDly(100);/*1 second delay*/
		for (i = 0; i<10; i++) /*Simulate the process of acquiring data*/
		{
			IO[0][i] = 1;
		}
		printf("Time: %d, after task TaskIO1 gets the IO data, it is ready to submit the event, the current event flag: %d\n", OSTimeGet(), rdyflag);
		rdyflag = OSFlagPost(pFlagGroupDataProcess,
			0x01,
			OS_FLAG_SET,
			perr);            /*Submit the event flag, set the last bit in the event flag group to 0*/
		printf("Time: %d, after task TaskIO1 gets the IO data, submit the event, the current event flag: %d\n", OSTimeGet(), rdyflag);
	}
}
void TaskIO2(void *pParam)
{
	INT8U     *perr;
	INT8U err, i;
	OS_FLAGS rdyflag;
	perr = &err;
	err = OS_ERR_NONE;
	rdyflag = 0;
	while (1)
	{
		OSTimeDly(100);/*1 second delay*/
		for (i = 0; i<10; i++) /*Simulate the process of acquiring data*/
		{
			IO[1][i] = 2;
		}
		printf("Time: %d, after task TaskIO2 gets the IO data, it is ready to submit the event, the current event flag: %d\n", OSTimeGet(), rdyflag);
		rdyflag = OSFlagPost(pFlagGroupDataProcess,
			0x02,
			OS_FLAG_SET,
			perr);            /* Submit the event flag, set bit 1 in the event flag group */
		printf("Time: %d, after task TaskIO2 gets the IO data, submit the event, the current event flag: %d\n", OSTimeGet(), rdyflag);
	}
}
void TaskIO3(void *pParam)
{
	INT8U     *perr;
	INT8U err, i;
	OS_FLAGS rdyflag;
	perr = &err;
	err = OS_ERR_NONE;
	rdyflag = 0;
	while (1)
	{
		OSTimeDly(100);/*1 second delay*/
		for (i = 0; i<10; i++) /*Simulate the process of acquiring data*/
		{
			IO[2][i] = 3;
		}
		printf("Time: %d, after task TaskIO3 gets the IO data, it is ready to submit the event, the current event flag: %d\n", OSTimeGet(), rdyflag);
		rdyflag = OSFlagPost(pFlagGroupDataProcess,
			0x04,
			OS_FLAG_SET,
			perr);            /* Submit the event flag, set bit 2 in the event flag group */
		printf("Time: %d, after task TaskIO3 gets the IO data, submit the event, the current event flag: %d\n", OSTimeGet(), rdyflag);
	}
}
void TaskIO4(void *pParam)
{
	INT8U     *perr;
	INT8U err, i;
	OS_FLAGS rdyflag;
	perr = &err;
	err = OS_ERR_NONE;
	rdyflag = 0;
	while (1)
	{
		OSTimeDly(100);/*1 second delay*/
		for (i = 0; i<10; i++) /*Simulate the process of acquiring data*/
		{
			IO[3][i] = 3;
		}
		printf("Time: %d, after task TaskIO4 gets the IO data, it is ready to submit the event, the current event flag: %d\n", OSTimeGet(), rdyflag);
		rdyflag = OSFlagPost(pFlagGroupDataProcess,
			0x08,
			OS_FLAG_SET,
			perr);            /*Submit the event flag, set bit 3 in the event flag group*/
		printf("Time: %d, after task TaskIO4 gets the IO data, submit the event, the current event flag: %d\n", OSTimeGet(), rdyflag);
	}
}

/* Mutex example program, using priority inversion */
OS_EVENT  *myMutex;
void TaskMutex1(void *pParam)
{
	INT8U     *perr;
	INT8U err, i;
	INT32U j;
	perr = &err;
	err = OS_ERR_NONE;


	myMutex = OSMutexCreate(3, perr);/* Create mutually exclusive semaphore, priority inheritance priority PIP is 9 */
	if (myMutex == (OS_EVENT  *)0)   /* Check if the creation is successful*/
	{
		printf("Time: %d, high priority task TaskMutex1 failed to create mutex semaphore, failure number %d\n", OSTimeGet(), *perr);
		OSTaskDel(OS_PRIO_SELF); /*If unsuccessful, delete this task*/
		return;
	}
	printf("Time: %d, the high priority task TaskMutex1 successfully created the mutex semaphore.\n", OSTimeGet());
	OSTimeDly(100);/*1 second delay*/
	printf("Time: %d, high priority task TaskMutex1 requests a mutex semaphore.\n", OSTimeGet());
	OSMutexPend(myMutex, 0, perr);/*µÈ´ý»¥³âÐÅºÅÁ¿*/
	printf("Time: %d, the high-priority task TaskMutex1 gets the mutex semaphore.\n", OSTimeGet());
	if (*perr == OS_ERR_NONE)
	{

		for (i = 1; i <= 5; i++)
		{
			printf("Time %d: High priority task TaskMutex1 outputs data to the serial port %d\n", OSTimeGet(), i);  /*Analog operation IO*/
			for (j = 1; j <= 9999999; j++);	 /*Ä£Äâ²Ù×÷´®¿Ú*/
		}
	}
	else
	{
		printf("Time: %d, high priority task TaskMutex1 request semaphore failed, failure number %d\n", OSTimeGet(), *perr);
	}
	OSMutexPost(myMutex);
	for (i = 1; i <= 5; i++)
	{

		printf("Time %d: High priority task TaskMutex1 performs other operations after executing the submitted semaphore%d\n", OSTimeGet(), i);	/*Analog operation IO*/
		for (j = 1; j <= 99999999; j++);	 /*ÑÓÊ±,±íÊ¾ÔÚ²Ù×÷´®¿Ú*/
	}
	printf("The high priority task TaskMutex1 ends running and deletes itself\n");
	OSTaskDel(OS_PRIO_SELF); /*Delete this task*/
	return;

}
void TaskMutex2(void *pParam)
{
	INT8U 	*perr;
	INT8U err, i;
	INT32U j;
	perr = &err;
	err = OS_ERR_NONE;
	if (myMutex == (OS_EVENT  *)0)	 /*Check if there is a mutex semaphore created*/
	{
		printf("Time: %d, the mutex semaphore is not created", OSTimeGet());
		OSTaskDel(OS_PRIO_SELF); /*Delete this task*/
		return;
	}
	OSTimeDly(90);/* Delay is less than 1 second*/
	printf("Time: %d, low priority task TaskMutex2 request mutex semaphore\n", OSTimeGet());
	OSMutexPend(myMutex, 0, perr);/*Waiting for mutually exclusive semaphore*/
	printf("Time: %d, task TaskMutex2 gets the mutex semaphore\n", OSTimeGet());
	if (*perr == OS_ERR_NONE)
	{
		printf("Time: %d, the low priority task TaskMutex2 obtains the mutually exclusive semaphore\n", OSTimeGet());
		for (i = 1; i <= 5; i++)
		{

			printf("Time %d: Low priority TaskMutex2 outputs data to the serial port %d\n", OSTimeGet(), i);  /* Analog operation IO */
			for (j = 1; j <= 99999999; j++);   /* Simulate operation serial port */
		}
	}
	else
	{
		printf("Time: %d, low priority task TaskMutex2 request semaphore failed, failure number: %d\n", OSTimeGet(), *perr);
	}
	OSMutexPost(myMutex);
	for (i = 1; i <= 5; i++)
	{
		printf("Time %d: Low priority TaskMutex2 performs other operations after executing the submitted semaphore%d\n", OSTimeGet(), i);  /*Analog operation IO*/
		for (j = 1; j <= 99999999; j++);   /*ÑÓÊ±,±íÊ¾ÔÚ²Ù×÷´®¿Ú*/
	}
	printf("The low priority task TaskMutex2 ends running and deletes itself\n");
	OSTaskDel(OS_PRIO_SELF); /*Delete this task*/
	return;
}

void TaskPrint(void *pParam)
{
	INT8U     *perr;
	INT8U err, i;
	INT32U j;
	perr = &err;
	err = OS_ERR_NONE;
	i = 0;
	OSTimeDly(95);
	for (i = 1; i <= 5; i++)
	{
		printf("Time %d: Medium priority task TaskPrint is running, printing data %d\n", OSTimeGet(), i++);  /*Analog operation IO*/
		for (j = 1; j <= 99999999; j++);   /* Simulate printing operation */
	}
	printf("The medium priority task TaskPrint finishes running and deletes itself\n");
	OSTaskDel(OS_PRIO_SELF); /*Delete this task*/
}

// Message mailbox example
OS_EVENT  *myMBox;
void TaskMessageSen(void *pParam)
{
	INT8U     *perr;
	INT8U err;//,i;  
			  //INT32U j;
	INT32U scount;
	int a[3];
	perr = &err;
	err = OS_ERR_NONE;

	scount = 1;

	a[1] = 5;
	a[3] = 6;
	myMBox = OSMboxCreate(&scount);/* Create a mailbox, */
	if (myMBox == (OS_EVENT  *)0)   /*Check if the creation is successful*/
	{
		printf("Time: %d, TaskMessageSen failed to create mailbox\n", OSTimeGet());
		OSTaskDel(OS_PRIO_SELF); /*If unsuccessful, delete this task*/
		return;
	}
	printf("Time: %d, TaskMessageSen successfully created the mailbox\n", OSTimeGet());
	while (1)
	{
		OSTimeDly(100);/*1 second delay*/
		scount++;
		printf("Time: %d, task TaskMessageSend is ready to send a message, the message is %d\n", OSTimeGet(), scount);
		OSMboxPost(myMBox, &scount); /* Send message */
	}
}
void TaskMessageRec(void *pParam)
{
	INT8U     *perr;
	INT8U err;//,i;  
	INT32U * prcount;
	perr = &err;
	err = OS_ERR_NONE;

	if (myMBox == (OS_EVENT  *)0)   /*Check if the mailbox exists*/
	{
		printf("Time: %d, task TaskMessageRec judged that the mailbox does not exist!\n", OSTimeGet());
		OSTaskDel(OS_PRIO_SELF); /*If unsuccessful, delete this task*/
		return;
	}

	while (1)
	{
		prcount = (INT32U *)OSMboxPend(myMBox, 0, perr); /* Request message, block if the message does not exist */
		if (*perr == OS_ERR_NONE)
			printf("Time: %d, task TaskMessageRec received message as %d\n", OSTimeGet(), *prcount);
		else
			printf("Time: %d, task TaskMessageRec waited for abnormal end, error number: %d\n", OSTimeGet(), *perr);
	}
}

// Example of message queue
OS_EVENT  *myQ;
void TaskQSen(void *pParam)
{
	INT8U     *perr;
	INT8U err, i;
	INT8U scount;
	OS_Q_DATA myQData; /*Message queue data*/
	void * myq[6]; /*message queue*/
	INT8U mymessage[256];
	perr = &err;
	err = OS_ERR_NONE;
	scount = 0;

	myQ = OSQCreate(myq, 6);/*Create a message queue with a volume of 6*/
	if (myQ == (OS_EVENT  *)0)   /*Check if the creation is successful*/
	{
		printf("Time: %d, TaskQSen failed to create a message queue\n", OSTimeGet());
		OSTaskDel(OS_PRIO_SELF); /*If unsuccessful, delete this task*/
		return;
	}
	printf("Time: %d, TaskQSen successfully created the message queue\n", OSTimeGet());
	for (i = 0; i <= 254; i++)
		mymessage[i] = i;
	mymessage[255] = i;
	while (1)
	{
		OSTimeDly(100);

		printf("Time: %d, task TTaskQSen is ready to send a message, the message is %d\n", OSTimeGet(), mymessage[scount]);
		err = OSQPost(myQ, &mymessage[scount]); /*Send message*/
		switch (err) {
		case OS_ERR_Q_FULL:
			printf("Time: %d, task TTaskQSen failed to send a message, the message queue is full\n", OSTimeGet());
			break;
		case OS_ERR_NONE:
			printf("Time: %d, task TTaskQSen sent a message successfully\n", OSTimeGet());
			break;
		default:
			printf("Time: %d, task TTaskQSen failed to send message, error number:%d\n", OSTimeGet(), err);
		}
		OSQQuery(myQ, &myQData);
		printf("Time: %d, the number of messages in the current queue: %d\n", OSTimeGet(), myQData.OSNMsgs);
		scount++;
	}
}

void TaskQRec(void *pParam)
{
	INT8U     *perr;
	INT8U err;
	INT8U rcount;
	INT8U rec;
	OS_Q_DATA myQData; /*Message queue data*/
					   //INT8U mymessage[256];
	perr = &err;
	err = OS_ERR_NONE;
	rcount = 0;


	if (myQ == (OS_EVENT  *)0)   /*Check if the message queue exists*/
	{
		printf("Time: %d, TaskQRec determined that the message queue does not exist\n", OSTimeGet());
		OSTaskDel(OS_PRIO_SELF); /*If unsuccessful, delete this task*/
		return;
	}
	while (1)
	{
		OSTimeDly(200);
		printf("Time: %d, task TaskQRec started waiting for messages\n", OSTimeGet());
		rec = *(INT8U *)OSQPend(myQ, 0, perr);
		switch (err) {
		case OS_ERR_NONE:
			printf("Time: %d, task TaskQRec received message %d\n", OSTimeGet(), rec);
			break;
		default:
			printf("Time: %d, task TTaskMessageSen failed to send message, error number: %d\n", OSTimeGet(), err);
		}
		OSQQuery(myQ, &myQData);
		if (rec == 4)
			printf("4\n");
		printf("Time: %d, the number of messages in the current queue: %d\n", OSTimeGet(), myQData.OSNMsgs);
	}
}

//Examples of memory management
void TaskM(void *pParam)
{

	INT8U *perr;
	INT8U err, i;
	OS_MEM  *pmyMem;     //MCB block address
	INT8U myMem[3][20]; //Used as memory partition
	void    *pblk[10];   //Memory block address array
	BOOLEAN require;
	OS_MEM_DATA mem_data;//Used to query memory block information
	err = OS_ERR_NONE;
	perr = &err;
	require = 1;
	pmyMem = OSMemCreate(myMem, 3, 20, perr);/*Create memory partition, 10 blocks, 20 bytes each*/
	if (pmyMem == (OS_MEM  *)0)   /*Check if the creation is successful*/
	{
		printf("Time: %d, TaskM failed to create memory partition\n", OSTimeGet());
		OSTaskDel(OS_PRIO_SELF); /*If unsuccessful, delete this task*/
		return;
	}
	printf("Time: %d, TaskM successfully created the memory partition, containing 10 blocks, each with 20 bytes\n", OSTimeGet());
	i = 0;
	while (1)
	{
		if (i>5)
		{
			i = 0;
			require = !require;
		}
		//printf("time:%d£¬i=%d\n",OSTimeGet(),i);
		OSTimeDly(100);/*1 second delay*/
		if (require)
		{
			printf("Time: %d, task TaskM is ready to request a block ->", OSTimeGet());
			pblk[i++] = OSMemGet(pmyMem, perr); /*Request memory block*/
			switch (err) {
			case OS_ERR_MEM_NO_FREE_BLKS:
				printf("Time: %d, task TaskM failed to request a memory block, and there is no free memory block in the partition!\n", OSTimeGet());
				break;
			case OS_ERR_NONE:
				printf("Time: %d, task TaskM gets the memory block\n", OSTimeGet());
				break;
			default:
				printf("Time: %d, task TaskM failed to request memory block, error number:%d\n", OSTimeGet(), err);
			}
		}
		else
		{
			printf("Time: %d, task TaskM is ready to release a block ->", OSTimeGet());
			err = OSMemPut(pmyMem, pblk[i++]); /*Request memory block*/
			switch (err) {
			case OS_ERR_MEM_FULL:
				printf("Time: %d, task TaskM failed to request memory block, the partition is full!\n", OSTimeGet());
				break;
			case OS_ERR_MEM_INVALID_PBLK:
				printf("Time: %d, the task TaskM failed to release the memory block, and the invalid memory block was released!\n", OSTimeGet());
				break;
			case OS_ERR_NONE:
				printf("Time: %d, task TaskM successfully released the memory block\n", OSTimeGet());
				break;
			default:
				printf("Time: %d, task TaskM failed to release memory block, error number: %d\n", OSTimeGet(), err);
			}

		}
		OSMemQuery(pmyMem, &mem_data);
		printf("Time: %d, the number of used memory blocks in the current partition: %d\n", OSTimeGet(), mem_data.OSNUsed);
		printf("Time: %d, the number of free memory blocks in the current partition: %d\n", OSTimeGet(), mem_data.OSNFree);
	}
}

