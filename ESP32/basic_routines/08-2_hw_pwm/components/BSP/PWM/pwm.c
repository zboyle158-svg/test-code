/**
 * @file        pwm.c
 * @brief       PWM驱动代码
 *
 * 这里的项目函数在ESP-IDF的LEDC驱动之上，配置PWM并调用其硬件渐变功能。
 */

#include "pwm.h"


/**
 * @brief       初始化一个LEDC定时器和一个LEDC输出通道
 * @param       resolution PWM占空比分辨率；例如13位对应计数范围0~8191
 * @param       freq PWM载波频率，单位Hz
 */
void pwm_init(uint8_t resolution, uint16_t freq)
{
    ledc_timer_config_t ledc_timer;                 /* ESP-IDF类型：描述LEDC定时器配置。 */
    ledc_channel_config_t ledc_channel;             /* ESP-IDF类型：描述LEDC通道与GPIO的连接。 */

    /* 先配置定时器：它决定PWM频率及每周期可使用的占空比计数范围。 */
    ledc_timer.duty_resolution = resolution;
    ledc_timer.freq_hz = freq;
    ledc_timer.speed_mode = LEDC_PWM_MODE;
    ledc_timer.timer_num = LEDC_PWM_TIMER;
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;             /* 由ESP-IDF为LEDC选择可用时钟源。 */
    ledc_timer_config(&ledc_timer);                 /* ESP-IDFAPI：把配置写入LEDC硬件。 */

    /* 再配置通道：将通道0绑定到GPIO1，并指定它使用上面的定时器0。 */
    ledc_channel.gpio_num = LEDC_PWM_CH0_GPIO;
    ledc_channel.speed_mode = LEDC_PWM_MODE;
    ledc_channel.channel = LEDC_PWM_CH0_CHANNEL;
    ledc_channel.intr_type = LEDC_INTR_DISABLE;
    ledc_channel.timer_sel = LEDC_PWM_TIMER;
    ledc_channel.duty = 0;                          /* 初始占空比为0。 */
    ledc_channel_config(&ledc_channel);             /* ESP-IDFAPI：完成GPIO与LEDC通道连接。 */

    /* ESP-IDFAPI：安装LEDC渐变服务；后续设置渐变前必须先执行此操作。 */
    ledc_fade_func_install(0);
}

/**
 * @brief       发起一次“增亮后熄灭”的硬件PWM渐变
 * @param       duty 目标占空比计数值，而不是百分数
 */
void pwm_set_duty(uint16_t duty)
{
    /* 配置从当前值到duty的渐变，时长由宏指定，单位为ms。 */
    ledc_set_fade_with_time(LEDC_PWM_MODE, LEDC_PWM_CH0_CHANNEL, duty, LEDC_PWM_FADE_TIME);
    /* 非阻塞启动：本次启动调用立即返回，实际占空比由LEDC硬件逐步改变。 */
    ledc_fade_start(LEDC_PWM_MODE, LEDC_PWM_CH0_CHANNEL, LEDC_FADE_NO_WAIT);

    /* 为避免覆写正在进行的第一段，本机IDF驱动会在此等待其完成，再配置回到0。 */
    ledc_set_fade_with_time(LEDC_PWM_MODE, LEDC_PWM_CH0_CHANNEL, 0, LEDC_PWM_FADE_TIME);
    ledc_fade_start(LEDC_PWM_MODE, LEDC_PWM_CH0_CHANNEL, LEDC_FADE_NO_WAIT);
}
