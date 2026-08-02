/**
 * @file    led.h
 * @brief   LED驱动的公开声明
 *
 * 其他源文件只需包含本头文件，就可以使用LED引脚定义、控制宏和初始化函数。
 */

/* 头文件保护：防止同一个头文件被重复包含，造成重复定义。 */
#ifndef __LED_H_
#define __LED_H_

/* ESP-IDF的GPIO驱动，提供gpio_config()、gpio_set_level()等接口。 */
#include "driver/gpio.h"

/* 开发板上的LED连接到GPIO1。更换硬件引脚时主要修改这里。 */
#define LED_GPIO_PIN    GPIO_NUM_1

/** GPIO输出电平，枚举可以让0和1的含义更容易阅读。 */
enum GPIO_OUTPUT_STATE
{
    PIN_RESET, /* 枚举首项默认值为0，表示低电平。 */
    PIN_SET    /* 后一项自动递增为1，表示高电平。 */
};

/*
 * 设置LED引脚电平。
 * LED(x)中的x非0时输出高电平，x为0时输出低电平。
 * 本开发板LED为低电平点亮，所以LED(1)熄灭，LED(0)点亮。
 * do { ... } while (0)让多条语句组成的宏可以像普通函数一样安全使用。
 */
#define LED(x)          do { x ?                                      \
                             gpio_set_level(LED_GPIO_PIN, PIN_SET) :  \
                             gpio_set_level(LED_GPIO_PIN, PIN_RESET); \
                        } while (0)

/*
 * 翻转LED引脚的电平：高电平变低电平，低电平变高电平。
 * 逻辑非运算符“!”会把非0值变成0、把0变成1。
 * 由于这里需要读取输出引脚的当前电平，led_init()将GPIO配置为输入输出模式。
 */
#define LED_TOGGLE()    do {                                            \
                             gpio_set_level(LED_GPIO_PIN,               \
                                 !gpio_get_level(LED_GPIO_PIN));        \
                        } while (0)

/* 函数声明：具体实现位于led.c中。 */
void led_init(void);

#endif /* __LED_H_ */
