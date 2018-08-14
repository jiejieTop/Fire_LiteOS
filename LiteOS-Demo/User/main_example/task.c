/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   这是一个[野火]-STM32F103霸道LiteOS任务管理实验！
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-霸道 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
 /* LiteOS 头文件 */
#include "los_sys.h"
#include "los_typedef.h"
#include "los_task.ph"
 /* 板级外设头文件 */
#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_key.h" 

/* 定义任务句柄 */
UINT32 LED_Task_Handle;
UINT32 Key_Task_Handle;

/* 函数声明 */
static UINT32 Creat_LED_Task(void);
static UINT32 Creat_Key_Task(void);
static void LED_Task(void);
static void Key_Task(void);
static void BSP_Init(void);


/***************************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   第一步：开发板硬件初始化 
			第二步：创建APP应用任务
			第三步：启动LiteOS，开始多任务调度，启动失败则输出错误信息
  **************************************************************************/
int main(void)
{	
	UINT32 uwRet = LOS_OK;  //定义一个任务创建的返回值，默认为创建成功
	/* 板载相关初始化 */
  BSP_Init();
	/* 发送一个字符串 */
	printf("这是一个[野火]-STM32F103霸道LiteOS任务管理实验！\n\n");
  printf("按下K1挂起任务，按下K2恢复任务\n");
	/* LiteOS 核心初始化 */
	uwRet = LOS_KernelInit();
  if (uwRet != LOS_OK)
   {
	printf("LiteOS 核心初始化失败！\n");
	return LOS_NOK;
   }
	/* 创建LED_Task任务创建失败 */
	uwRet = Creat_LED_Task();
  if (uwRet != LOS_OK)
   {
	printf("LED_Task任务创建失败！\n");
       return LOS_NOK;
   }
	/* 创建Key_Task任务创建失败 */
	uwRet = Creat_Key_Task();
  if (uwRet != LOS_OK)
   {
	printf("Key_Task任务创建失败！\n");
       return LOS_NOK;
   }
   /* 开启LiteOS任务调度 */
   LOS_Start();
}
/*********************************************************************************
  * @ 函数名  ： LED_Task
  * @ 功能说明： LED_Task任务实现
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************************/
static void LED_Task(void)
{
  /* 任务都是一个无限循环，不能返回 */
	while(1)
	{
		LED2_TOGGLE;      //LED2翻转
		printf("LED_Task任务正在运行！\n");
		LOS_TaskDelay(1000);			
	}
}
/*********************************************************************************
  * @ 函数名  ： Key_Task
  * @ 功能说明： Key_Task任务实现
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************************/
static void Key_Task(void)
{
	UINT32 uwRet = LOS_OK;				/* 定义一个创建任务的返回类型，初始化为创建成功的返回值 */
	/* 任务都是一个无限循环，不能返回 */
	while(1)
	{
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )      /* K1 被按下 */
		{
			printf("挂起LED1任务！\n");
			uwRet = LOS_TaskSuspend(LED_Task_Handle);/* 挂起LED1任务 */
			if(LOS_OK == uwRet)
			{
				printf("挂起LED1任务成功！\n");
			}
		}
		else if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON ) /* K2 被按下 */
		{
			printf("恢复LED1任务!\n");
			uwRet = LOS_TaskResume(LED_Task_Handle); /* 恢复LED1任务 */
			if(LOS_OK == uwRet)
			{
				printf("恢复LED1任务成功！\n");
			}
			
		}
		
		LOS_TaskDelay(20);                                            /* 20ms扫描一次 */
	}
}

/*********************************************************************************
  * @ 函数名  ： Creat_LED_Task
  * @ 功能说明： 创建LED_Task任务
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************************/
static UINT32 Creat_LED_Task()
{
    UINT32 uwRet = LOS_OK;				/* 定义一个创建任务的返回类型，初始化为创建成功的返回值 */
    TSK_INIT_PARAM_S task_init_param;

    task_init_param.usTaskPrio = 4;								/* 优先级，数值越小，优先级越高 */
    task_init_param.pcName = "LED_Task";						/* 任务名，字符串形式，方便调试 */
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)LED_Task;	/* 任务函数名 */
    task_init_param.uwStackSize = 0x1000;						/* 栈大小，单位为字，即4个字节 */

    uwRet = LOS_TaskCreate(&LED_Task_Handle, &task_init_param);/* 创建任务 */
    return uwRet;
}
/*********************************************************************************
  * @ 函数名  ： Creat_Key_Task
  * @ 功能说明： 创建Key_Task任务
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************************/
static UINT32 Creat_Key_Task()
{
    UINT32 uwRet = LOS_OK;				/* 定义一个创建任务的返回类型，初始化为创建成功的返回值 */
    TSK_INIT_PARAM_S task_init_param;

    task_init_param.usTaskPrio = 5;								/* 优先级，数值越小，优先级越高 */
    task_init_param.pcName = "Key_Task";						/* 任务名，字符串形式，方便调试 */
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Key_Task;	/* 任务函数名 */
    task_init_param.uwStackSize = 0x1000;						/* 栈大小，单位为字，即4个字节 */

    uwRet = LOS_TaskCreate(&Key_Task_Handle, &task_init_param);/* 创建任务 */

    return uwRet;
}
/*********************************************************************************
  * @ 函数名  ： BSP_Init
  * @ 功能说明： 板级外设初始化，所有板子上的初始化均可放在这个函数里面
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************************/
static void BSP_Init(void)
{
	/*
	 * STM32中断优先级分组为4，即4bit都用来表示抢占优先级，范围为：0~15
	 * 优先级分组只需要分组一次即可，以后如果有其他的任务需要用到中断，
	 * 都统一用这个优先级分组，千万不要再分组，切忌。
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED 初始化 */
	LED_GPIO_Config();
  
  /* 按键初始化 */
	Key_GPIO_Config();
  
	/* 串口初始化	*/
	USART_Config();
  
	

}


/*********************************************END OF FILE**********************/

