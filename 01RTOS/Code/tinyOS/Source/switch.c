#include "tinyOS.h"
#include "ARMCM3.h"

//NVIC寄存器地址定义
#define	NVIC_INT_CTRL			0xE000Ed04
#define	NVIC_PENDSVSET			0x10000000
#define	NVIC_SYSPRI2			0xE000ED22
#define	NVIC_PENDSV_PRI			0x000000FF	

//对于内存地址的操作
#define	MEM32(addr)				*(volatile unsigned long *)(addr)
#define	MEM8(addr)				*(volatile unsigned char *)(addr)

//开临界区保护
uint32_t tTaskEnterCritical (void)  
{
	uint32_t primask = __get_PRIMASK();  //获取PRIMASK中断屏蔽寄存器
	__disable_irq();					 //关闭中断
	return primask;
}

//关临界区保护
void tTaskExitCritical (uint32_t status)
{
	__set_PRIMASK(status);				//设置PRIMASK中断屏蔽寄存器
}

__asm void PendSV_Handler (void)
{
	IMPORT currentTask	
	IMPORT nextTask		//在汇编中导入当前任务和下个任务 结构体
	
	MRS R0, PSP
	CBZ R0, PendSVHander_nosave  //第一次调度则直接恢复currentTask的栈
	
	STMDB R0!, {R4-R11}		//将R4-R11入栈
	
	LDR R1, =currentTask	
	LDR R1, [R1]
	STR R0, [R1]			//更新任务栈的值
	
PendSVHander_nosave
	LDR R0, =currentTask  
	LDR R1, =nextTask
	LDR R2, [R1]
	STR R2, [R0]
	
	LDR R0, [R2]		  //当前任务指针指向下个任务
	LDMIA R0!, {R4-R11}   //恢复R4-R11的值
	
	MSR PSP, R0			  //更新PSP的值
	ORR LR, LR, #0x04	  
	BX LR				  //带状态的程序跳转
}

void tTaskRunFirst ()
{
	__set_PSP(0);

	//配置PendSV异常中断优先级
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

void tTaskSwitch ()
{
	//触发PendSV异常
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;  
}

