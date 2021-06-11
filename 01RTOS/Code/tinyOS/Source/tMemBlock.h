#ifndef TMEMBLOCK_H
#define	TMEMBLOCK_H

#include "tEvent.h"

//内存块
typedef struct _tMemBlock
{
	tEvent event;		  //事件
	void * memStart;	  //内存起始地址
	uint32_t blockSize;   //块大小
	uint32_t maxCount;	  //最大计数
	tList blockList;	  //块链表节点
}tMemBlock;

//块信息
typedef struct _tMemBlockInfo
{
	uint32_t count;
	uint32_t maxCount;
	uint32_t blockSize;
	uint32_t taskCount;
}tMemBlockInfo;

void tMemBlockInit (tMemBlock * memBlock, uint8_t * memStart, uint32_t blockSize, uint32_t blockCnt);
uint32_t tMemBlockWait (tMemBlock * memBlock, uint8_t **mem, uint32_t waitTicks);
uint32_t tMemBlockNoWaitGet (tMemBlock * memBlock, void ** mem);
void tMemBlockNotify (tMemBlock * memBlock, uint8_t * mem);
void tMemBlockGetInfo (tMemBlock * memBlock, tMemBlockInfo * info);
uint32_t tMemBlockDestroy (tMemBlock * memBlock);

#endif
