/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-26
 * @brief       IO扩展实验
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
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "led.h"
#include "iic.h"
#include "xl9555.h"


i2c_obj_t i2c0_master;

/* Hardware path: ESP32-S3 GPIO41/42 -> I2C0 -> XL9555 (0x20).
 * XL9555 provides the four keys and buzzer output; GPIO1 drives the LED.
 * The application polls the keys and uses xl9555_key_scan() for debouncing. */

/**
 * @brief       显示实验信息
 * @param       无
 * @retval      无
 */
void show_mesg(void)
{
    /* 串口输出实验信息 */
    printf("\n");
    printf("********************************\n");
    printf("ESP32-S3\n");
    printf("EXIO TEST\n");
    printf("ATOM@ALIENTEK\n");
    printf("KEY0:Beep On, KEY1:Beep Off\n");
    printf("KEY2:LED On, KEY3:LED Off\n");
    printf("********************************\n");
    printf("\n");
}

/**
 * @brief       程序入口
 * @param       无
 * @retval      无
 */
void app_main(void)
{
    uint8_t key;
    esp_err_t ret;
    
    ret = nvs_flash_init();             /* 初始化NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    led_init();                         /* 初始化LED */
    i2c0_master = iic_init(I2C_NUM_0);  /* 初始化IIC0 */
    xl9555_init(i2c0_master);           /* 初始化XL9555 */
    show_mesg();                        /* 显示实验信息 */

    while(1)
    {
        /* mode == 0 reports one event per press/release cycle. */
        key = xl9555_key_scan(0);
        
        switch (key)
        {
            case KEY0_PRES:
            {
                printf("KEY0 has been pressed \n");
                /* The board buzzer is active-low: low level enables it. */
                xl9555_pin_write(BEEP_IO, 0);
                break;
            }
            case KEY1_PRES:
            {
                printf("KEY1 has been pressed \n");
                xl9555_pin_write(BEEP_IO, 1);
                break;
            }
            case KEY2_PRES:
            {
                printf("KEY2 has been pressed \n");
                /* The board LED is active-low, so low level turns it on. */
                LED(0);
                break;
            }
            case KEY3_PRES:
            {
                printf("KEY3 has been pressed \n");
                LED(1);
                break;
            }
            default:
            {
                break;
            }
        }

        /* INT is only monitored for debugging; no interrupt handler is used. */
        if (XL9555_INT == 0)
        {
            printf("123");
        }
        vTaskDelay(200);
    }
}
