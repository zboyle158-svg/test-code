/**
 * @file        pwm.h
 * @brief       PWM驱动接口与硬件参数
 */

#ifndef __PWM_H_
#define __PWM_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/ledc.h"
#include "driver/gpio.h"


/* 本项目将LEDC定时器0、通道0和开发板LED所在GPIO1组合为一条PWM输出链路。 */
#define LEDC_PWM_TIMER          LEDC_TIMER_0        /* 使用LEDC定时器0产生PWM计数周期。 */
#define LEDC_PWM_MODE           LEDC_LOW_SPEED_MODE /* ESP32-S3使用该LEDC速度模式。 */
#define LEDC_PWM_CH0_GPIO       GPIO_NUM_1          /* 通道0的PWM波形输出到GPIO1，连接板载LED。 */
#define LEDC_PWM_CH0_CHANNEL    LEDC_CHANNEL_0      /* 选择LEDC的通道0。 */
#define LEDC_PWM_DUTY           8000                /* 13位分辨率下的目标计数值，接近满量程8191。 */
#define LEDC_PWM_FADE_TIME      3000                /* 每一段渐变允许使用的最长时间，单位ms。 */

/* 项目函数声明，不是ESP-IDF内置API。 */
void pwm_init(uint8_t resolution, uint16_t freq);   /* 配置PWM硬件资源。 */
void pwm_set_duty(uint16_t duty);                   /* 发起两段硬件渐变。 */

#endif
