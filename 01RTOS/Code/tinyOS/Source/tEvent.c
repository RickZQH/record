#include "tinyOS.h"

//初始化事件
void tEventInit (tEvent * event, tEventType type)
{
	event->type = tEventTypeUnknow;
	tListInit(&event->waitList);	//初始化节点
}

//等待事件
void tEventWait (tEvent * event, tTask * task, void * msg, uint32_t state, uint32_t timeout)
{
	uint32_t status = tTaskEnterCritical();
	
	task->state |= state << 16;		
	task->waitEvent = event;		//等待的事件
	task->eventMsg = msg;			//事件消息
	task->waitEventResult = tErrorNoError;
	
	tTaskSchedUnRdy(task);							  //将任务从就绪表中移除
	
	tListAddLast(&event->waitList, &task->linkNode);  //加入事件任务链表
	
	if (timeout)
	{
		tTimeTaskWait(task, timeout);				  //设置任务延时
	}
	
	tTaskExitCritical(status);
}

//事件唤醒
tTask * tEventWakeUp (tEvent * event, void * msg, uint32_t result)
{
	tNode * node;
	tTask * task = (tTask *)0;
	
	uint32_t status = tTaskEnterCritical();
	
	if ((node = tListRemoveFirst(&event->waitList)) != (tNode *)0)
	{
		task = (tTask *)tNodeParent(node, tTask, linkNode);
		task->waitEvent = (tEvent *)0;			//清除等待事件
		task->eventMsg = msg;					//设置事件消息
		task->waitEventResult = result;		
		task->state &= ~TINYOS_TASK_WAIT_MASK;	//清除事件
		
		if (task->delayTicks != 0)
		{
			tTimeTaskWakeUp(task);				//从定时器队列中唤醒
		}
		
		tTaskSchedRdy(task);					//将任务添加到就绪列表
	}
	
	tTaskExitCritical(status);
	
	return task;
}

//事件唤醒任务
tTask * tEventWakeUpTask (tEvent * event, tTask * task, void * msg, uint32_t result)
{	
	uint32_t status = tTaskEnterCritical();
	
	tListRemove(&event->waitList, &task->linkNode);		//从事件链表中移除
	task->waitEvent = (tEvent *)0;
	task->eventMsg = msg;
	task->waitEventResult = result;
	task->state &= ~TINYOS_TASK_WAIT_MASK;				
		
	if (task->delayTicks != 0)
	{
		tTimeTaskWakeUp(task);
	}
		
	tTaskSchedRdy(task);
	
	tTaskExitCritical(status);

	return task;
}

//移除
void tEventRemoveTask (tTask *task, void * msg, uint32_t result)
{
	uint32_t status = tTaskEnterCritical();
	
	tListRemove(&task->waitEvent->waitList, &task->linkNode);
	task->waitEvent = (tEvent *)0;
	task->eventMsg = msg;
	task->waitEventResult = result;
	task->state &= ~TINYOS_TASK_WAIT_MASK;

	tTaskExitCritical(status);
}

//移除全部
uint32_t tEventRemoveAll (tEvent * event, void * msg, uint32_t result)
{
	tNode * node;
	uint32_t count = 0;
	
	uint32_t status = tTaskEnterCritical();
	
	count = tListCount(&event->waitList);
	
	while ((node = tListRemoveFirst(&event->waitList)) != (tNode *)0)
	{
		tTask * task = (tTask *)tNodeParent(node, tTask, linkNode);
		task->waitEvent = (tEvent *)0;
		task->eventMsg = msg;
		task->waitEventResult = result;
		task->state &= ~TINYOS_TASK_WAIT_MASK;
		
		if (task->delayTicks != 0)
		{
			tTimeTaskWakeUp(task);
		}
		
		tTaskSchedRdy(task);
	}
	
	tTaskExitCritical(status);
	
	return count;
}

//等待事件的任务个数
uint32_t tEventWaitCount (tEvent * event)
{
	uint32_t count = 0;
	
	uint32_t status = tTaskEnterCritical();
	count = tListCount(&event->waitList);
	tTaskExitCritical(status);
	
	return count;
}

