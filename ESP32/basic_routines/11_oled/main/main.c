/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-26
 * @brief       OLED实验
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

#include "driver/gpio.h"
#include "led.h"
#include "iic.h"
#include "xl9555.h"
#include "oled.h"


i2c_obj_t i2c0_master;
i2c_obj_t i2c1_master;

/*
 * 本例的硬件分工：
 *   I2C0 -> XL9555，负责扩展 IO，并为 OLED 提供复位控制信号；
 *   I2C1 -> OLED，负责发送 OLED 控制命令和显示数据。
 *
 * i2c_obj_t 不是 ESP-IDF 的类型，而是本工程 IIC 驱动封装的“总线控制块”。
 * iic_init() 返回它，传给 xl9555_init()/oled_init() 后，驱动就知道使用哪条总线。
 */

/**
 * @brief       程序入口
 * @param       无
 * @retval      无
 */
void app_main(void)
{
    /* t 保存当前要显示的 ASCII 字符，范围从空格 ' ' 到 '~'。 */
    uint8_t t = 0;

    /* 先初始化 GPIO1 LED，再初始化两条 I2C 总线和两个外设。 */
    led_init();                             /* 初始化LED */
    i2c0_master = iic_init(I2C_NUM_0);      /* 初始化IIC0 */
    i2c1_master = iic_init(I2C_NUM_1);      /* 初始化IIC1 */
    xl9555_init(i2c0_master);               /* 初始化XL9555 */
    oled_init(i2c1_master);                 /* 初始化OLED */

    /*
     * oled_show_string() 只修改 OLED_GRAM 显存缓冲区；
     * 必须调用 oled_refresh_gram()，这些内容才会通过 I2C 真正发送到屏幕。
     */
    oled_show_string(0, 0, "ALIENTEK", 24);
    oled_show_string(0, 24, "0.96' OLED TEST", 16);
    oled_show_string(0, 40, "ATOM 2023/8/26", 12);
    oled_show_string(0, 52, "ASCII:", 12);
    oled_show_string(64, 52, "CODE:", 12);
    oled_refresh_gram();                    /* 更新显示到OLED */
    
    t = ' ';
    while(1)
    {
        /* 每轮显示一个字符及其 ASCII 数值，然后整体刷新屏幕。 */
        oled_show_char(36, 52, t, 12, 1);   /* 显示ASCII字符 */
        oled_show_num(94, 52, t, 3, 12);    /* 显示ASCII字符的码值 */
        oled_refresh_gram();                /* 更新显示到OLED */
        t++;

        if (t > '~')
        {
            t = ' ';
        }

        /* 这里的 500 是 FreeRTOS tick，不是固定的 500 ms；具体时长由 tick 频率决定。 */
        vTaskDelay(500);
        LED_TOGGLE();                       /* LED闪烁 */
    }
}
