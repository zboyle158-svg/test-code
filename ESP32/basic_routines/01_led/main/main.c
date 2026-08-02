/**
 * @file    main.c
 * @brief   ESP32-S3 LED闪烁实验的程序入口
 *
 * 程序流程：初始化NVS -> 初始化LED引脚 -> 周期性翻转LED电平。
 */

/* FreeRTOS基础定义，例如TickType_t等类型。 */
#include "freertos/FreeRTOS.h"
/* FreeRTOS任务接口，本例使用其中的vTaskDelay()进行延时。 */
#include "freertos/task.h"
/* ESP-IDF的NVS（非易失性存储）接口。 */
#include "nvs_flash.h"
/* 本项目自己编写的LED驱动接口。 */
#include "led.h"

/**
 * @brief ESP-IDF应用程序入口
 *
 * ESP-IDF启动完成后会自动调用app_main()，它相当于普通C程序中的main()。
 * app_main()本身运行在一个FreeRTOS任务中，因此可以调用vTaskDelay()。
 */
void app_main(void)
{
    /* esp_err_t是ESP-IDF统一使用的错误码类型。 */
    esp_err_t ret;

    /*
     * 初始化NVS。
     * NVS用于把数据保存在Flash中，设备断电后数据仍可保留。
     * 本LED实验本身不依赖NVS，这部分通常是ESP-IDF工程的通用初始化代码。
     */
    ret = nvs_flash_init();

    /*
     * 以下两种情况表示NVS分区无法直接使用：
     * ESP_ERR_NVS_NO_FREE_PAGES：NVS中已经没有可用页面；
     * ESP_ERR_NVS_NEW_VERSION_FOUND：Flash中的NVS格式与当前程序不兼容。
     */
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        /* 擦除旧NVS。ESP_ERROR_CHECK会在操作失败时打印错误并终止程序。 */
        ESP_ERROR_CHECK(nvs_flash_erase());

        /* 擦除后重新初始化NVS。 */
        ret = nvs_flash_init();
    }

    /* 配置LED所连接的GPIO，并设置LED的初始状态。 */
    led_init();

    /* 嵌入式程序通常持续运行，因此这里使用无限循环。 */
    while (1)
    {
        /* 读取LED引脚当前电平，然后输出相反电平，实现亮灭切换。 */
        LED_TOGGLE();

        /*
         * 当前任务暂停500个FreeRTOS时钟节拍，让出CPU给其他任务。
         * 注意：参数单位是tick，不一定永远等于毫秒；本工程常见配置下1 tick为1 ms。
         */
        vTaskDelay(500);
    }
}
