#ifndef TTIMER_H
#define	TTIMER_H

#include "tEvent.h"

//定时器状态
typedef enum _tTimerState
{
	tTimerCreated,		//创建
	tTimerStarted,		//开始
	tTimerRunning,		//运行
	tTimerStopped,		//停止
	tTimerDestroyed		//销毁
}tTimerState;

//定时器结构体
typedef struct _tTimer
{
	tNode linkNode;						//链表节点
	uint32_t startDelayTicks;			
	uint32_t durationTicks;
	uint32_t delayTicks;
	void (*timerFunc) (void * arg);	    //回调函数
	void * arg;							//回调函数参数
	uint32_t config;					//定时器类型：硬件or软件
	
	tTimerState state;					//定时器状态
}tTimer;

typedef struct _tTimerInfo
{
	uint32_t startDelayTicks;
	uint32_t durationTicks;
	void (*timerFunc) (void * arg);
	void * arg;
	uint32_t config;
	tTimerState state;
}tTimerInfo;

//定时器类型
#define	TIMER_CONFIG_TYPE_HARD		(1 << 0)	//硬件定时器
#define	TIMER_CONFIG_TYPE_SOFT		(0 << 0)	//软件定时器

void tTimerInit (tTimer * timer, uint32_t delayTicks, uint32_t durationTicks,
		void (*timerFunc) (void * arg), void * arg, uint32_t config);
void tTimerStart (tTimer * timer);
void tTimerStop (tTimer * timer);
void tTimerDestroy (tTimer * timer);
void tTimerGetInfo (tTimer * timer, tTimerInfo * info);
void tTimerModuleTickNotify (void);
void tTimerModuleInit (void);
void tTimerInitTask (void);

#endif
