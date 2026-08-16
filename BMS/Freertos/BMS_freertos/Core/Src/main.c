/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"  /* CMSIS-RTOS鎺ュ彛锛岀敤浜庡垵濮嬪寲鍜屽惎鍔‵reeRTOS銆?*/
#include "can.h"       /* CAN外设句柄和初始化函数声明?*/
#include "usart.h"     /* UART外设句柄和初始化函数声明?*/
#include "gpio.h"      /* GPIO初始化函数声明?*/

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <rtthread.h>  /* 保留的RT-Thread兼容接口；当前主调度器为FreeRTOS?*/

#include "board.h"     /* 鏉跨骇鎺ュ彛澹版槑銆?*/

#include "bms_app.h"   /* BMS鎬诲垵濮嬪寲鎺ュ彛澹版槑銆?*/

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void); /* 创建FreeRTOS对象和默认启动任务?*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /*
   * 系统启动总入口。执行到osKernelStart()后，CPU控制权交给FreeRTOS
   * 调度器，BMS功能由各个RTOS任务并发运行?
   */
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init(); /* 初始化HAL、复位外设并准备系统Tick?*/

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config(); /* 配置HSE、PLL及HCLK/PCLK时钟树?*/

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();         /* 初始化LED、I2C、BQ769x0和控制引脚?*/
  MX_USART1_UART_Init();  /* 初始化调试或通信串口1?*/
  MX_USART2_UART_Init();  /* 初始化通信串口2?*/
  MX_CAN_Init();          /* 初始化CAN外设?*/
  /* USER CODE BEGIN 2 */

  //Board_Initialize();

 // BMS_SysInitialize();

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* 初始化RTOS内核，此时调度器尚未运行?*/
  MX_FREERTOS_Init();     /* 创建默认任务及其他RTOS对象?*/

  /* Start scheduler */
  osKernelStart();        /* 启动调度器，之后由FreeRTOS选择任务运行?*/

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	//rt_kprintf("Hello RT-Thread!\r\n");
//	rt_thread_mdelay(500);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  /* RCC振荡器配置结构体：保存HSE、HSI和PLL参数?*/
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  /* RCC总线时钟配置结构体：保存SYSCLK、AHB和APB分频参数?*/
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9; /* HSE经PLL进行9倍频?*/
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; /* 系统时钟使用PLL输出。 */
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* htim指向产生周期更新中断的定时器句柄?*/
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4) {
    HAL_IncTick(); /* TIM4作为HAL时间基准，递增uwTick。 */
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* 初始化失败后关闭中断并停在此处，便于调试器定位故障?*/
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* file和line分别记录断言失败的源文件和代码行号?*/
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
