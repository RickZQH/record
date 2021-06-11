#ifndef TCONFIG_H
#define TCONFIG_H

#define	TINYOS_PRO_COUNT	32				//优先级
#define	TINYOS_SLICE_MAX	10				//同任务优先级的时间片
#define	TINYOS_IDLETASK_STACK_SIZE	1024	//空闲任务栈大小

#define	TINYOS_TIMERTASK_STACK_SIZE	1024	//定时器任务栈大小
#define	TINYOS_TIMERTASK_PRIO		1	    //定时器模块任务优先级

#define	TINYOS_SYSTICK_MS			10      //Systick定时器中断频率

#endif
