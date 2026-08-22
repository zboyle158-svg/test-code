/**
 * @file bms_analysis.c
 * @brief 电压统计、功率、容量修正和SOC估算。
 * 运行期间采用安时积分法，容量变化满足$\Delta Q=I\Delta t/3600$；上电或静置时可使用OCV估算。
 * 使用 OCV 查表校准，并对结果进行容量边界约束。
 */
#define BMS_DBG_TAG "Analysis"

#include <stdio.h>
#include <stdlib.h>
#include <rtthread.h>

#include "bms_analysis.h"

#include "bms_monitor.h"
#include "bms_protect.h"

#include "bms_utils.h"
#include "bms_global.h"
#include "bms_debug.h"





// thread config
#define ANALYSISI_TASK_STACK_SIZE	512
#define ANALYSISI_TASK_PRIORITY		11
#define ANALYSISI_TASK_TIMESLICE	25

#define ANALYSISI_TASK_PERIOD		1000



#define TEMP_CAP_RATE_LIMITH_HIGH   1050
#define TEMP_CAP_RATE_LIMITL_LOW    750







BMS_AnalysisDataTypedef BMS_AnalysisData =
{
	.CapacityRated = BMS_BATTERY_CAPACITY,
};




// 三元锂电池SOC开路电压法计算数据表。
// 支持磷酸铁锂、钛酸锂也得做一张这个表
uint16_t SocOcvTab[101]=
{
	3282, // 0%~1%	
	3309, 3334, 3357, 3378, 3398, 3417, 3434, 3449, 3464, 3477,	// 0%~10%
	3489, 3500, 3510, 3520, 3528, 3536, 3543, 3549, 3555, 3561,	// 11%~20%
	3566, 3571, 3575, 3579, 3583, 3586, 3590, 3593, 3596, 3599,	// 21%~30%
	3602, 3605, 3608, 3611, 3615, 3618, 3621, 3624, 3628, 3632,	// 31%~40%
	3636, 3640, 3644, 3648, 3653, 3658, 3663, 3668, 3674, 3679,	// 41%~50%
	3685, 3691, 3698, 3704, 3711, 3718, 3725, 3733, 3741, 3748,	// 51%~60%
	3756, 3765, 3773, 3782, 3791, 3800, 3809, 3818, 3827, 3837,	// 61%~70%
	3847, 3857, 3867, 3877, 3887, 3897, 3908, 3919, 3929, 3940,	// 71%~80%
	3951, 3962, 3973, 3985, 3996, 4008, 4019, 4031, 4043, 4055,	// 81%~90%
	4067, 4080, 4092, 4105, 4118, 4131, 4145, 4158, 4172, 4185,	// 91~100%
};

const osThreadAttr_t analysisTask_attributes = {
  .name = "analysisTask",
  .stack_size = ANALYSISI_TASK_STACK_SIZE,
  .priority = (osPriority_t) osPriorityNormal3,
};


static void BMS_AnalysisTaskEntry(void *paramter);


static void BMS_AnalysisEasy(void);
static void BMS_AnalysisCalCap(void);
static void BMS_AnalysisSocCheck(void);
static void BMS_AnalysisCapAndSocInit(void);






// 电池状态分析模块初始化
/** @brief 初始化分析任务和SOC/容量计算模块。无参数无返回值；应在监控数据可用后由系统初始化流程调用。 */
void BMS_AnalysisInit(void)
{
	osThreadId_t thread;

	thread = osThreadNew(BMS_AnalysisTaskEntry, NULL, &analysisTask_attributes);

	if (thread == NULL)
	{
		BMS_ERROR("Create Task Fail");
	}
}


// 电池状态分析任务线程入口。
/** @brief 分析任务入口，按固定周期执行基础统计、温度修正、容量积分和SOC检查。任务参数未使用。 */
static void BMS_AnalysisTaskEntry(void *paramter)
{
	BMS_AnalysisCapAndSocInit();
	while(1)
	{			
		BMS_AnalysisEasy();
		BMS_AnalysisCalCap();
		BMS_AnalysisSocCheck();
		osDelay(ANALYSISI_TASK_PERIOD);
	}
}


