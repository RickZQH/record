#include "tSem.h"
#include "tinyOS.h"

void tSemInit (tSem * sem, uint32_t startCount, uint32_t maxCount)
{
	tEventInit(&sem->event, tEventTypeSem);  //初始化事件
	
	sem->maxCount = maxCount;
	if (maxCount == 0)
	{
		sem->count = startCount;
	}
	else 
	{
		sem->count = (startCount > maxCount) ? maxCount : startCount;
	}
}

uint32_t tSemWait (tSem * sem, uint32_t waitTicks)
{
	uint32_t status = tTaskEnterCritical();

	//获取信号量成功
	if (sem->count > 0)
	{
		--sem->count;
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	else
	{
		//获取失败则开始等待事件
		tEventWait(&sem->event, currentTask, (void *)0, tEventTypeSem, waitTicks); 
		tTaskExitCritical(status);
		
		tTaskSched();
		
		return currentTask->waitEventResult;
	}
}

//获取信号量但不等待
uint32_t tSemNoWaitGet (tSem * sem)
{
	uint32_t status = tTaskEnterCritical();
	
	if (sem->count > 0)
	{
		--sem->count;
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	else 
	{
		tTaskExitCritical(status);
		return tErrorResourceUnavaliable;
	}
}

//信号量通知
void tSemNotify (tSem * sem)
{
	uint32_t status = tTaskEnterCritical();
	
	if (tEventWaitCount(&sem->event) > 0)
	{
		//当有任务等待信号释放时
		tTask * task = tEventWakeUp(&sem->event, (void *)0, tErrorNoError);
		if (task->prio < currentTask->prio)
		{
			tTaskSched();
		}
	}
	else
	{
		//释放信号量
		++sem->count;
		if ((sem->maxCount != 0) && (sem->count > sem->maxCount))
		{
			sem->count = sem->maxCount;
		}
	}
	tTaskExitCritical(status);
}

//获取信号量信息
void tSemGetInfo (tSem * sem, tSemInfo * info)
{
	uint32_t status = tTaskEnterCritical();
	
	info->count = sem->count;
	info->maxCount = sem->maxCount;
	info->taskCount = tEventWaitCount(&sem->event);
	
	tTaskExitCritical(status);
}

//删除信号量
uint32_t tSemDestroy (tSem * sem)
{
	uint32_t status = tTaskEnterCritical();
	uint32_t count = tEventRemoveAll(&sem->event, (void *)0, tErrorDel);
	sem->count = 0;
	tTaskExitCritical(status);
	
	if (count > 0)
	{
		tTaskSched();
	}
	
	return count;
}

