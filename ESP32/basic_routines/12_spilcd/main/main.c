/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-26
 * @brief       SPILCD实验
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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "led.h"
#include "lcd.h"


i2c_obj_t i2c0_master;

/*
 * 本工程的硬件初始化顺序很重要：
 * 1. NVS 是 ESP-IDF 常用的系统存储区域，先处理它的初始化异常；
 * 2. 初始化板载 LED，便于通过闪烁观察程序是否仍在运行；
 * 3. 初始化 I2C0，XL9555 依赖它控制 LCD 的电源和复位；
 * 4. 初始化 SPI2，LCD 的像素数据和控制命令通过它发送；
 * 5. 初始化 XL9555 和 LCD，最后才进入显示循环。
 * 这里的 i2c0_master 是“设备控制块”的副本，传给 XL9555 后，
 * XL9555 驱动就知道应该使用哪个 I2C 控制器以及对应的 SDA/SCL 引脚。
 */

/**
 * @brief       程序入口
 * @param       无
 * @retval      无
 */
void app_main(void)
{
    uint8_t x = 0;                     /* 当前要显示的背景颜色编号，0~11循环 */
    esp_err_t ret;
    
    
    ret = nvs_flash_init();             /* ESP-IDF系统初始化：挂载NVS分区 */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        /* 分区空间不足或版本变化时，擦除旧NVS后重新初始化。 */
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    led_init();                         /* 初始化LED */
    i2c0_master = iic_init(I2C_NUM_0);  /* 初始化IIC0 */
    spi2_init();                        /* 初始化SPI2 */
    xl9555_init(i2c0_master);           /* IO扩展芯片初始化 */
    lcd_init();                         /* 初始化LCD */

    /* 主循环每次更换一种背景色，再重新绘制四行文字。 */
    while (1)
    {
        switch (x)
        {
            case 0:
            {
                lcd_clear(WHITE);
                break;
            }
            case 1:
            {
                lcd_clear(BLACK);
                break;
            }
            case 2:
            {
                lcd_clear(BLUE);
                break;
            }
            case 3:
            {
                lcd_clear(RED);
                break;
            }
            case 4:
            {
                lcd_clear(MAGENTA);
                break;
            }
            case 5:
            {
                lcd_clear(GREEN);
                break;
            }
            case 6:
            {
                lcd_clear(CYAN);
                break;
            }
            case 7:
            {
                lcd_clear(YELLOW);
                break;
            }
            case 8:
            {
                lcd_clear(BRRED);
                break;
            }
            case 9:
            {
                lcd_clear(GRAY);
                break;
            }
            case 10:
            {
                lcd_clear(LGRAY);
                break;
            }
            case 11:
            {
                lcd_clear(BROWN);
                break;
            }
        }

        /* x、y是左上角坐标；width/height是可绘制区域；size是字体高度。 */
        lcd_show_string(10, 40, 240, 32, 32, "ESP32", RED);
        lcd_show_string(10, 80, 240, 24, 24, "SPILCD TEST", RED);
        lcd_show_string(10, 110, 240, 16, 16, "ATOM@ALIENTEK", RED);
        lcd_show_string(10, 150, 240, 12, 12, "ATOM@ALIENTEK", RED);
        x++;

        if (x == 12)
        {
            x = 0;
        }

        LED_TOGGLE();                    /* 使用LED翻转表示一次显示刷新完成 */
        vTaskDelay(500);                  /* 参数是FreeRTOS tick，当前代码保持原值 */
    }
}
