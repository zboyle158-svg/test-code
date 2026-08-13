/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   FreeRTOS v9.0.0 + STM32 静态创建单任务
  *********************************************************************
  * @attention
  *
  * 实验平台:野火 STM32全系列开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             包含的头文件
*************************************************************************
*/ 
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
/* 开发板硬件bsp头文件 */
#include "bsp_led.h"
#include "bsp_debug_usart.h"

/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
 /* 创建任务句柄 */
static TaskHandle_t AppTaskCreate_Handle;
/* LED任务句柄 */
static TaskHandle_t LED_Task_Handle;		

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


/******************************* 全局变量声明 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */
/* AppTaskCreate任务任务堆栈 */
/*
 * 静态任务必须由应用程序提前提供任务栈空间。
 * 数组元素类型是 StackType_t，而不是字节数组；因此 128 表示 128 个栈单元，
 * 实际占用字节数由当前移植层对 StackType_t 的定义决定。
 */
/* 静态任务栈：数组元素是 StackType_t，128 表示 128 个栈单元，实际字节数由移植层决定。 */
static StackType_t AppTaskCreate_Stack[128];
/* LED任务堆栈 */
static StackType_t LED_Task_Stack[128];

/* AppTaskCreate 任务控制块 */
/*
 * StaticTask_t 是任务控制块的静态存储区。
 * FreeRTOS 会把任务状态、优先级、栈指针、链表节点等运行时信息放入其中。
 * 任务栈和任务控制块必须同时有效，不能只提供其中一项。
 */
/* 静态任务控制块：保存任务状态、优先级、栈指针及调度链表节点等信息。 */
static StaticTask_t AppTaskCreate_TCB;
/* AppTaskCreate 任务控制块 */
static StaticTask_t LED_Task_TCB;

/* 空闲任务任务堆栈 */
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];
/* 定时器任务堆栈 */
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];

/* 空闲任务控制块 */
static StaticTask_t Idle_Task_TCB;	
/* 定时器任务控制块 */
static StaticTask_t Timer_Task_TCB;
 
/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void AppTaskCreate(void);/* 用于创建任务 */

static void LED_Task(void* pvParameters);/* LED_Task任务实现 */

static void BSP_Init(void);/* 用于初始化板载相关资源 */

/**
	* 使用了静态分配内存，以下这两个函数是由用户实现，函数在task.c文件中有引用
	*	当且仅当 configSUPPORT_STATIC_ALLOCATION 这个宏定义为 1 的时候才有效
	*/
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, 
																		StackType_t **ppxTimerTaskStackBuffer, 
																		uint32_t *pulTimerTaskStackSize);

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
																	 StackType_t **ppxIdleTaskStackBuffer, 
																	 uint32_t *pulIdleTaskStackSize);

/*****************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   第一步：开发板硬件初始化 
            第二步：创建APP应用任务
            第三步：启动FreeRTOS，开始多任务调度
  ****************************************************************/
int main(void)
{	
  /* 开发板硬件初始化 */
  BSP_Init();
  printf("这是一个[野火]-STM32全系列开发板-FreeRTOS-静态创建单任务!\r\n");
   /*
   * 第一个任务是“任务创建任务”。它只负责在调度器运行后创建 LED_Task，
   * 创建完成即自删除，因此它不是长期运行的业务任务。
   *
   * xTaskCreateStatic() 不从 FreeRTOS heap 中申请 TCB 和栈，而是使用调用者
   * 传入的 AppTaskCreate_Stack 与 AppTaskCreate_TCB；返回 NULL 表示创建失败。
   */
  /*
   * 先创建一个临时的任务创建任务。它启动后创建 LED_Task，随后自删除。
   * xTaskCreateStatic 使用调用者提供的任务栈和 TCB，不从 FreeRTOS heap 取空间。
   */
  /* 创建 AppTaskCreate 任务 */
	AppTaskCreate_Handle = xTaskCreateStatic((TaskFunction_t	)AppTaskCreate,		//任务函数
															(const char* 	)"AppTaskCreate",		//任务名称
															(uint32_t 		)128,	//任务堆栈大小
															(void* 		  	)NULL,				//传递给任务函数的参数
															(UBaseType_t 	)3, 	//任务优先级
															(StackType_t*   )AppTaskCreate_Stack,	//任务堆栈
															(StaticTask_t*  )&AppTaskCreate_TCB);	//任务控制块   
															
	if(NULL != AppTaskCreate_Handle)/* 创建成功 */
    vTaskStartScheduler();   /* 启动任务，开启调度 */
  
  while(1);   /* 正常不会执行到这里 */    
}


