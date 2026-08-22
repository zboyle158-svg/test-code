#ifndef __BMS_ENERGY_H__
#define __BMS_ENERGY_H__


#include <rtthread.h>
#include "cmsis_os2.h"
#include "bms_type.h"


typedef struct
{
	float SocStopChg;			// 停止充电的SOC阈值。
	float SocStartChg;			// 允许重新充电的SOC阈值。
	float SocStopDsg;			// 停止放电的SOC阈值。
	float SocStartDsg;			// 允许重新放电的SOC阈值。
	
	float BalanceStartVoltage;	// 启动均衡的电芯电压，单位为V。
	float BalanceDiffeVoltage;	// 允许均衡的最小压差，单位为V。
	uint32_t BalanceCycleTime;	// 均衡周期，单位为秒。
	BMS_CellIndexTypedef BalanceRecord;	// 当前已经参与均衡的电芯位图。
	
}BMS_EnergyDataTypedef;


extern osSemaphoreId_t BalanceSem;
extern BMS_EnergyDataTypedef BMS_EnergyData;


void BMS_EnergyInit(void);



#endif

