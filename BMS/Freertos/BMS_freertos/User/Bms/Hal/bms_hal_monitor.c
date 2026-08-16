#include "bms_hal_monitor.h"

#include <string.h>

#include "bms_monitor.h"
#include "bms_global.h"
#include "bms_utils.h"

#include "bms_debug.h"

#include "drv_softi2c_bq769x0.h"





// 冒泡排序的比较程序,对电压数据进行比较
/* 比较函数：返回 1 表示前一个电芯电压更高，需要交换。 */
/* 比较两个电芯数据的电压。
 * 参数：e1/e2为BMS_CellDataTypedef元素地址。
 * 返回：前者电压更高返回1，否则返回0。
 * 注意：供BubbleSort调用，不修改输入数据。 */
static int compaer_cell(void *e1, void *e2)
{
	float temp1, temp2;
	
	temp1 = (*(BMS_CellDataTypedef *)e1).CellVoltage;
	temp2 = (*(BMS_CellDataTypedef *)e2).CellVoltage;

	if (temp1 > temp2)
	{
		return 1;
	}

    return 0;
}


/* 数据路径：BQ769x0 寄存器 -> 驱动层 SampleData -> 业务层 MonitorData。 */
/* 读取并整理所有单体电压。
 * 参数：无。返回：无。
 * 副作用：更新BMS_MonitorData，并对CellData副本按电压排序。
 * 注意：函数运行在监控任务上下文，底层访问BQ769x0软件I2C。 */
void Bms_HalMonitorCellVoltage(void)
{	
	BQ769X0_UpdateCellVolt();
	for (uint8_t index = 0; index < BMS_GlobalParam.Cell_Real_Number; index++)
	{
		BMS_MonitorData.CellVoltage[index] = BQ769X0_SampleData.CellVoltage[index];
		BMS_MonitorData.CellData[index].CellVoltage = BQ769X0_SampleData.CellVoltage[index];
		BMS_MonitorData.CellData[index].CellNumber = index;
	}

	// 进行冒泡排序
	BubbleSort(BMS_MonitorData.CellData, BMS_GlobalParam.Cell_Real_Number, sizeof(BMS_CellDataTypedef), compaer_cell);
}


/* 读取BQ769x0电池总电压并写入监控数据快照。参数和返回值：无。
 * 注意：BatteryVoltage单位为V。 */
void Bms_HalMonitorBatteryVoltage(void)
{
	BQ769X0_UpadteBatVolt();
	BMS_MonitorData.BatteryVoltage = BQ769X0_SampleData.BatteryVoltage;
}


/* 读取电池电流并更新监控数据。参数和返回值：无。
 * 注意：BatteryCurrent单位为A，正负方向由BQ769x0采样约定决定。 */
void Bms_HalMonitorBatteryCurrent(void)
{
	BQ769X0_UpdateCurrent();
	BMS_MonitorData.BatteryCurrent = BQ769X0_SampleData.BatteryCurrent;	
}


/* 读取温度通道，过滤超出有效范围的样本并排序。参数和返回值：无。
 * 副作用：更新CellTemp和CellTempEffectiveNumber；温度单位为摄氏度。 */
void Bms_HalMonitorCellTemperature(void)
{	
	uint8_t index1 = 0, index2 = 0;
	
	BQ769X0_UpdateTsTemp();	
	for (; index1 < BMS_GlobalParam.Temp_Real_Number; index1++)
	{
		if (BQ769X0_SampleData.TsxTemperature[index1] >= BMS_TEMP_MEASURE_MIN &&  BQ769X0_SampleData.TsxTemperature[index1] <= BMS_TEMP_MEASURE_MAX)
		{
			BMS_MonitorData.CellTemp[index2++] = BQ769X0_SampleData.TsxTemperature[index1];
		}
	}
	
	BMS_MonitorData.CellTempEffectiveNumber = index2;

	// 进行顺序排序
	BubbleFloat(BMS_MonitorData.CellTemp, index2);
}


/* 查询BQ769x0的负载检测状态。参数：无。返回：true表示检测到负载，否则false。
 * 注意：检测受BQ769x0充电开关和引脚电压条件限制。 */
bool Bms_HalMonitorLoadDetect(void)
{
	// BQ芯片只有在未开启充电的情况下并且CHG引脚电压大于0.7V才能够检测到负载
	return BQ769X0_LoadDetect();
}


