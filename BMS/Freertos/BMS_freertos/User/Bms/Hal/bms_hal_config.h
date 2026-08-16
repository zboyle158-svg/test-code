/**
 * @file bms_hal_config.h
 * @brief BMS硬件抽象层的BQ769x0保护参数配置接口。
 *
 * 本文件只声明配置类型和接口，不直接执行寄存器写操作。
 * 具体的寄存器地址、编码方式以及I2C访问过程由对应的.c文件实现。
 *
 * 注意：枚举值是BQ769x0寄存器规定的编码，不能随意调整顺序或数值。
 */

#ifndef __BMS_HAL_CONFIG_H__
#define __BMS_HAL_CONFIG_H__


#include <stdint.h>



/**
 * @brief 短路保护延时时间选择。
 *
 * BMS检测到短路条件后，不会立即判定为故障，而是等待所选延时时间，
 * 用于滤除瞬态干扰。该参数通常对应BQ769x0的SCD延时配置位。
 */
typedef enum
{
	BMS_SCD_DELAY_50us  = 0x00, /**< 短路检测延时50us。 */
	BMS_SCD_DELAY_100us = 0x01, /**< 短路检测延时100us。 */
	BMS_SCD_DELAY_200us = 0x02, /**< 短路检测延时200us。 */
	BMS_SCD_DELAY_400us = 0x03, /**< 短路检测延时400us。 */
}BMS_SCDDelayTypedef;

/**
 * @brief 放电过流保护延时时间选择。
 *
 * 当放电电流超过保护阈值时，芯片等待该延时时间后再触发OCD保护。
 * 延时时间越短，保护响应越快，但对瞬态电流尖峰越敏感。
 */
typedef enum
{
	BMS_OCD_DEALY_10ms	 = 0x00, /**< 放电过流延时10ms。名称中的DEALY为原有拼写，保持兼容。 */
	BMS_OCD_DELAY_20ms	 = 0x01, /**< 放电过流延时20ms。 */
	BMS_OCD_DELAY_40ms	 = 0x02, /**< 放电过流延时40ms。 */
	BMS_OCD_DELAY_80ms	 = 0x03, /**< 放电过流延时80ms。 */
	BMS_OCD_DELAY_160ms	 = 0x04, /**< 放电过流延时160ms。 */
	BMS_OCD_DELAY_320ms	 = 0x05, /**< 放电过流延时320ms。 */
	BMS_OCD_DELAY_640ms	 = 0x06, /**< 放电过流延时640ms。 */
	BMS_OCD_DELAY_1280ms = 0x07, /**< 放电过流延时1280ms。 */
}BMS_OCDDelayTypedef;

/** @brief 充电过压保护延时时间选择，单位为秒。 */
typedef enum
{
	BMS_OV_DELAY_1s	 = 0x00, /**< 过压持续1s后触发保护。 */
	BMS_OV_DELAY_2s	 = 0x01, /**< 过压持续2s后触发保护。 */
	BMS_OV_DELAY_4s	 = 0x02, /**< 过压持续4s后触发保护。 */
	BMS_OV_DELAY_8s  = 0x03, /**< 过压持续8s后触发保护。 */
}BMS_OVDelayTypedef;

/** @brief 放电欠压保护延时时间选择，单位为秒。 */
typedef enum
{
	BMS_UV_DELAY_1s	 = 0x00, /**< 欠压持续1s后触发保护。 */
	BMS_UV_DELAY_4s	 = 0x01, /**< 欠压持续4s后触发保护。 */
	BMS_UV_DELAY_8s	 = 0x02, /**< 欠压持续8s后触发保护。 */
	BMS_UV_DELAY_16s = 0x03, /**< 欠压持续16s后触发保护。 */
}BMS_UVDelayTypedef;

/**
 * @brief 设置短路保护延时。
 * @param SCDDelay 短路延时枚举值，必须使用BMS_SCDDelayTypedef中的成员。
 * @note 调用前应完成BQ769x0通信初始化；具体是否立即写入硬件由实现决定。
 */
void BMS_HalCfgSCDDelaySet(BMS_SCDDelayTypedef SCDDelay);

/**
 * @brief 设置放电过流保护延时。
 * @param OCDDelay 放电过流延时枚举值。
 * @note 延时过短可能对瞬态电流尖峰敏感，延时过长会降低保护响应速度。
 */
void BMS_HalCfgOCDDelaySet(BMS_OCDDelayTypedef OCDDelay);

/**
 * @brief 设置放电欠压保护延时。
 * @param UVDelay 欠压延时枚举值。
 * @note 该参数用于避免负载突变造成误触发。
 */
void BMS_HalCfgUVDelaySet(BMS_UVDelayTypedef UVDelay);

/**
 * @brief 设置充电过压保护延时。
 * @param OVDelay 过压延时枚举值。
 * @note 应结合电芯化学体系和充电器控制策略选择。
 */
void BMS_HalCfgOVDelaySet(BMS_OVDelayTypedef OVDelay);

/**
 * @brief 设置放电欠压保护阈值。
 * @param UVPThreshold 欠压保护阈值，具体单位和缩放方式由实现约定，通常为mV。
 * @note 必须确认数值处于BQ769x0允许的寄存器范围内。
 */
void BMS_HalCfgUVPThresholdSet(uint16_t UVPThreshold);

/**
 * @brief 设置充电过压保护阈值。
 * @param OVPThreshold 过压保护阈值，具体单位和缩放方式由实现约定，通常为mV。
 * @note 阈值应符合电芯化学体系，不能直接套用不同类型电池的参数。
 */
void BMS_HalCfgOVPThresholdSet(uint16_t OVPThreshold);



#endif

