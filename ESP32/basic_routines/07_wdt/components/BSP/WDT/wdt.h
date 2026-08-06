#ifndef __WDT_H_
#define __WDT_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

/* 这些是实验参数示例，当前代码中没有直接使用它们。 */
#define TWDT_TIMEOUT_MS      3000
#define TASK_RESET_PERIOD_MS 2000
#define MAIN_DELAY_MS        10000

/* 创建并启动软件看门狗定时器，tps 单位为微秒。 */
void wdt_init(uint16_t arr, uint64_t tps);

/* 重新开始定时器，用于模拟喂狗，timeout 单位为微秒。 */
void restart_timer(uint64_t timeout);

/* 定时器超时后的回调函数。 */
void wdt_isr_handler(void *arg);  /* IRAM_ATTR仅写在wdt.c的定义处，避免声明与定义的段属性冲突 */

#endif
