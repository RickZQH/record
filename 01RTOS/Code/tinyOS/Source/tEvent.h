#ifndef TEVENT_H
#define	TEVENT_H

#include "tLib.h"
#include "tTask.h"

//事件类型
typedef enum _tEventType {
	tEventTypeUnknow,	//未知事件
	tEventTypeSem,		//信号量
	tEventTypeMbox,		//邮箱
	tEventTypeMemBlock,
	tEventTypeFlagGroup,
	tEventTypeMutex
}tEventType;

//事件结构体
typedef struct _tEvent {
	tEventType type;		//事件类型
	tList waitList;			//链表节点
}tEvent;

void tEventInit (tEvent * event, tEventType type);
void tEventWait (tEvent * event, tTask * task, void * msg, uint32_t state, uint32_t timeout);
tTask * tEventWakeUp (tEvent * event, void * msg, uint32_t result);
tTask * tEventWakeUpTask (tEvent * event, tTask * task, void * msg, uint32_t result);
void tEventRemoveTask (tTask *task, void * msg, uint32_t result);
uint32_t tEventWaitCount (tEvent * event);
uint32_t tEventRemoveAll (tEvent * event, void * msg, uint32_t result);

#endif
