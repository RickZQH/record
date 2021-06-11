#ifndef TTASK_H
#define	TTASK_H

//任务状态
#define	TINYOS_TASK_STATE_RDY					0				//就绪
#define	TINYOS_TASK_STATE_DESTORYED				(1 << 1)		//删除
#define	TINYOS_TASK_STATE_DELAYED				(1 << 2)		//延时
#define	TINYOS_TASK_STATE_SUSPEND				(1 << 3)		//挂起

#define	TINYOS_TASK_WAIT_MASK					(0xFF << 16)

struct _tEvent;

typedef uint32_t tTaskStack;

//任务句柄
typedef struct _tTask {
	tTaskStack * stack;
	uint32_t * stackBase;  //栈地址
	uint32_t stackSize;	   //栈大小
	tNode linkNode;		   //任务链表节点
	uint32_t delayTicks;
	tNode delayNode;	   //延时任务链表节点
	uint32_t prio;		   //任务优先级
	uint32_t state;		   //任务状态
	uint32_t slice; 	   //同任务优先级时间片
	uint32_t suspendCount; //挂起计数(标志)
	
	void (*clean) (void * param);	//清除回调函数
	void * cleanParam;				//清除函数参数
	uint8_t requestDeleteFlag;		//请求删除任务标志
	
	struct _tEvent * waitEvent;  	//等待事件
	void * eventMsg;				//事件消息
	uint32_t waitEventResult;		//事件结果(标志)
	
	uint32_t waitFlagsType;
	uint32_t eventFlags;
}tTask;

//任务信息
typedef struct _tTaskInfo {
	uint32_t delayTicks;
	uint32_t prio;
	uint32_t state;
	uint32_t slice;
	uint32_t suspendCount;
	
	uint32_t stackSize;
	uint32_t stackFree;		//剩下的任务栈
}tTaskInfo;

void tTaskInit (tTask * task, void (*entry)(void *), void * param, uint32_t prio, tTaskStack * stack, uint32_t size);
void tTaskSuspend (tTask * task);
void tTaskWakeUp (tTask * task);
void tTaskSetCleanCallFunc (tTask * task, void (*clean)(void * param), void * param);
void tTaskForceDelete (tTask * task);
void tTaskRequestDelete (tTask * task);
uint8_t tTaskIsRequestedDeleted (void);
void tTaskDeleteSelf (void);
void tTaskGetInfo (tTask * task, tTaskInfo * info);

#endif
