/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-26
 * @brief       通用定时器实验
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
#include "gptim.h"


/**
 * @brief       程序入口
 * @param       无
 * @retval      无
 */
void app_main(void)
{
    /*
     * record：示例程序中的记录变量。
     * 当前代码只对它赋值和递减，并不参与后续判断，因此不影响定时器功能。
     */
    uint8_t record;

    /* ESP-IDF统一使用esp_err_t表示API执行结果。 */
    esp_err_t ret;

    /* 用于保存GPTimer回调函数发送到FreeRTOS队列中的事件数据。 */
    gptimer_event_t g_tim_evente;
    
    /* 初始化NVS（Non-Volatile Storage，非易失性存储）。 */
    ret = nvs_flash_init();

    /*
     * 当NVS分区空间不足，或NVS数据版本发生变化时，
     * 先擦除旧NVS，再重新初始化。
     */
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    
    /* 初始化LED对应的GPIO。 */
    led_init();

    /*
     * 初始化GPTimer：
     * counts=100：定时器初始计数值；
     * resolution=1000000：计数频率为1MHz，即每个计数约为1us。
     */
    gptim_int_init(100, 1000000);
    
    while (1)
    {
        /* 每次循环开始时将示例记录变量设为1。 */
        record = 1;

        /*
         * 从GPTimer驱动创建的队列中等待定时器事件。
         * 2000是等待的FreeRTOS Tick数量，不一定直接等于2000ms。
         */
        if (xQueueReceive(queue, &g_tim_evente, 2000))
        {
            /*
             * 队列接收成功，说明定时器已经触发过一次报警。
             * event_count是本次报警发生时的定时器计数值。
             */
            ESP_LOGI("GPTIMER_ALARM", "定时器报警, 计数值： %llu", g_tim_evente.event_count);
            record--;
        }
        else
        {
            /* 在指定等待时间内没有收到事件，提示可能错过了一次事件。 */
            ESP_LOGW("GPTIMER_ALARM", "错过一次计数事件");
        }
    }

    /* while(1)不会结束，因此这句在当前程序中实际上不会执行。 */
    vQueueDelete(queue);
}
