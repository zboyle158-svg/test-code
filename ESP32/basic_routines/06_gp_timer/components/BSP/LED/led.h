/**
 ****************************************************************************************************
 * @file        led.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-26
 * @brief       LED驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 ESP32-S3 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 * 
 ****************************************************************************************************
 */

#ifndef __LED_H_
#define __LED_H_

#include "driver/gpio.h"


/* 开发板板载LED连接的GPIO引脚。 */
#define LED_GPIO_PIN    GPIO_NUM_1  /* LED连接的GPIO端口 */

/* 用更直观的名称表示GPIO输出电平。 */
enum GPIO_OUTPUT_STATE
{
    PIN_RESET,
    PIN_SET
};

/*
 * 设置LED输出电平：
 * x为非0时输出高电平，x为0时输出低电平。
 * 该宏使用do...while(0)，可以安全地放在if语句等代码结构中。
 */
#define LED(x)          do { x ?                                      \
                             gpio_set_level(LED_GPIO_PIN, PIN_SET) :  \
                             gpio_set_level(LED_GPIO_PIN, PIN_RESET); \
                        } while(0)  /* LED翻转 */

/* 读取当前GPIO电平并输出相反电平，实现LED翻转。 */
#define LED_TOGGLE()    do { gpio_set_level(LED_GPIO_PIN, !gpio_get_level(LED_GPIO_PIN)); } while(0)  /* LED翻转 */

/* 初始化LED对应GPIO。 */
void led_init(void);    /* 初始化LED */

#endif
