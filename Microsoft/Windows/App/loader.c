#include <os.h>
#include "luyouliang.h"
#include "usercode.h"

#define TASK_STK_SIZE   512
OS_STK	TaskStk[OS_MAX_TASKS][TASK_STK_SIZE];

int MenuLoader(void)
{
	int Experiment;

	printf("0.No user tasks\n");
	printf("1.The first example, a user task\n");
	printf("2.In the second example, two tasks share the CPU alternately\n");
	printf("3.The third example, suspension and resume of tasks\n");
	printf("4.The fourth example, semaphore management\n");
	printf("5.The fifth example, mutual exclusion semaphore management\n");
	printf("6.The sixth example, event flag group\n");
	printf("7.The seventh example, message mailbox\n");
	printf("8.The eighth example, message queue\n");
	printf("9.The nineth example, memory management\n");
	printf("10. User hello world\n");

	printf("Please enter the serial number selection example:\n");
	scanf_s("%d", &Experiment);
	if ((Experiment<0) || (Experiment>10))
	{
		printf("Invalid input!");
		return 1;
	}

	OSInit();
	switch (Experiment)
	{
	case 1://One task run
		OSTaskCreate(FirstTask, 0, &TaskStk[5][TASK_STK_SIZE - 1], 5);
		break;
	case 2://Two tasks share CPU
		OSTaskCreate(E2_task1, 0, &TaskStk[5][TASK_STK_SIZE - 1], 5);
		OSTaskCreate(E2_task2, 0, &TaskStk[6][TASK_STK_SIZE - 1], 6);
		break;
	case 3://Suspend and resume tasks
		OSTaskCreate(E3_task0, 0, &TaskStk[5][TASK_STK_SIZE - 1], 5);
		OSTaskCreate(E3_task1, 0, &TaskStk[6][TASK_STK_SIZE - 1], 6);
		OSTaskCreate(E3_task2, 0, &TaskStk[7][TASK_STK_SIZE - 1], 7);
		break;
	case 4://Semaphore management routines
		OSTaskCreate(UserTaskSemA, 0, &TaskStk[5][TASK_STK_SIZE - 1], 7);
		OSTaskCreate(UserTaskSemB, 0, &TaskStk[6][TASK_STK_SIZE - 1], 6);
		OSTaskCreate(UserTaskSemC, 0, &TaskStk[7][TASK_STK_SIZE - 1], 5);
		break;
	case 5://Mutex semaphore management routines
		OSTaskCreate(TaskMutex1, 0, &TaskStk[6][TASK_STK_SIZE - 1], 6);
		OSTaskCreate(TaskMutex2, 0, &TaskStk[7][TASK_STK_SIZE - 1], 50);
		OSTaskCreate(TaskPrint, 0, &TaskStk[8][TASK_STK_SIZE - 1], 30);
		break;
	case 6://Event flag group management routine
		OSTaskCreate(TaskDataProcess, 0, &TaskStk[5][TASK_STK_SIZE - 1], 5);
		OSTaskCreate(TaskIO1, 0, &TaskStk[6][TASK_STK_SIZE - 1], 6);
		OSTaskCreate(TaskIO2, 0, &TaskStk[7][TASK_STK_SIZE - 1], 7);
		OSTaskCreate(TaskIO3, 0, &TaskStk[8][TASK_STK_SIZE - 1], 8);
		OSTaskCreate(TaskIO4, 0, &TaskStk[9][TASK_STK_SIZE - 1], 9);
		break;
	case 7://Message mailbox
		OSTaskCreate(TaskMessageSen, 0, &TaskStk[6][TASK_STK_SIZE - 1], 6);
		OSTaskCreate(TaskMessageRec, 0, &TaskStk[7][TASK_STK_SIZE - 1], 7);
		break;
	case 8://message queue
		OSTaskCreate(TaskQSen, 0, &TaskStk[7][TASK_STK_SIZE - 1], 5);
		OSTaskCreate(TaskQRec, 0, &TaskStk[8][TASK_STK_SIZE - 1], 6);
		OSTaskCreate(TaskQRec, 0, &TaskStk[9][TASK_STK_SIZE - 1], 7);
		break;
	case 9://Memory management
		OSTaskCreate(TaskM, 0, &TaskStk[8][TASK_STK_SIZE - 1], 6);
		break;
	case 10:
		OSTaskCreate(helloworld, 0, &TaskStk[6][TASK_STK_SIZE - 1], 10);
		break;
	default:
		;
	}
	return 0;
}
