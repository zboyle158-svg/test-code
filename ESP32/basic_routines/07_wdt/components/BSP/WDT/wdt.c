#include "wdt.h"

/* 保存 ESP-IDF 定时器句柄，重启定时器时需要使用该句柄。 */
esp_timer_handle_t esp_tim_handle;

/*
 * 创建并启动软件看门狗定时器。
 * arr 当前没有使用；tps 是定时器周期，单位为微秒。
 */
void wdt_init(uint16_t arr, uint64_t tps)
{
    /* 配置 ESP-IDF esp_timer 的回调函数和用户参数。 */
    esp_timer_create_args_t tim_periodic_arg = {
        /* 定时器到期后，ESP-IDF 会调用这个回调函数。 */
        .callback = &wdt_isr_handler,

        /* 本实验不需要向回调函数传递额外参数。 */
        .arg = NULL,
    };

    /* 创建定时器，并把句柄保存到全局变量中。 */
    esp_timer_create(&tim_periodic_arg, &esp_tim_handle);

    /* 启动周期定时器；main.c 传入 1000000，即周期为 1 秒。 */
    esp_timer_start_periodic(esp_tim_handle, tps);
}

/*
 * 重新开始当前定时器，用于模拟“喂狗”。
 * 只要程序能执行到这里，就会把下一次超时的时间向后推迟 timeout 微秒。
 */
void restart_timer(uint64_t timeout)
{
    esp_timer_restart(esp_tim_handle, timeout);
}

/*
 * 看门狗超时回调。
 * 如果主程序没有及时调用 restart_timer()，就执行 ESP-IDF 的系统重启 API。
 */
void IRAM_ATTR wdt_isr_handler(void *arg)
{
    esp_restart();
}
