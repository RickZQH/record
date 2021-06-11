#ifndef TINYOS_H
#define TINYOS_H

#include <stdint.h>

#include "tLib.h"

#include "tConfig.h"

#include "tTask.h"

#include "tEvent.h"

#include "tSem.h"

#include "tMbox.h"

#include "tMemBlock.h"

#include "tFlagGroup.h"

#include "tMutex.h"

#include "tTimer.h"

#define	TICKS_PER_SEC			(1000 / TINYOS_SYSTICK_MS)

//错误标志
typedef enum _tError {
	tErrorNoError = 0,			//无错误
	tErrorTimeout,				//超时
	tErrorResourceUnavaliable,
	tErrorDel,
	tErrorResourceFull,			//资源满
	tErrorOwner,
}tError;

extern tTask * currentTask;
extern tTask * nextTask;

uint32_t tTaskEnterCritical (void);
void tTaskExitCritical (uint32_t status);
	
void tTaskRunFirst (void);
void tTaskSwitch (void);

void tTaskSchedInit (void);
void tTaskSchedDisable (void);
void tTaskSchedEnable (void);
void tTaskSchedRdy (tTask * task);
void tTaskSchedUnRdy (tTask * task);
void tTaskSchedRemove (tTask * task);
void tTaskSched (void);
void tTimeTaskWait (tTask * task, uint32_t ticks);
void tTimeTaskWakeUp (tTask * task);
void tTimeTaskRemove (tTask * task);
void tTaskSystemTickHandler (void);
void tTaskDelay (uint32_t delay);

void tSetSysTickPeriod (uint32_t ms);
void tInitApp (void);

float tCpuUsageGet (void);

#endif
