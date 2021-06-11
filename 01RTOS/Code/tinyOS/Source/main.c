#include "tinyOS.h"
#include "ARMCM3.h"

tTask * currentTask;
tTask * nextTask;
tTask * idleTask;

tBitmap taskPrioBitmap;				//位图数据结构
tList taskTable[TINYOS_PRO_COUNT];  //任务链表

uint8_t schedLockCount;		//调度锁

uint32_t tickCount;
tList tTaskDelayedList;		//任务延时链表

uint32_t idleCount;
uint32_t idleMaxCount;

static void initCpuUsageStat (void);
static void checkCpuUsage (void);
static void cpuUsageSyncWithSysTick (void);

tTask * tTaskHighestReady (void)
{
	uint32_t highestPrio = tBitmapGetFirstSet(&taskPrioBitmap);
	tNode * node = tListFirst(&taskTable[highestPrio]);
	return tNodeParent(node, tTask, linkNode);
}

void tTaskSchedInit (void)
{
	int i;
	
	schedLockCount = 0;
	tBitmapInit(&taskPrioBitmap);		//初始化位图
	for (i = 0; i < TINYOS_PRO_COUNT; i++)
	{
		tListInit(&taskTable[i]);		//初始化任务链表
	}
}

//调度锁失能
void tTaskSchedDisable (void)
{
	uint32_t status = tTaskEnterCritical();  
	
	if (schedLockCount < 255)
	{
		schedLockCount++;
	}
	
	tTaskExitCritical(status);
}
//调度锁使能
void tTaskSchedEnable (void)
{
	uint32_t status = tTaskEnterCritical();
	
	if (schedLockCount > 0)
	{
		if (--schedLockCount == 0)
		{
			tTaskSched();
		}
	}
	
	tTaskExitCritical(status);
}

void tTaskSchedRdy (tTask * task)
{
	tListAddFirst(&(taskTable[task->prio]), &(task->linkNode));
	tBitmapSet(&taskPrioBitmap, task->prio);
}

void tTaskSchedUnRdy (tTask * task)
{
	tListRemove(&taskTable[task->prio], &(task->linkNode));  //从任务队列中移除
	if (tListCount(&taskTable[task->prio]) == 0)			 
	{
		//若无相同优先级任务则在位图中清除
		tBitmapClear(&taskPrioBitmap, task->prio);
	}
}

void tTaskSchedRemove (tTask * task)
{
	tListRemove(&taskTable[task->prio], &(task->linkNode));
	if (tListCount(&taskTable[task->prio]) == 0)
	{
		tBitmapClear(&taskPrioBitmap, task->prio);
	}
}

void tTaskSched ()
{
	tTask * tempTask;
	uint32_t status = tTaskEnterCritical(); //临界区保护

	//若调度锁开启则直接退出
	if (schedLockCount > 0)
	{
		tTaskExitCritical(status);
		return;
	}
	
	tempTask = tTaskHighestReady(); //获取优先级最高的就绪任务
	if (tempTask != currentTask)
	{
		nextTask = tempTask;
		tTaskSwitch();		//进行任务调度
	}
		
	tTaskExitCritical(status);	//退出临界区
}

void tTaskDelayedInit (void)
{
	tListInit(&tTaskDelayedList);
}

//设置任务延时
void tTimeTaskWait (tTask * task, uint32_t ticks)
{
	task->delayTicks = ticks;							  //设置延时
	tListAddLast(&tTaskDelayedList, &(task->delayNode));  //添加到延时队列
	task->state |= TINYOS_TASK_STATE_DELAYED;			  //设置延时标志
}

//唤醒延时任务
void tTimeTaskWakeUp (tTask * task)
{
	tListRemove(&tTaskDelayedList, &(task->delayNode));  //从延时队列中移除
	task->state &= ~TINYOS_TASK_STATE_DELAYED;			 //取消延时状态
}

void tTimeTaskRemove (tTask * task)
{
	tListRemove(&tTaskDelayedList, &(task->delayNode));
}

void tTimeTickInit (void)
{
	tickCount = 0;
}

