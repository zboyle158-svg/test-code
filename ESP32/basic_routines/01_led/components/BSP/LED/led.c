/**
 * @file    led.c
 * @brief   LED驱动的具体实现
 */

/* 包含LED引脚、控制宏和led_init()声明。 */
#include "led.h"

/**
 * @brief 初始化LED所连接的GPIO
 *
 * GPIO配置完成后将引脚输出高电平，使低电平点亮的LED默认保持熄灭。
 */
void led_init(void)
{
    /*
     * 定义GPIO配置结构体，并把所有成员先清零。
     * 这样即使结构体以后增加成员，未单独赋值的成员也有确定的初始值。
     */
    gpio_config_t gpio_init_struct = {0};

    /* LED不需要通过引脚中断响应外部信号，因此关闭GPIO中断。 */
    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;

    /*
     * 配置为输入输出模式：
     * 输出功能用于控制LED，输入功能用于gpio_get_level()读取当前电平。
     */
    gpio_init_struct.mode = GPIO_MODE_INPUT_OUTPUT;

    /* 开启内部上拉电阻，让引脚在某些状态下倾向于保持高电平。 */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;

    /* 关闭内部下拉电阻，避免同时启用上下拉。 */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;

    /*
     * pin_bit_mask是位掩码，每一位代表一个GPIO。
     * 1ULL表示64位无符号整数1，左移LED_GPIO_PIN位后选中GPIO1。
     * 当LED_GPIO_PIN为1时，结果为二进制...0010，即第1位为1。
     */
    gpio_init_struct.pin_bit_mask = 1ULL << LED_GPIO_PIN;

    /* 将上面的所有配置一次性应用到选中的GPIO。 */
    gpio_config(&gpio_init_struct);

    /* 本板LED低电平点亮；输出高电平，使LED初始化后保持熄灭。 */
    LED(1);
}
