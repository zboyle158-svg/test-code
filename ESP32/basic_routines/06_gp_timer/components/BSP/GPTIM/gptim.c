/**
 ****************************************************************************************************
 * @file        gtim.c
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

#include "gptim.h"


/*
 * 中断回调函数和app_main()通过这个队列通信：
 * 回调函数负责发送事件，app_main()负责接收和处理事件。
 */
QueueHandle_t queue;

/**
 * @brief       初始化通用定时器
 * @param       counts: 计数值
 * @param       resolution: 定时器周期，resolution = 1s = 1000000μs（此处，定时器以微秒作为计算单位，）
 * @retval      无
 */
void gptim_int_init(uint16_t counts, uint32_t resolution)
{
    /* 这些结构体由ESP-IDF GPTimer API使用，分别保存定时器、报警和回调配置。 */
    gptimer_config_t g_tim_handle;
    gptimer_alarm_config_t alarm_config;
    gptimer_event_callbacks_t g_tim_callbacks;

    /* 用于读取并打印设置后的当前计数值。 */
    uint64_t count;

    /* 配置通用定时器 */
    ESP_LOGI("GPTIMER_ALARM", "配置通用定时器");

    /* 定时器句柄是ESP-IDF内部定时器对象的引用。 */
    gptimer_handle_t g_tim = NULL;

    /* 使用默认时钟源，采用向上计数，计数频率由调用者传入。 */
    g_tim_handle.clk_src = GPTIMER_CLK_SRC_DEFAULT;                                     /* 选择定时器时钟源 */
    g_tim_handle.direction = GPTIMER_COUNT_UP;                                          /* 递增计数模式 */         
    g_tim_handle.resolution_hz = resolution;                                            /* 计数器分辨率 */

    /* 定时器报警时执行项目提供的回调函数。 */
    g_tim_callbacks.on_alarm = gptimer_callback;                                        /* 注册用户回调函数 */ 

    /* 第一次达到1000000个计数时触发报警，1MHz下约为1秒。 */
    alarm_config.alarm_count = 1000000;                                                 /* 报警目标计数值 */
    ESP_ERROR_CHECK(gptimer_new_timer(&g_tim_handle, &g_tim));                          /* 创建新的通用定时器，并返回句柄 */

    /* 创建长度为10的队列，每个队列元素都是一个定时器事件结构体。 */
    queue = xQueueCreate(10, sizeof(gptimer_event_t));                                  /* 创建一个队列，并引入一个事件 */

    if (!queue)
    {
        ESP_LOGE("GPTIMER_ALARM", "创建队列失败");                                       /* 创建队列失败 */

        return;
    }

    /* 设置和读取定时器当前计数值，用于演示GPTimer的计数器读写API。 */
    ESP_LOGI("GPTIMER_ALARM", "设置计数值");
    ESP_ERROR_CHECK(gptimer_set_raw_count(g_tim, counts));                              /* 设置计数值 */
    ESP_LOGI("GPTIMER_ALARM", "获取计数值");
    ESP_ERROR_CHECK(gptimer_get_raw_count(g_tim, &count));                              /* 获取计数值 */
    ESP_LOGI("GPTIMER_ALARM", "定时器计数值： %llu", count);

    /*
     * 注册报警回调函数，并将queue作为user_data传入。
     * 这样回调函数可以通过user_data取得用于发送事件的队列句柄。
     */
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(g_tim, &g_tim_callbacks, queue));  /* 配置通用定时器回调函数 */

    /* 按顺序使能定时器、配置报警动作，然后启动定时器。 */
    ESP_LOGI("GPTIMER_ALARM", "使能通用定时器");
    ESP_ERROR_CHECK(gptimer_enable(g_tim));                                             /* 使能通用定时器 */
    ESP_ERROR_CHECK(gptimer_set_alarm_action(g_tim, &alarm_config));                    /* 配置通用定时器报警事件 */
    ESP_ERROR_CHECK(gptimer_start(g_tim));                                              /* 启动通用定时器 */
}

/**
 * @brief       定时器回调函数
 * @param       无
 * @retval      无
 */
bool IRAM_ATTR gptimer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    /* 用于记录发送队列后是否唤醒了更高优先级任务。 */
    BaseType_t high_task_awoken = pdFALSE;

    /* 取回初始化时传入的FreeRTOS队列句柄。 */
    queue = (QueueHandle_t)user_data;

    /* 从ESP-IDF提供的报警事件数据中读取本次计数值。 */
    gptimer_event_t ele = {
        .event_count = edata->count_value
    };

    /*
     * 当前函数运行在ISR中，必须使用FromISR版本的队列API。
     * ISR只负责快速投递事件，具体打印等工作交给普通任务完成。
     */
    xQueueSendFromISR(queue, &ele, &high_task_awoken);
    
    /* 将报警值向后推迟1000000个计数，形成约1秒一次的周期报警。 */
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = edata->alarm_value + 1000000, /* 在接下来的1秒内报警 */ 
    };
    gptimer_set_alarm_action(timer, &alarm_config);
    
    /* 如果发送事件唤醒了高优先级任务，则请求ISR结束后立即切换任务。 */
    return high_task_awoken == pdTRUE;
}
