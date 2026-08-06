/**
 * @file        main.c
 * @brief       硬件PWM占空比渐变示例的应用入口
 *
 * 本文件负责初始化NVS和项目PWM驱动，然后让LEDC硬件重复执行渐变请求。
 */

#include "nvs_flash.h"
#include "pwm.h"


/**
 * @brief       ESP-IDF应用入口，由系统创建的主任务调用
 */
void app_main(void)
{
    esp_err_t ret;  /* ESP-IDF错误码类型，用于保存NVS初始化结果。 */

    /* NVS是非易失性键值存储；许多ESP-IDF组件会依赖其可用状态。 */
    ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        /* 分区已无空页或格式版本不兼容时，擦除后重新初始化。 */
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    /* 项目函数：13位分辨率、5kHzPWM；它会将LEDC通道0输出到GPIO1。 */
    pwm_init(13, 5000);

    while(1) 
    {
        /* FreeRTOSAPI：延时10个系统节拍，让出CPU；不是固定的10ms。 */
        vTaskDelay(10);
        /* 项目函数：请求硬件先渐变到目标占空比，再渐变回0。 */
        pwm_set_duty(LEDC_PWM_DUTY);
    }
}
