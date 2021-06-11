#include "tinyOS.h"


void tMboxInit (tMbox * mbox, void **msgBufer, uint32_t maxCount)
{
	tEventInit(&mbox->event, tEventTypeMbox);
	
	mbox->msgBuffer = msgBufer;
	mbox->maxCount = maxCount;
	mbox->read = 0;
	mbox->write = 0;
	mbox->count = 0;
}

//等待邮箱消息
uint32_t tMboxWait (tMbox * mbox, void **msg, uint32_t waitTicks)
{
	uint32_t status = tTaskEnterCritical();

	//邮箱存在消息，获取消息
	if (mbox->count > 0)
	{
		--mbox->count;
		*msg = mbox->msgBuffer[mbox->read++];
		if (mbox->read >= mbox->maxCount)
		{
			mbox->read = 0;
		}
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	//不存在消息
	else
	{
		tEventWait(&mbox->event, currentTask, (void *)0, tEventTypeMbox, waitTicks);
		tTaskExitCritical(status);
		
		tTaskSched();
		
		*msg = currentTask->eventMsg;
		return currentTask->waitEventResult;
	}
}

//不阻塞获取邮箱消息
uint32_t tMboxNoWaitGet (tMbox * mbox, void **msg)
{
	uint32_t status = tTaskEnterCritical();
	
	if (mbox->count > 0)
	{
		--mbox->count;
		*msg = mbox->msgBuffer[mbox->read++];
		if (mbox->read >= mbox->maxCount)
		{
			mbox->read = 0;
		}
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	else
	{
		tTaskExitCritical(status);
		return tErrorResourceUnavaliable;
	}
}

//发送邮箱消息
uint32_t tMboxNotify (tMbox * mbox, void * msg, uint32_t notifyOption)
{
	uint32_t status = tTaskEnterCritical();

	//若有任务等待
	if (tEventWaitCount(&mbox->event) > 0)
	{
		tTask * task = tEventWakeUp(&mbox->event, (void *)msg, tErrorNoError);
		if (task->prio < currentTask->prio)
		{
			tTaskSched();
		}
	}
	else
	{
		//以达到邮箱最大等待次数
		if (mbox->count >= mbox->maxCount)
		{
			tTaskExitCritical(status);
			return tErrorResourceFull;
		}
		
		if (notifyOption & tMBOXSendFront)
		{
			if (mbox->read <= 0)
			{
				mbox->read = mbox->maxCount - 1;
			}
			else
			{
				--mbox->read;
			}
			mbox->msgBuffer[mbox->read] = msg;
		}
		else
		{
			mbox->msgBuffer[mbox->write++] = msg;
			if (mbox->write >= mbox->maxCount)
			{
				mbox->write = 0;
			}
		}
		
		mbox->count++;
	}
	
	tTaskExitCritical(status);
	return tErrorNoError;
}

//清空所有的邮箱消息
void tMboxFlush (tMbox * mbox)
{
	uint32_t status = tTaskEnterCritical();
	
	if (tEventWaitCount(&mbox->event) == 0)
	{
		mbox->read = 0;
		mbox->write = 0;
		mbox->count = 0;
	}
	
	tTaskExitCritical(status);
}

//销毁邮箱
uint32_t tMboxDestroy (tMbox * mbox)
{
	uint32_t status = tTaskEnterCritical();
	uint32_t count = tEventRemoveAll(&mbox->event, (void *)0, tErrorDel);
	tTaskExitCritical(status);
	
	if (count > 0)
	{
		tTaskSched();
	}
	
	return count;
}

//获取邮箱信息
void tMboxGetInfo (tMbox * mbox, tMboxInfo * info)
{
	uint32_t status = tTaskEnterCritical();
	
	info->count = mbox->count;
	info->maxCount = mbox->maxCount;
	info->taskCount = tEventWaitCount(&mbox->event);
	
	tTaskExitCritical(status);
}

