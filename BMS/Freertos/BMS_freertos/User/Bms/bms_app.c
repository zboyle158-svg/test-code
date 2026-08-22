#include "bms_app.h" /* BMS应用层初始化接口。 */


#include "bms_hal_control.h"

#include "bms_monitor.h"
#include "bms_protect.h"
#include "bms_analysis.h"
#include "bms_energy.h"
#include "bms_info.h"
#include "bms_comm.h"
#include "bms_global.h"

#include "drv_soft_i2c.h"
#include "drv_softi2c_bq769x0.h"



/*
 * 作用：完成BQ769x0、软件I2C和BMS业务任务的总初始化。
 * 参数：无。返回值：无。
 * 注意：应在FreeRTOS调度器启动后的任务上下文中调用。
 */
void BMS_SysInitialize(void)
{
	#if 1
	
	/* 初始化参数：包含BQ769x0告警回调和保护阈值配置。 */

	BQ769X0_InitDataTypedef InitData;

	InitData.AlertOps.ocd 	 = BMS_ProtectHwOCD;
	InitData.AlertOps.scd 	 = BMS_ProtectHwSCD;
	InitData.AlertOps.ov	 = BMS_ProtectHwOV;
	InitData.AlertOps.uv 	 = BMS_ProtectHwUV;	
	
	InitData.AlertOps.cc 	 = BMS_MonitorHwCurrent;
	
	
	
	
	InitData.AlertOps.device = BMS_ProtectHwDevice;
	InitData.AlertOps.ovrd 	 = BMS_ProtectHwOvrd;

	InitData.ConfigData.SCDDelay	 = (BQ769X0_SCDDelayTypedef)INIT_SCD_DELAY;
	InitData.ConfigData.OCDDelay	 = (BQ769X0_OCDDelayTypedef)INIT_OCD_DELAY;
	InitData.ConfigData.UVDelay	 	 = (BQ769X0_OVDelayTypedef)INIT_UV_DELAY;
	InitData.ConfigData.OVDelay	 	 = (BQ769X0_UVDelayTypedef)INIT_OV_DELAY;
	InitData.ConfigData.UVPThreshold = INIT_UV_PROTECT * 1000;
	InitData.ConfigData.OVPThreshold = INIT_OV_PROTECT * 1000;
	
	/* 先建立软件I2C时序，再访问BQ769x0寄存器。 */
	I2C_BusInitialize();
	BQ769X0_Initialize(&InitData);
#endif
	
	BMS_MonitorInit(); 	/* 创建电池监控任务。 */	
	BMS_ProtectInit(); 	/* 创建保护任务和保护定时器。 */	
	BMS_AnalysisInit(); /* 创建电池分析和SOC任务。 */	
	BMS_EnergyInit(); 	/* 创建能量管理和均衡任务。 */	
	BMS_InfoInit(); 	/* 创建信息输出任务。 */		

}
