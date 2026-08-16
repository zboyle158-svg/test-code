/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "freertos.h"    /* FreeRTOS核心类型和配置。 */
#include "task.h"        /* FreeRTOS任务接口。 */
#include "cmsis_os2.h"   /* CMSIS-RTOS2统一接口。 */
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void); /* HAL或系统初始化失败时进入的错误处理函数。 */

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RS485_EN_Pin GPIO_PIN_4             /* RS485收发器方向控制引脚。 */
#define RS485_EN_GPIO_Port GPIOA            /* RS485_EN所在GPIO端口。 */
#define RS485_PWREN_Pin GPIO_PIN_5          /* RS485电源使能引脚。 */
#define RS485_PWREN_GPIO_Port GPIOA         /* RS485_PWREN所在GPIO端口。 */
#define CAN_PWREN_Pin GPIO_PIN_10           /* CAN收发器电源使能引脚。 */
#define CAN_PWREN_GPIO_Port GPIOB           /* CAN_PWREN所在GPIO端口。 */
#define I2C1_SDA_Pin GPIO_PIN_13            /* 软件I2C数据线SDA。 */
#define I2C1_SDA_GPIO_Port GPIOB            /* SDA所在GPIO端口。 */
#define I2C1_SCL_Pin GPIO_PIN_14            /* 软件I2C时钟线SCL。 */
#define I2C1_SCL_GPIO_Port GPIOB            /* SCL所在GPIO端口。 */
#define BQ769X0_TS1_Pin GPIO_PIN_15         /* BQ769x0 TS1唤醒/温度通道引脚。 */
#define BQ769X0_TS1_GPIO_Port GPIOA         /* TS1所在GPIO端口。 */
#define LED_Pin GPIO_PIN_5                  /* 系统心跳LED引脚。 */
#define LED_GPIO_Port GPIOB                 /* 系统心跳LED所在GPIO端口。 */
#define LED1_Pin GPIO_PIN_6                 /* 电池状态指示灯1引脚。 */
#define LED1_GPIO_Port GPIOB                /* LED1所在GPIO端口。 */
#define LED2_Pin GPIO_PIN_7                 /* 电池状态指示灯2引脚。 */
#define LED2_GPIO_Port GPIOB                /* LED2所在GPIO端口。 */
#define LED3_Pin GPIO_PIN_8                 /* 电池状态指示灯3引脚。 */
#define LED3_GPIO_Port GPIOB                /* LED3所在GPIO端口。 */
#define LED4_Pin GPIO_PIN_9                 /* 电池状态指示灯4引脚。 */
#define LED4_GPIO_Port GPIOB                /* LED4所在GPIO端口。 */
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
