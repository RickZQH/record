#include "tinyOS.h"


void tTaskInit (tTask * task, void (*entry)(void *), void * param, uint32_t prio, tTaskStack * stack, uint32_t size)
{
	uint32_t * stackTop;
	
	task->stackBase = stack;
	task->stackSize = size;
	memset(stack, 0, size);
	
	stackTop = stack + size / sizeof(tTaskStack);  //栈顶

	//设置栈帧
	*(--stackTop) = (unsigned long)(1 << 24);  //xPSR寄存器
	*(--stackTop) = (unsigned long)entry;      //返回地址	
	*(--stackTop) = (unsigned long)0x14;	   //LR
	*(--stackTop) = (unsigned long)0x12;	   //R12
	*(--stackTop) = (unsigned long)0x3;	 	   //R3
	*(--stackTop) = (unsigned long)0x2;		   //R2
	*(--stackTop) = (unsigned long)0x1;		   //R1
	*(--stackTop) = (unsigned long)param;	   //R0，任务参数	

	//设置R4-R11寄存器的值
	*(--stackTop) = (unsigned long)0x11;
	*(--stackTop) = (unsigned long)0x10;
	*(--stackTop) = (unsigned long)0x9;
	*(--stackTop) = (unsigned long)0x8;
	*(--stackTop) = (unsigned long)0x7;
	*(--stackTop) = (unsigned long)0x6;
	*(--stackTop) = (unsigned long)0x5;
	*(--stackTop) = (unsigned long)0x4;
	
	task->slice = TINYOS_SLICE_MAX;			//时间片
	task->stack = stackTop;					//任务栈
	task->delayTicks = 0;					//任务延时
	task->prio = prio;						//任务优先级
	task->state = TINYOS_TASK_STATE_RDY;	//任务状态
	task->suspendCount = 0;					//调度计数
	task->clean = (void (*)(void *))0;		//清除回调函数
	task->cleanParam = (void *)0;			//清除回调函数参数
	task->requestDeleteFlag = 0;			//请求删除标志
	
	tNodeInit(&(task->delayNode));			//链表节点初始化
	tNodeInit(&(task->linkNode));	
	
	tTaskSchedRdy(task);					//加入调度就绪列表
}

//挂起任务
void tTaskSuspend (tTask * task)
{
	uint32_t status = tTaskEnterCritical();	//临界区

	//挂起的任务不能处于延时状态
	if (!(task->state & TINYOS_TASK_STATE_DELAYED)) 
	{
		//第一次挂起时
		if (++task->suspendCount <= 1)
		{
			task->state |= TINYOS_TASK_STATE_SUSPEND; //设置任务为挂起状态
			tTaskSchedUnRdy(task);					  //从就绪列表中移除
			if (task == currentTask)
			{
				tTaskSched();						  //当挂起当前任务则进行调度
			}
		}
	}
	
	tTaskExitCritical(status);			   //退出临界区
}

//挂起唤醒
void tTaskWakeUp (tTask * task)
{
	uint32_t status = tTaskEnterCritical();	    	//进入临界区
	
	if (task->state & TINYOS_TASK_STATE_SUSPEND)
	{
		if (--task->suspendCount == 0)		
		{
			task->state &= ~TINYOS_TASK_STATE_SUSPEND;  //当挂起计数为0，清除挂起标志
			tTaskSchedRdy(task);						//重新加入任务就绪队列
			tTaskSched();								//进行任务调度
		}
	}
	
	tTaskExitCritical(status);	//退出临界区
}

//设置任务清除回调函数及参数
void tTaskSetCleanCallFunc (tTask * task, void (*clean)(void * param), void * param)
{
	task->clean = clean;
	task->cleanParam = param;
}

//强制删除任务
void tTaskForceDelete (tTask * task)
{
	uint32_t status = tTaskEnterCritical();		//进入临界区
	
	if (task->state & TINYOS_TASK_STATE_DELAYED)
	{
		tTimeTaskRemove(task);					//从延时任务队列中移除
	}
	else if (!(task->state & TINYOS_TASK_STATE_SUSPEND))
	{
		tTaskSchedRemove(task);					//从任务队列中移除
	}
	
	if (task->clean)
	{
		task->clean(task->cleanParam);			//执行清除回调函数
	}
	
	if (currentTask == task)
	{
		tTaskSched();							//若是删除当前任务进行任务调度
	}
	
	tTaskExitCritical(status);					//退出临界区
}

//请求删除任务
void tTaskRequestDelete (tTask * task)
{
	uint32_t status = tTaskEnterCritical();
	
	task->requestDeleteFlag = 1;
	
	tTaskExitCritical(status);
}

//返回请求删除函数
uint8_t tTaskIsRequestedDeleted (void)
{
	uint8_t delete;
	
	uint32_t status = tTaskEnterCritical();
	
	delete = currentTask->requestDeleteFlag;
	
	tTaskExitCritical(status);
	
	return delete;
}

//删除当前任务
void tTaskDeleteSelf (void)
{
	uint32_t status = tTaskEnterCritical();
	
	tTaskSchedRemove(currentTask);	//从任务队列中移除
	
	if (currentTask->clean)
	{
		currentTask->clean(currentTask->cleanParam);
	}
	
	tTaskSched();					//进行任务调度
	
	tTaskExitCritical(status);
}

//返回任务信息
void tTaskGetInfo (tTask * task, tTaskInfo * info)
{
	uint32_t * stackEnd;
	uint32_t status = tTaskEnterCritical();
	
	info->delayTicks = task->delayTicks;
	info->prio = task->prio;
	info->state = task->state;
	info->slice = task->slice;
	info->suspendCount = task->suspendCount;
	info->stackSize = task->stackSize;
	
	info->stackFree = 0;
	stackEnd = task->stackBase;
	while ((*stackEnd++ == 0) && (stackEnd <= task->stackBase + task->stackSize / sizeof(tTaskStack)))
	{
		info->stackFree++;
	}
	info->stackFree *= sizeof(tTaskStack);
	
	tTaskExitCritical(status);
}


