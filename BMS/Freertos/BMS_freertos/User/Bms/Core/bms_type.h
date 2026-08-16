#ifndef __BMS_TYPE_H__
#define __BMS_TYPE_H__

/** BMS 公共状态类型；CellIndex 是位图，可用按位或同时选择多节电芯?*/


#include <stdbool.h>

#include "bms_config.h"





/* BMS功能开关状态：用于统一表示启用和禁用。 */
typedef enum
{
	BMS_STATE_ENABLE,  /* 允许功能工作 */
	BMS_STATE_DISABLE  /* 绂佹鍔熻兘宸ヤ綔 */
}BMS_StateTypedef;


/* BMS功能开关状态：用于统一表示启用和禁用。 */
typedef enum
{
	BMS_CELL_NULL		= 0x0000,
	BMS_CELL_INDEX1 	= 0x0001,
	BMS_CELL_INDEX2 	= 0x0002,
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








/* BMS功能开关状态：用于统一表示启用和禁用。 */
typedef enum
{
	BMS_MODE_NULL,
	BMS_MODE_CHARGE,	// 充电模式
	BMS_MODE_DISCHARGE,	// 放电模式
	BMS_MODE_STANDBY,	// 寰呮満妯″紡
	BMS_MODE_SLEEP,		// 鐫＄湢妯″紡
}BMS_SysModeTypedef; /* 系统工作模式类型。 */


typedef struct
{
	BMS_SysModeTypedef SysMode;	// 当前系统处于什么模?
	BMS_StateTypedef Charge;	// 充电状态。
	BMS_StateTypedef Discharge;	// 放电状态。
	BMS_StateTypedef Balance;	// 鍧囪　鐘舵€?
	
	uint8_t Cell_Real_Number; /* ǰʵʲĵоλڡ */	// 电芯实时数量
	uint8_t Temp_Real_Number; /* ǰʵõ¶ͨλ· */	// 温度实时数量
}BMS_GlobalParamTypedef;



#endif