void tTaskSystemTickHandler ()
{
	tNode * node;
	
	uint32_t status = tTaskEnterCritical(); //临界区

	//轮询延时任务队列
	for (node = tTaskDelayedList.headNode.nextNode; node != &(tTaskDelayedList.headNode); node = node->nextNode)
	{
		tTask * task = tNodeParent(node, tTask, delayNode); //获取到任务结构体 
		//延时时间到
		if (--task->delayTicks == 0)
		{
			if (task->waitEvent)
			{
				//若任务正在等待事件，移除等待
				tEventRemoveTask(task, (void *)0, tErrorTimeout);
			}
			
			tTimeTaskWakeUp(task); //唤醒延时任务
			
			tTaskSchedRdy(task);   //将任务添加到就绪链表
		}
	}

	//相同任务优先级的时间片判断
	if (--currentTask->slice == 0)
	{
		if (tListCount(&taskTable[currentTask->prio]) > 0)
		{
			//将时间片到的任务从头部移到尾部，并重置时间片
			tListRemoveFirst(&taskTable[currentTask->prio]);
			tListAddLast(&taskTable[currentTask->prio], &(currentTask->linkNode));
			
			currentTask->slice = TINYOS_SLICE_MAX;
		}
	}
	
	tickCount++;    //Systick节拍计数自加
	checkCpuUsage();
	
	tTaskExitCritical(status);	//退出临界区
	
	tTimerModuleTickNotify();   //定时器模块
	
	tTaskSched();				//进行任务调度
}

static float cpuUsage;
static uint32_t enableCpuUsageState;

static void initCpuUsageStat (void)
{
	idleCount = 0;
	idleMaxCount = 0;
	cpuUsage = 0.0f;
	enableCpuUsageState = 0;
}

static void checkCpuUsage (void)
{
	if (enableCpuUsageState == 0)
	{
		enableCpuUsageState = 1;
		tickCount = 0;
		return;
	}
	
	if (tickCount == TICKS_PER_SEC)
	{
		idleMaxCount = idleCount;
		idleCount = 0;
		
		tTaskSchedEnable();
	} 
	else if (tickCount % TICKS_PER_SEC == 0)
	{
		cpuUsage = 100 - (idleCount * 100.0 / idleMaxCount);
		idleCount = 0;
	}
}

static void cpuUsageSyncWithSysTick (void)
{
	while (enableCpuUsageState == 0)
	{
		;;
	}
}

float tCpuUsageGet (void)
{
	float usage = 0;
	
	uint32_t status = tTaskEnterCritical();
	usage = cpuUsage;
	tTaskExitCritical(status);
	
	return usage;
}


tTask tTaskIdle; 									//空闲任务句柄
tTaskStack idleTaskEnv[TINYOS_IDLETASK_STACK_SIZE]; //空闲任务栈

//空闲任务入口
void idleTaskEntry (void * param) {
	tTaskSchedDisable();  //关闭调度锁
	
	tInitApp();			  //初始化各个任务
	
	tTimerInitTask();	  //初始化定时器模块任务

	tSetSysTickPeriod(TINYOS_SYSTICK_MS);
	
	cpuUsageSyncWithSysTick();
	
	for (;;)
	{
		uint32_t status = tTaskEnterCritical();
		idleCount++;
		tTaskExitCritical(status);
	}
}

int main ()
{
	tTaskSchedInit();		//任务调度初始化
	
	tTaskDelayedInit();		//初始化任务延时链表
	
	tTimerModuleInit();		//定时器模块
	
	tTimeTickInit();		//Systick定时器计数初始化
	
	initCpuUsageStat();		//初始化CPU使用状态
	
	tTaskInit(&tTaskIdle, idleTaskEntry, (void *)0, TINYOS_PRO_COUNT - 1, idleTaskEnv, TINYOS_IDLETASK_STACK_SIZE);
	idleTask = &tTaskIdle;	//初始化空闲任务

	nextTask = tTaskHighestReady();	//获取任务优先级最高的任务
	
	tTaskRunFirst();		//开始运行
	
	return 0;
}
