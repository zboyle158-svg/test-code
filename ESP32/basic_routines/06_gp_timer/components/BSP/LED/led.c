/**
 ****************************************************************************************************
 * @file        led.c
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

#include "led.h"


/**
 * @brief       初始化LED
 * @param       无
 * @retval      无
 */
void led_init(void)
{
    /* GPIO配置结构体用于集中描述引脚的中断、输入输出和上下拉属性。 */
    gpio_config_t gpio_init_struct = {0};

    /* LED只作为普通GPIO使用，因此关闭GPIO中断。 */
    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;         /* 失能引脚中断 */

    /* 配置为输入输出模式，既可以读取，也可以驱动LED。 */
    gpio_init_struct.mode = GPIO_MODE_INPUT_OUTPUT;         /* 输入输出模式 */

    /* 使能内部上拉，禁止内部下拉。 */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;       /* 使能上拉 */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;  /* 失能下拉 */

    /* 位掩码的第1位对应GPIO1，因此只配置LED_GPIO_PIN。 */
    gpio_init_struct.pin_bit_mask = 1ull << LED_GPIO_PIN;   /* 设置的引脚的位掩码 */

    /* 调用ESP-IDF官方GPIO API，将上述配置写入硬件。 */
    gpio_config(&gpio_init_struct);                         /* 配置GPIO */

    /* 开发板LED通常为低电平点亮，因此输出高电平将LED关闭。 */
    LED(1);                                                 /* 关闭LED */
}
