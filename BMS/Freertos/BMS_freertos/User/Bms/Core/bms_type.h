#ifndef __BMS_TYPE_H__
#define __BMS_TYPE_H__

/**
 * @file bms_type.h
 * @brief BMS业务层共用的状态、采样对象和系统模式类型定义。
 *
 * CellIndex是电芯选择位图：第n位为1表示选择第n节电芯，
 * 可以使用按位或同时选择多节电芯。
 */


#include <stdbool.h>

#include "bms_config.h"





/** @brief BMS功能开关状态。 */
typedef enum
{
	BMS_STATE_ENABLE,  /**< 允许对应功能工作。 */
	BMS_STATE_DISABLE  /**< 禁止对应功能工作。 */
}BMS_StateTypedef;


/**
 * @brief 电芯选择位图。
 * @note 每个枚举值占用一个独立的位，BMS_CELL_ALL表示当前支持的全部电芯。
 */
typedef enum
{
	BMS_CELL_NULL		= 0x0000, /**< 不选择任何电芯。 */
	BMS_CELL_INDEX1 	= 0x0001, /**< 选择第1节电芯。 */
	BMS_CELL_INDEX2 	= 0x0002, /**< 选择第2节电芯。 */
	BMS_CELL_INDEX3 	= 0x0004,
	BMS_CELL_INDEX4 	= 0x0008,
	BMS_CELL_INDEX5 	= 0x0010,
	BMS_CELL_INDEX6 	= 0x0020,
	BMS_CELL_INDEX7 	= 0x0040,
	BMS_CELL_INDEX8 	= 0x0080,
	BMS_CELL_INDEX9 	= 0x0100,
	BMS_CELL_INDEX10	= 0x0200,	
	BMS_CELL_INDEX11 	= 0x0400,
	BMS_CELL_INDEX12 	= 0x0800,
	BMS_CELL_INDEX13 	= 0x1000,
	BMS_CELL_INDEX14 	= 0x2000,
	BMS_CELL_INDEX15	= 0x4000,
	BMS_CELL_ALL		= 0x3FFF,
}BMS_CellIndexTypedef; /* 电芯位图类型：bit0对应第1节电芯。 */



/** @brief BMS当前运行模式。 */
typedef enum
{
	BMS_MODE_NULL,
	BMS_MODE_CHARGE,	// 充电模式
	BMS_MODE_DISCHARGE,	// 放电模式
	BMS_MODE_STANDBY,	// 待机模式
	BMS_MODE_SLEEP,		// 休眠模式
}BMS_SysModeTypedef; /* 系统工作模式类型。 */


typedef struct
{
	BMS_SysModeTypedef SysMode;	// 当前系统运行模式。
	BMS_StateTypedef Charge;	// 充电状态。
	BMS_StateTypedef Discharge;	// 放电状态。
	BMS_StateTypedef Balance;	// 均衡功能状态。
	
	uint8_t Cell_Real_Number; /* 当前实际参与监测的电芯数量，单位：节。 */
	uint8_t Temp_Real_Number; /* 当前实际参与监测的温度通道数量，单位：路。 */
}BMS_GlobalParamTypedef;



#endif

