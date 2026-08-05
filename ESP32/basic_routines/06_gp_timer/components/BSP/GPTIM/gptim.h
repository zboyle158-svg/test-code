/**
 ****************************************************************************************************
 * @file        gtim.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-26
 * @brief       通用定时器驱动代码
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

#ifndef __GPTIM_H_
#define __GPTIM_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include "esp_log.h"


/*
 * 定时器事件数据：
 * 回调函数在中断环境中取得计数值后，将它封装成该结构体，
 * 再通过FreeRTOS队列传递给普通任务。
 */
typedef struct {
    uint64_t event_count;
} gptimer_event_t;

/* GPTimer事件队列句柄，由gptim.c创建，由main.c接收事件。 */
extern QueueHandle_t queue;

/* 初始化通用定时器，counts为初始计数值，resolution为计数频率。 */
void gptim_int_init(uint16_t counts, uint32_t resolution);                                                          /* 初始化通用定时器 */

/* GPTimer达到报警值时由ESP-IDF调用的中断回调函数。 */
bool IRAM_ATTR gptimer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data);  /* 定时器回调函数 */

#endif
