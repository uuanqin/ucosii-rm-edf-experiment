/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*

*********************************************************************************************************
*/
/*
for book:嵌入式实时操作系统μCOS原理与实践
卢有亮
2011于成都电子科技大学
*/
#include "includes.h"


/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  TASK_STK_SIZE               512       /* Size of each task's stacks (# of WORDs)            */
#define TaskStart_Prio	1
#define Task1_Prio		2


OS_STK  TaskStk[OS_MAX_TASKS][TASK_STK_SIZE];    // Tasks stacks
HANDLE mainhandle;		//主线程句柄
CONTEXT Context;		//主线程切换上下文
BOOLEAN FlagEn = 1;		//增加一个全局变量，做为是否时钟调度的标志


void TaskStart(void * pParam) ;
//void Task1(void * pParam) ;                            /* Function prototypes of tasks                  */

void VCInit(void);						//初始化相关变量,一定需要

/*$PAGE*/
/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/

// Lab2 TCB扩展信息
/*
typedef struct tcb_ext_info{
	INT32U c;      // 保存C
	INT32U t;      // 保存T 周期
	INT32U d;      // 保存D 截止时间（EDF算法）
	INT32U rest_c; // 保存剩余工作时间
	INT32U rest_t; // 保存周期内剩余的事件
	INT32U rest_d; // 保存当前时间与截至时间的差值（EDF算法）
	INT32U task_id;// 任务ID
} tcb_ext_info;
*/
#define LAB2_EDF

int main(int argc, char **argv)
{
	int TasksNum,i,C,T;
#ifdef LAB2_EDF
	int D;
#endif

	tcb_ext_info* task_info_array = (tcb_ext_info*)malloc(sizeof(tcb_ext_info)*TasksNum);

	VCInit();

	// 输入提示
#ifdef LAB2_EDF
    printf("EDF算法实验。请输入任务数:\n");
#else
	printf("RM 算法实验。请输入任务数:\n");
#endif

	scanf("%d",&TasksNum);
    if (TasksNum<0)
	{
		printf("无效的输入!");
        return(1); 	
	}
	OSInit();

	
	for(i=0;i<TasksNum;i++){
#ifdef LAB2_EDF
		// EDF 每个任务三个参数
		scanf("%d %d %d",&C,&D,&T);
#else
		// RM 每个任务两个参数
		scanf("%d %d",&C,&T);
#endif
		
		task_info_array[i].c = C;
		task_info_array[i].t = T;

		task_info_array[i].rest_c = C;
		task_info_array[i].rest_t = T;

		task_info_array[i].task_id = i;
		task_info_array[i].is_pend = 0;  // 初始为不挂起

#ifdef LAB2_EDF
		task_info_array[i].d = D;
		task_info_array[i].rest_d = D;
#endif
			
		OSTaskCreateExt(
			FirstTask, 
			0, 
			&TaskStk[i+5][TASK_STK_SIZE-1], 
			i+5,  //设置互不冲突的数即可
			i,  // 任务ID
			&TaskStk[i+5][0], 
			TASK_STK_SIZE,
			(void *)&task_info_array[i],
			OS_TASK_OPT_NONE //	OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR // opt
		);
		
	}

	timeSetEvent(1000/OS_TICKS_PER_SEC, 0, OSTickISRuser, 0, TIME_PERIODIC);

 
	OSStart();	       
	return(0);
}

void VCInit(void)
{
	HANDLE cp,ct;
	Context.ContextFlags = CONTEXT_CONTROL;
	cp = GetCurrentProcess();	//得到当前进程句柄
	ct = GetCurrentThread();	//得到当前线程伪句柄
	DuplicateHandle(cp, ct, cp, &mainhandle, 0, TRUE, 2);	//伪句柄转换,得到线程真句柄
		
}

/*

void TaskStart(void * pParam) 
{	
	char err;	
	OS_EVENT *sem1;
    
	// 模拟设置定时器中断。开启一个定时器线程,每秒中断100次,中断服务程序OSTickISRuser 
	timeSetEvent(1000/OS_TICKS_PER_SEC, 0, OSTickISRuser, 0, TIME_PERIODIC);
	OSStatInit(); // 统计任务初始化
	sem1 = OSSemCreate(0); 
	OSSemPend(sem1, 0, &err);   //等待事件发生，被阻塞；
}

*/
