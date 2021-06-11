#ifndef TMBOX_H
#define	TMBOX_H

#include "tEvent.h"

#define	tMBOXSendNormal			0x00	//正常发送
#define	tMBOXSendFront			0x01	//优先发送

typedef struct _tMbox
{
	tEvent event;		//继承事件
	uint32_t count;		//计数
	uint32_t read;		//读索引
	uint32_t write;		//写索引
	uint32_t maxCount;	//最大计数
	void ** msgBuffer;	//缓存
}tMbox;

typedef struct _tMboxInfo {
	uint32_t count;
	uint32_t maxCount;
	uint32_t taskCount;
}tMboxInfo;

void tMboxInit (tMbox * mbox, void **msgBufer, uint32_t maxCount);
uint32_t tMboxWait (tMbox * mbox, void **msg, uint32_t waitTicks);
uint32_t tMboxNoWaitGet (tMbox * mbox, void **msg);
uint32_t tMboxNotify (tMbox * mbox, void * msg, uint32_t notifyOption);
void tMboxFlush (tMbox * mbox);
uint32_t tMboxDestroy (tMbox * mbox);
void tMboxGetInfo (tMbox * mbox, tMboxInfo * info);

#endif