// 简单分析：通过数据直接计算即可得到结果。
/** @brief 执行轻量级电池数据分析，更新平均电压、最大压差和实际功率等结果。 */
static void BMS_AnalysisEasy(void)
{
	uint8_t index;

	// 最大电压差
	BMS_AnalysisData.MaxVoltageDifference = BMS_MonitorData.CellData[BMS_GlobalParam.Cell_Real_Number - 1].CellVoltage - BMS_MonitorData.CellData[0].CellVoltage;
	
	
	// 平均电压
	for (index = 0, BMS_AnalysisData.AverageVoltage = 0; index < BMS_GlobalParam.Cell_Real_Number; index++)
	{
		BMS_AnalysisData.AverageVoltage += BMS_MonitorData.CellVoltage[index];
	}
	BMS_AnalysisData.AverageVoltage /= BMS_GlobalParam.Cell_Real_Number;
	
	
	// 实时功率
	BMS_AnalysisData.PowerReal = BMS_MonitorData.BatteryVoltage * BMS_MonitorData.BatteryCurrent;	


	// 查找当前电芯电压最大值和最小值。
	BMS_AnalysisData.CellVoltMax = BMS_MonitorData.CellData[BMS_GlobalParam.Cell_Real_Number - 1].CellVoltage;
	BMS_AnalysisData.CellVoltMin = BMS_MonitorData.CellData[0].CellVoltage;
}





// 温度校准：锂电池会因为温度变化而影响电池容量。
/** @brief 根据电芯温度修正可用容量比例。温度数据来自监控模块，结果写入分析数据。 */
static void BMS_AnalysisTempCal(void)
{
	static int16_t LastTemp = 0;

	uint8_t  Ratio; 	// 校准比率
	uint16_t RateTemp;	
	int16_t MinTemp = BMS_MonitorData.CellTemp[0] * 10;


	if (BMS_MonitorData.CellTempEffectiveNumber == 0)
	{
		return;
	}


	// 判断温度变化是否超过1摄氏度。
	if( MinTemp > LastTemp)  
	{
		if (MinTemp - LastTemp >= 10)
		{
			LastTemp = MinTemp;
		}
		else
		{
			return;
		}
	}
	else
	{
		if (LastTemp - MinTemp >= 10) 
		{
			LastTemp = MinTemp;
		}
		else
		{
			return;
		}
	}

	
	// 确定校准比率
	if (MinTemp >= 250)                             
	{
		Ratio = 1;
	}
	else if (MinTemp >= 100 && MinTemp < 250)   
	{   
		Ratio = 2;
	}
	else if (MinTemp >= 0 && MinTemp < 100)      
	{   
		Ratio = 3;
	}
	else if (MinTemp >= -200 && MinTemp < -10)    
	{   
		Ratio = 4;
	}
	else if (MinTemp >= -300 && MinTemp < -200)    
	{   
		Ratio = 5;
	}
	else
	{
		Ratio = 6;                                       
	}


	RateTemp = 1000 + Ratio * (MinTemp - 250) / 10;
	if(RateTemp > TEMP_CAP_RATE_LIMITH_HIGH )
	{
		RateTemp = TEMP_CAP_RATE_LIMITH_HIGH;
	}
	else if(RateTemp < TEMP_CAP_RATE_LIMITL_LOW)
	{
		RateTemp = TEMP_CAP_RATE_LIMITL_LOW;
	}

	// 实时容量
	BMS_AnalysisData.CapacityReal = BMS_AnalysisData.CapacityRated * RateTemp / 1000;

	// 剩余容量
	BMS_AnalysisData.CapacityRemain = BMS_AnalysisData.CapacityReal * BMS_AnalysisData.SOC;
}



// 实时校准容量涉及因素:温度、完整充放电、老化等等
/** @brief 根据电流积分计算本次运行的容量变化。电流单位为A，时间换算必须与采样周期一致。 */
static void BMS_AnalysisCalCap(void)
{	
	BMS_AnalysisTempCal();
}


// 根据最低电芯电压计算SOC，用于上电和长时间静止状态下的校准。
/** @brief 通过OCV查表将电芯电压转换为SOC。参数电压单位为mV，返回值通常为0到100的百分比。 */
static uint16_t BMS_AnalysisOcvToSoc(uint16_t voltage)
{
	uint16_t soc = 0;
	
	if (voltage <= SocOcvTab[0])
	{
		soc  = 0;
	}
	else if (voltage >= SocOcvTab[100])
	{
		soc = 1000;
	}
	else
	{
		uint16_t index = right_bound(SocOcvTab, 0, 100, voltage);

		if (voltage == SocOcvTab[index])
		{
			// 将SOC限制在有效范围内。
			soc = index * 10;
		}
		else
		{
			// 计算百分比后的小数点
			soc = index * 10 + (( SocOcvTab[index] - voltage) * 10) / (SocOcvTab[index] - SocOcvTab[index + 1]);		
		}
	}
	
	return soc;
}

