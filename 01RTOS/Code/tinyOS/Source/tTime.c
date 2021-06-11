#include "tinyOS.h"

void tTaskDelay (uint32_t delay)
{
	uint32_t status = tTaskEnterCritical();  //临界区
	 
	tTimeTaskWait(currentTask, delay);		 //设置任务延时

	tTaskSchedUnRdy(currentTask);	   		//从任务就绪列表中移除
	
	tTaskExitCritical(status);				//退出临界区
	
	tTaskSched();
}
