/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   这是一个[野火]-STM32F103霸道LiteOS二值信号量同步实验！
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
#include "los_sem.h"
/* 板级外设头文件 */
#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_key.h" 

/********************************** 任务句柄 *************************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */

/* 定义任务句柄 */
UINT32 Read_Task_Handle;
UINT32 Write_Task_Handle;

/********************************** 内核对象句柄 *********************************/
/*
 * 信号量，消息队列，事件标志组，软件定时器这些都属于内核的对象，要想使用这些内核
 * 对象，必须先创建，创建成功之后会返回一个相应的句柄。实际上就是一个指针，后续我
 * 们就可以通过这个句柄操作这些内核对象。
 *
 * 内核对象说白了就是一种全局的数据结构，通过这些数据结构我们可以实现任务间的通信，
 * 任务间的事件同步等各种功能。至于这些功能的实现我们是通过调用这些内核对象的函数
 * 来完成的
 * 
 */
 
/* 定义二值信号量的句柄 */
UINT32 BinarySem_Handle;


/******************************* 全局变量声明 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */

uint8_t ucValue [ 2 ] = { 0x00, 0x00 };

/* 函数声明 */
static UINT32 Creat_Read_Task(void);
static UINT32 Creat_Write_Task(void);

static void AppTaskCreate(void);

static void Read_Task(void);
static void Write_Task(void);
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
	/* 板级初始化，所有的跟开发板硬件相关的初始化都可以放在这个函数里面 */
  BSP_Init();
  
	/* 发送一个字符串 */
	printf("这是一个[野火]-STM32F103霸道LiteOS二值信号量同步实验！\n\n");
	/* LiteOS 核心初始化 */
	uwRet = LOS_KernelInit();
  if (uwRet != LOS_OK)
   {
	printf("LiteOS 核心初始化失败！\n");
	return LOS_NOK;
   }
 
	 /* 创建APP应用任务，所有的应用任务都可以放在这个函数里面 */
   AppTaskCreate();
    
   /* 开启LiteOS任务调度 */
   LOS_Start();
}


/*********************************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 任务创建，为了方便管理，所有的任务创建函数都可以放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  ********************************************************************************/
static void AppTaskCreate(void)
{
	UINT32 uwRet = LOS_OK;/* 定义一个创建任务的返回类型，初始化为创建成功的返回值 */
   
	/* 创建一个二值信号量*/
	uwRet = LOS_BinarySemCreate(1,&BinarySem_Handle);
	if (uwRet != LOS_OK)
    {
		printf("BinarySem_Handle二值信号量创建失败！\n");
    }
	
	/* 创建Read_Task任务创建失败 */
	uwRet = Creat_Read_Task();
    if (uwRet != LOS_OK)
    {
		printf("Read_Task任务创建失败！\n");
    }
	/* 创建est2_Task任务创建失败 */
	uwRet = Creat_Write_Task();
    if (uwRet != LOS_OK)
    {
		printf("Write_Task任务创建失败！\n");
    }
}

/*********************************************************************************
  * @ 函数名  ： Read_Task
  * @ 功能说明： Read_Task任务实现
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************************/
static void Read_Task(void)
{
	while(1)
	{
		LOS_SemPend( BinarySem_Handle , LOS_WAIT_FOREVER );  //获取二值信号量 xSemaphore,没获取到则一直等待
		
		if ( ucValue [ 0 ] == ucValue [ 1 ] )
		{ 			
			printf ( "\r\nSuccessful\r\n" );			
		}
		else
		{
			printf ( "\r\nFail\r\n" );			
		}
		
		LOS_SemPost( BinarySem_Handle );              	  //给出二值信号量 xSemaphore
		
		LOS_TaskDelay ( 1000 );  					      //每1s读一次		
  }
}
/*********************************************************************************
  * @ 函数名  ： Write_Task
  * @ 功能说明： Write_Task任务实现
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************************/
static void Write_Task(void)
{
		LOS_SemPend( BinarySem_Handle , LOS_WAIT_FOREVER );  //获取二值信号量 BinarySem_Handle，没获取到则一直等待		
		ucValue [ 0 ] ++;		
		LOS_TaskDelay ( 100 );        						 /* 延时100ms */		
		ucValue [ 1 ] ++;		
		LOS_SemPost( BinarySem_Handle );         				//给出二值信号量 xSemaphore		
		LOS_TaskYield();  									//放弃剩余时间片，进行一次任务切换	
}

/*********************************************************************************
  * @ 函数名  ： Creat_Read_Task
  * @ 功能说明： 创建Read_Task任务
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************************/
static UINT32 Creat_Read_Task()
{
    UINT32 uwRet = LOS_OK;				/* 定义一个创建任务的返回类型，初始化为创建成功的返回值 */
    TSK_INIT_PARAM_S task_init_param;

    task_init_param.usTaskPrio = 5;								/* 优先级，数值越小，优先级越高 */
    task_init_param.pcName = "Read_Task";						/* 任务名，字符串形式，方便调试 */
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Read_Task;	/* 任务函数名 */
    task_init_param.uwStackSize = 0x1000;						/* 栈大小，单位为字，即4个字节 */

    uwRet = LOS_TaskCreate(&Read_Task_Handle, &task_init_param);/* 创建任务 */
    return uwRet;
}
/*********************************************************************************
  * @ 函数名  ： Creat_Write_Task
  * @ 功能说明： 创建Write_Task任务
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************************/
static UINT32 Creat_Write_Task()
{
    UINT32 uwRet = LOS_OK;				/* 定义一个创建任务的返回类型，初始化为创建成功的返回值 */
    TSK_INIT_PARAM_S task_init_param;

    task_init_param.usTaskPrio = 4;								/* 优先级，数值越小，优先级越高 */
    task_init_param.pcName = "Write_Task";						/* 任务名，字符串形式，方便调试 */
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Write_Task;	/* 任务函数名 */
    task_init_param.uwStackSize = 0x1000;						/* 栈大小，单位为字，即4个字节 */

    uwRet = LOS_TaskCreate(&Write_Task_Handle, &task_init_param);/* 创建任务 */

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

