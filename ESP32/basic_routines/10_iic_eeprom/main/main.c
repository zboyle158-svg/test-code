/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-26
 * @brief       EEPORM实验
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
#include "led.h"
#include "iic.h"
#include "xl9555.h"
#include "24cxx.h"


/* 项目自己的I2C0描述对象：初始化后把端口、SCL/SDA引脚和结果传给两个I2C外设驱动。 */
i2c_obj_t i2c0_master;

/* const表示该字符串运行中不改写；数组自动保留结尾的'\0'，因此可按%s输出。 */
const uint8_t g_text_buf[] = {"ESP32-S3 EEPROM"};   /* 要写入到24c02的字符串数组 */
#define TEXT_SIZE   sizeof(g_text_buf)              /* 写入长度包含结束符，读回后才能安全作为字符串显示。 */

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
    printf("ESP32\n");
    printf("IIC EEPROM TEST\n");
    printf("ATOM@ALIENTEK\n");
    printf("KEY0:Write Data, KEY1:Read Data\n");
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
    uint16_t i = 0;
    uint8_t err = 0;
    uint8_t key;
    uint8_t datatemp[TEXT_SIZE];
    esp_err_t ret;
    
    /* ESP-IDF的NVS初始化；本例不直接读写NVS，但系统组件仍可能需要它。 */
    ret = nvs_flash_init();             /* 初始化NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    led_init();                         /* 初始化GPIO1上的低电平点亮LED */
    i2c0_master = iic_init(I2C_NUM_0);  /* 配置I2C0：SDA=GPIO41，SCL=GPIO42，速率400kHz */
    xl9555_init(i2c0_master);           /* 初始化XL9555：按键经该I2C IO扩展器读取 */
    at24cxx_init(i2c0_master);          /* 初始化AT24C02：与XL9555共享同一条I2C0总线 */
    show_mesg();                        /* 显示实验信息 */

    err = at24cxx_check();              /* 检测AT24C02 */
    
    if (err != 0)
    {
        while (1)                       /* 检测不到24c02 */
        {
            printf("24C02 check failed, please check!\n");
            vTaskDelay(500);
            LED_TOGGLE();               /* LED闪烁 */
        }
    }

    printf("24C02 Ready!\n");
    printf("\n");

    while(1)
    {
        /* 轮询XL9555上的按键。传入0表示一次按下只产生一次键值，须松开后才可再次触发。 */
        key = xl9555_key_scan(0);
        
        switch (key)
        {
            case KEY0_PRES:
            {
                /* 从EEPROM字节地址0开始逐字节写入字符串。强制转换仅适配驱动接口，不会修改const数据。 */
                at24cxx_write(0, (uint8_t *)g_text_buf, TEXT_SIZE);
                printf("The data written is:%s\n", g_text_buf);
                break;
            }
            case KEY1_PRES:
            {
                /* 从同一地址读回到RAM数组；写入时已保存\0，因此printf可按C字符串输出。 */
                at24cxx_read(0, datatemp, TEXT_SIZE);
                printf("The data read is:%s\n", datatemp);
                break;
            }
            default:
            {
                break;
            }
        }

        /* 主循环每轮延时10ms；计满20轮约为200ms，作为运行指示灯的翻转周期。 */
        i++;

        if (i == 20)
        {
            LED_TOGGLE();               /* LED闪烁 */
            i = 0;
        }
        
        vTaskDelay(10);
    }
}
