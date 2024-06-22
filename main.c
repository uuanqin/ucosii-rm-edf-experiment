/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*

*********************************************************************************************************
*/
/*
for book:Ƕ��ʽʵʱ����ϵͳ��COSԭ����ʵ��
¬����
2011�ڳɶ����ӿƼ���ѧ
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
HANDLE mainhandle;		//���߳̾��
CONTEXT Context;		//���߳��л�������
BOOLEAN FlagEn = 1;		//����һ��ȫ�ֱ�������Ϊ�Ƿ�ʱ�ӵ��ȵı�־


void TaskStart(void * pParam) ;
//void Task1(void * pParam) ;                            /* Function prototypes of tasks                  */

void VCInit(void);						//��ʼ����ر���,һ����Ҫ

/*$PAGE*/
/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/

// Lab2 TCB��չ��Ϣ
/*
typedef struct tcb_ext_info{
	INT32U c;      // ����C
	INT32U t;      // ����T ����
	INT32U d;      // ����D ��ֹʱ�䣨EDF�㷨��
	INT32U rest_c; // ����ʣ�๤��ʱ��
	INT32U rest_t; // ����������ʣ����¼�
	INT32U rest_d; // ���浱ǰʱ�������ʱ��Ĳ�ֵ��EDF�㷨��
	INT32U task_id;// ����ID
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

	// ������ʾ
#ifdef LAB2_EDF
    printf("EDF�㷨ʵ�顣������������:\n");
#else
	printf("RM �㷨ʵ�顣������������:\n");
#endif

	scanf("%d",&TasksNum);
    if (TasksNum<0)
	{
		printf("��Ч������!");
        return(1); 	
	}
	OSInit();

	
	for(i=0;i<TasksNum;i++){
#ifdef LAB2_EDF
		// EDF ÿ��������������
		scanf("%d %d %d",&C,&D,&T);
#else
		// RM ÿ��������������
		scanf("%d %d",&C,&T);
#endif
		
		task_info_array[i].c = C;
		task_info_array[i].t = T;

		task_info_array[i].rest_c = C;
		task_info_array[i].rest_t = T;

		task_info_array[i].task_id = i;
		task_info_array[i].is_pend = 0;  // ��ʼΪ������

#ifdef LAB2_EDF
		task_info_array[i].d = D;
		task_info_array[i].rest_d = D;
#endif
			
		OSTaskCreateExt(
			FirstTask, 
			0, 
			&TaskStk[i+5][TASK_STK_SIZE-1], 
			i+5,  //���û�����ͻ��������
			i,  // ����ID
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
	cp = GetCurrentProcess();	//�õ���ǰ���̾��
	ct = GetCurrentThread();	//�õ���ǰ�߳�α���
	DuplicateHandle(cp, ct, cp, &mainhandle, 0, TRUE, 2);	//α���ת��,�õ��߳�����
		
}

/*

void TaskStart(void * pParam) 
{	
	char err;	
	OS_EVENT *sem1;
    
	// ģ�����ö�ʱ���жϡ�����һ����ʱ���߳�,ÿ���ж�100��,�жϷ������OSTickISRuser 
	timeSetEvent(1000/OS_TICKS_PER_SEC, 0, OSTickISRuser, 0, TIME_PERIODIC);
	OSStatInit(); // ͳ�������ʼ��
	sem1 = OSSemCreate(0); 
	OSSemPend(sem1, 0, &err);   //�ȴ��¼���������������
}

*/