/***********************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 为了方便管理，所有的任务创建函数都放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void AppTaskCreate(void)
{
  /*
   * 创建阶段暂时关闭可屏蔽中断/调度影响，避免 AppTaskCreate 尚未完成时
   * 被其他执行路径打断。临界区必须尽量短，且必须与 taskEXIT_CRITICAL() 配对。
   */
  /* 创建阶段暂时进入临界区，防止任务创建流程被调度打断；结束后必须配对退出。 */
  taskENTER_CRITICAL();           //进入临界区

  /* 创建LED_Task任务 */
	LED_Task_Handle = xTaskCreateStatic((TaskFunction_t	)LED_Task,		//任务函数
															(const char* 	)"LED_Task",		//任务名称
															(uint32_t 		)128,					//任务堆栈大小
															(void* 		  	)NULL,				//传递给任务函数的参数
															(UBaseType_t 	)4, 				//任务优先级
															(StackType_t*   )LED_Task_Stack,	//任务堆栈
															(StaticTask_t*  )&LED_Task_TCB);	//任务控制块   
	
	if(NULL != LED_Task_Handle)/* 创建成功 */
		printf("LED_Task任务创建成功!\n");
	else
		printf("LED_Task任务创建失败!\n");
	
  /*
   * LED_Task 创建完成后，AppTaskCreate 已完成使命，删除自身释放其任务资源。
   * 由于这里使用静态内存，删除不会归还 heap 中的栈块，但任务仍会从调度器
   * 的任务列表中移除；对应的静态数组在本示例中保持存在。
   */
  /* 创建完成后临时任务已无工作，删除自身并从调度器任务列表中移除。 */
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();            //退出临界区
}



/**********************************************************************
  * @ 函数名  ： LED_Task
  * @ 功能说明： LED_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void LED_Task(void* parameter)
{	
    while (1)
    {
        LED1_ON;
        /* 参数单位是 Tick，不是固定毫秒；本工程 1000 Tick/s，因此约为 500 ms。
         * 延时期间任务进入阻塞态，CPU 可运行其他就绪任务或空闲任务。 */
        vTaskDelay(500);   /* 延时500个tick */
        printf("LED_Task Running,LED1_ON\r\n");
        
        LED1_OFF;     
        vTaskDelay(500);   /* 延时500个tick */		 		
        printf("LED_Task Running,LED1_OFF\r\n");
    }
}

/***********************************************************************
  * @ 函数名  ： BSP_Init
  * @ 功能说明： 板级外设初始化，所有板子上的初始化均可放在这个函数里面
  * @ 参数    ：   
  * @ 返回值  ： 无
  *********************************************************************/
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

	/* 串口初始化	*/
	Debug_USART_Config();
  
}
 

/**
  **********************************************************************
  * @brief  获取空闲任务的任务堆栈和任务控制块内存
	*					ppxTimerTaskTCBBuffer	:		任务控制块内存
	*					ppxTimerTaskStackBuffer	:	任务堆栈内存
	*					pulTimerTaskStackSize	:		任务堆栈大小
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  **********************************************************************
  */ 
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
								   StackType_t **ppxIdleTaskStackBuffer, 
								   uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer=&Idle_Task_TCB;/* 任务控制块内存 */
	*ppxIdleTaskStackBuffer=Idle_Task_Stack;/* 任务堆栈内存 */
	*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;/* 任务堆栈大小 */
}

/**
  *********************************************************************
  * @brief  获取定时器任务的任务堆栈和任务控制块内存
	*					ppxTimerTaskTCBBuffer	:		任务控制块内存
	*					ppxTimerTaskStackBuffer	:	任务堆栈内存
	*					pulTimerTaskStackSize	:		任务堆栈大小
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  **********************************************************************
  */ 
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, 
									StackType_t **ppxTimerTaskStackBuffer, 
									uint32_t *pulTimerTaskStackSize)
{
	*ppxTimerTaskTCBBuffer=&Timer_Task_TCB;/* 任务控制块内存 */
	*ppxTimerTaskStackBuffer=Timer_Task_Stack;/* 任务堆栈内存 */
	*pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;/* 任务堆栈大小 */
}

/********************************END OF FILE****************************/
