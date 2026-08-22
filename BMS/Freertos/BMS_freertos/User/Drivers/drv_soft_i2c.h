#ifndef __DRV_SOFT_I2C_H__
#define __DRV_SOFT_I2C_H__

#include <stdio.h>
#include <rtthread.h>
#include "rthw.h"

#include "stm32f1xx_hal.h"


#define I2C_DEBUG_LEVEL 3

#if I2C_DEBUG_LEVEL == 0
#define I2C_INFO(fmt, arg...) 
#define I2C_WARNING(fmt, arg...)
#define I2C_ERROR(fmt, arg...)

#elif I2C_DEBUG_LEVEL == 1
#define I2C_INFO(fmt, arg...)   		print_usart1("<<-I2C-INFO->> "fmt"\r\n",##arg)
#define I2C_WARNING(fmt, arg...)
#define I2C_ERROR(fmt, arg...)

#elif I2C_DEBUG_LEVEL == 2
#define I2C_INFO(fmt, arg...)
#define I2C_WARNING(fmt, arg...)		print_usart1("<<-I2C-WARNING->> "fmt"\r\n",##arg)
#define I2C_ERROR(fmt, arg...)

#elif I2C_DEBUG_LEVEL == 3
#define I2C_INFO(fmt, arg...)
#define I2C_WARNING(fmt, arg...)
#define I2C_ERROR(fmt, arg...)	 		print_usart1("<<-I2C-ERROR->> "fmt"\r\n",##arg)
#endif


#define I2C_WR              0x00        /* 写操作标志。 */
#define I2C_RD              (1 << 0)       /* 读操作标志。 */
#define I2C_ADDR_10BIT      (1 << 1)       /* 使用10位I2C地址模式。 */
#define I2C_NO_START        (1 << 2)       /* 不发送START条件。 */
#define I2C_IGNORE_NACK     (1 << 3)       /* 忽略NACK。 */
#define I2C_NO_READ_ACK     (1 << 4)       /* 读取结束后不发送ACK。 */
#define I2C_NO_STOP         (1 << 5)       /* 不发送STOP条件。 */

#define I2C_CONTROL_BYTE    (1 << 6)       /* 使用控制字节cByte。 */
#define I2C_SAME_BYTE       (1 << 7)       /* 重复发送sByte。 */




/** I2C消息和总线配置结构。 */
struct I2C_MessageTypeDef
{
	uint8_t   *buf;          // 数据缓冲区地址。
	uint16_t  addr;         // 从设备地址。
	uint16_t  tLen;         // 待发送字节数。
	uint16_t  rLen;         // 待读取字节数。
	uint8_t   flags;       // 传输标志位组合。
	uint8_t   cByte;       // 控制字节。
	uint8_t   sByte;       // 重复发送字节。
};


/** I2C消息和总线配置结构。 */
struct I2C_BusTypeDef
{
	GPIO_TypeDef *gpiox; // GPIO端口。
	uint32_t gpio_rcc; // GPIO时钟配置。
	uint16_t sda_gpio_pin; // SDA引脚。
	uint16_t scl_gpio_pin; // SCL引脚。
	uint32_t retries; // 总线重试次数。
	void (*udelay)(uint32_t us); // 微秒延时回调。
	void (*lockInit)(void); // 锁初始化回调。
	void (*lock)(void); // 获取总线锁回调。
	void (*unlock)(void); // 释放总线锁回调。
};

extern struct I2C_BusTypeDef i2c1;

int I2C_BusInitialize(void);
uint32_t I2C_TransferMessages(struct I2C_BusTypeDef *bus, struct I2C_MessageTypeDef msgs[], uint32_t num);

#endif