// 开路电压法soc计算
/** @brief 使用开路电压法计算SOC，适合静置或低电流状态；运行中应注意极化电压影响。 */
static void BMS_AnalysisOcvSocCalculate(void)
{
	// 进入睡眠的条件：待机时间足够长且没有电芯处于均衡。
	if (BMS_GlobalParam.SysMode == BMS_MODE_SLEEP)
	{
		// 等待一段时间使电压趋于稳定，防止均衡刚结束时误判。
		osDelay(BALANCE_VOLT_RISE_DELAY);

		// 使用开路电压法进行SOC校准。
		BMS_AnalysisData.SOC = BMS_AnalysisOcvToSoc(BMS_MonitorData.CellData[0].CellVoltage  * 1000) / 1000.0;

		// 剩余容量 = 实际容量 * soc
		BMS_AnalysisData.CapacityRemain = BMS_AnalysisData.CapacityReal * BMS_AnalysisData.SOC;		
	}
}


// 安时积分法soc计算
// 待机模式下，最低电压达到过压保护解除阈值时将SOC设为100%。
// 待机模式下，最低电压低于欠压保护阈值时将SOC设为0%。
// 充电时对测量得到的电流进行积分。
// 放电时对测量得到的电流进行积分。
// SOC = 实时积分得到的剩余容量 / 电池包实际可用容量。
/** @brief 使用安时积分法更新SOC。该方法依赖电流方向、采样周期和初始容量准确性。 */
static void BMS_AnalysisAHSocCalculate(void)
{
	// 取电流绝对值，并将A/s换算为Ah。
	float CurrentValue = abs((int32_t)(BMS_MonitorData.BatteryCurrent * 1000)) / 1000.0 / 3600;

	
	if (BMS_GlobalParam.SysMode == BMS_MODE_STANDBY)
	{
		if (BMS_MonitorData.CellData[0].CellVoltage >= BMS_ProtectParam.OVProtect)
		{
			BMS_AnalysisData.SOC = 1;
		}
		else if (BMS_MonitorData.CellData[0].CellVoltage <= BMS_ProtectParam.UVProtect)
		{
			BMS_AnalysisData.SOC = 0;
		}
	}

	if (BMS_GlobalParam.SysMode == BMS_MODE_CHARGE)
	{
		if(BMS_AnalysisData.CapacityReal >= (BMS_AnalysisData.CapacityRemain + CurrentValue))
		{
			BMS_AnalysisData.CapacityRemain += CurrentValue;
		}
		else
		{
			BMS_AnalysisData.CapacityRemain = BMS_AnalysisData.CapacityReal;
		}
	}
	else if (BMS_GlobalParam.SysMode == BMS_MODE_DISCHARGE)
	{
		if(BMS_AnalysisData.CapacityRemain >= CurrentValue)
		{
			BMS_AnalysisData.CapacityRemain -= CurrentValue;
		}
		else
		{		
			BMS_AnalysisData.CapacityRemain = 0;
		}
	}

	/*
	else  // 是否考虑静态时的漏电电流，当前按0mA处理。
	{
		if(BMS_AnalysisData.CapacityRemain >= 0.01)   
		{
		 	BMS_AnalysisData.CapacityRemain -= 0.01;
		}
		else
		{
		 	BMS_AnalysisData.CapacityRemain = 0;
		}
	}
	*/

	BMS_AnalysisData.SOC = BMS_AnalysisData.CapacityRemain / BMS_AnalysisData.CapacityReal;
	if (BMS_AnalysisData.SOC > 1)
	{
		BMS_AnalysisData.SOC = 1;
	}
}

// SOC边界检查。
/** @brief 综合OCV和安时积分结果检查SOC边界，防止结果超过0到1范围。 */
static void BMS_AnalysisSocCheck(void)
{
	BMS_AnalysisOcvSocCalculate();
	BMS_AnalysisAHSocCalculate();
}


// 容量和SOC上电初始化。
/** @brief 根据电芯初始电压设置容量和SOC初值。上电时调用，不能替代长期运行中的安时积分。 */
static void BMS_AnalysisCapAndSocInit(void)
{
	uint16_t temp = BMS_MonitorData.CellData[0].CellVoltage * 1000;
	
	// soc计算
	BMS_AnalysisData.SOC = BMS_AnalysisOcvToSoc(BMS_MonitorData.CellData[0].CellVoltage  * 1000) / 1000.0;

	// 实际容量后续可进一步完善，涉及完整充放电流计算、老化损耗、温度特性曲线和信息存储模块。
	BMS_AnalysisData.CapacityReal = BMS_AnalysisData.CapacityRated;

	// 剩余容量 = 实际容量 * soc
	BMS_AnalysisData.CapacityRemain = BMS_AnalysisData.CapacityReal * BMS_AnalysisData.SOC; 
}
