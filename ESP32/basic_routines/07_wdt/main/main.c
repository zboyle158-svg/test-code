#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "led.h"
#include "key.h"
#include "wdt.h"

/*
 * 程序入口。
 * ESP-IDF 完成底层启动后，会创建主任务并调用 app_main()。
 */
void app_main(void)
{
    esp_err_t rets;

    /* 初始化 NVS 非易失性存储。NVS 常用于保存设备配置和 Wi-Fi 参数。 */
    rets = nvs_flash_init();

    /*
     * NVS 分区空间不足或数据版本变化时，擦除旧 NVS 数据后重新初始化。
     * ESP_ERROR_CHECK() 是 ESP-IDF 宏，遇到错误时会输出错误并触发异常处理。
     */
    if (rets == ESP_ERR_NVS_NO_FREE_PAGES || rets == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        rets = nvs_flash_init();
    }

    /* 初始化板载 LED 对应的 GPIO。 */
    led_init();

    /* 初始化 GPIO0 上的 BOOT 按键。该按键为低电平有效。 */
    key_init();

    /*
     * 初始化软件看门狗。
     * tps 的单位是微秒，1000000 微秒等于 1 秒。
     * arr 在当前 wdt_init() 实现中暂未使用。
     */
    wdt_init(5000, 1000000);

    /* 本开发板 LED 为低电平点亮，因此输出 0 会点亮 LED。 */
    LED(0);

    while (1)
    {
        /*
         * 扫描 BOOT 按键。参数 0 表示单次触发模式：按住按键时不会连续返回按键值。
         */
        if (key_scan(0) == BOOT_PRES)
        {
            /*
             * 按键按下后重新开始 1 秒计时，这就是本实验模拟的“喂狗”。
             * 如果超时前没有执行此处，看门狗回调就会重启芯片。
             */
            restart_timer(1000000);
        }

        /* 让出 CPU，避免主循环无延时地占满处理器。 */
        vTaskDelay(10);
    }
}
