#define BMS_DBG_TAG "Monitor"

#include <stdio.h>
#include <rtthread.h>

#include "bms_monitor.h"

#include "bms_hal_monitor.h"

#include "bms_energy.h"
#include "bms_global.h"
#include "bms_debug.h"



/* 监控任务栈大小，单位为FreeRTOS StackType_t元素，不是字节数。 */
#define MONITOR_TASK_STACK_SIZE	512
/* 监控任务优先级，数值越大表示优先级越高。 */
#define MONITOR_TASK_PRIORITY	9
/* 监控任务内部循环间隔，单位为RTOS Tick。 */
#define MONITOR_TASK_TIMESLICE	25

/* 监控任务的基础执行周期，当前配置下用于周期计数，单位为毫秒。 */
#define MONITOR_TASK_PERIOD		250



/* 电芯电压采样周期，单位为毫秒。 */
#define UPDATE_CELL_VOLTAGE_CYCLE	250
/* 电池总电压采样周期，单位为毫秒。宏名称保持原工程定义不变。 */
#define UPDAYE_BAT_VOLTAGE_CYCLE	250
/* 电芯温度采样周期，单位为毫秒。 */
#define UPDATE_CELL_TEMP_CYCLE		2000
/* 电池电流采样周期，单位为毫秒。 */
#define UPDATE_BAT_CURRENT_CYCLE	1000




/* 保存监控任务最近一次采集结果的全局数据对象。 */
BMS_MonitorDataTypedef BMS_MonitorData;



/* 外部中断请求电流采样的标志，true表示等待监控任务处理。 */
static bool FlagSampleIntCur = false;

/* 电芯电压采样使能标志，初始化为允许采样。 */
static bool FlagCellVoltage = true;
/* 电池总电压采样使能标志，初始化为允许采样。 */
static bool FlagBatVoltage = true;
/* 电芯温度采样使能标志，初始化为允许采样。 */
static bool FlagCellTemp = true;
/* 电池电流采样使能标志，初始化为允许采样。 */
static bool FlagBatCurrent = true;



/* 电芯电压采样周期累计计数，单位与MONITOR_TASK_PERIOD一致。 */
static uint16_t CountCellVoltage = 0;
/* 电池总电压采样周期累计计数，单位与MONITOR_TASK_PERIOD一致。 */
static uint16_t CountBatVoltage = 0;
/* 电芯温度采样周期累计计数，单位与MONITOR_TASK_PERIOD一致。 */
static uint16_t CountCellTemp = 0;
/* 当前未使用的电池电流周期计数，保留以兼容原工程结构。 */
//static uint16_t CountBatCurrent = 0;


/*
 * 监控任务的CMSIS-RTOS2属性对象。
 * name用于调试器识别任务；stack_size指定任务栈大小；priority指定任务优先级。
 * 注意：stack_size的具体单位由CMSIS-RTOS2适配层处理，不能与普通字节数组混淆。
 */
const osThreadAttr_t monitorTask_attributes = {
  .name = "monitorTask",
  .stack_size = MONITOR_TASK_STACK_SIZE,
  .priority = (osPriority_t) osPriorityNormal5,
};

/* 监控任务入口。参数：paramter为任务创建时传入的参数，本任务未使用。
 * 返回：不返回；持续执行采样和模式判断。周期：250ms。 */;
/* 周期单位为 ms；不同采样项通过计数器降低 I2C 访问频率。 */
/* 按配置周期读取电压、温度和电流。参数和返回值：无。
 * 副作用：更新BMS_MonitorData及内部采样计数器。 */;
/* 电流单位为 A：正值表示充电，负值表示放电，±0.02 A 是待机死区。 */
/* 根据电流判断充电、放电、待机或睡眠模式。参数和返回值：无。
 * 注意：电流阈值使用A，±0.02A为待机死区。 */;


/* 创建BMS监控任务。参数：无。返回：无。
 * 注意：必须在FreeRTOS调度器启动后由BMS_SysInitialize调用。 */
static void BMS_MonitorTaskEntry(void *paramter);
static void BMS_MonitorBattery(void);
static void BMS_MonitorSysMode(void);

/**
 * @brief 初始化BMS监控模块并创建监控任务。
 * @param 无。
 * @return 无。
 * @note 应在RTOS内核初始化后调用；任务创建失败时需结合系统日志排查堆空间和任务配置。
 */
void BMS_MonitorInit(void)
{
	osThreadId_t thread;
	
	thread = osThreadNew(BMS_MonitorTaskEntry, NULL, &monitorTask_attributes);

   if (thread == NULL)
   {
	   BMS_ERROR("Create Task Fail");
   }
}

/* 监控任务入口。参数：paramter为任务创建时传入的参数，本任务未使用。
 * 返回：不返回；持续执行采样和模式判断。周期：250ms。 */
/**
 * @brief 监控任务入口函数。
 * @param paramter RTOS任务参数，本实现未使用。
 * @return 无；函数通常不会返回，而是在循环中持续执行监控流程。
 * @note 任务周期由MONITOR_TASK_PERIOD决定，不能在此函数中执行长时间阻塞操作。
 */
static void BMS_MonitorTaskEntry(void *paramter)
{
	while (1)
	{
		BMS_MonitorBattery();
		BMS_MonitorSysMode();
		osDelay(MONITOR_TASK_PERIOD);
	}
}


// 监控电池各项数据
/* 周期单位为 ms；不同采样项通过计数器降低 I2C 访问频率。 */
/* 按配置周期读取电压、温度和电流。参数和返回值：无。
 * 副作用：更新BMS_MonitorData及内部采样计数器。 */
/**
 * @brief 按配置周期采集电芯电压、电池电压、温度和电流。
 * @param 无。
 * @return 无；采样结果写入BMS_MonitorData。
 * @note 各项采样是否执行由对应Flag控制，采样周期使用RTOS节拍累计实现。
 */
static void BMS_MonitorBattery(void)
{
	// 单体电芯电压
	CountCellVoltage += MONITOR_TASK_PERIOD;
	if (FlagCellVoltage == true && CountCellVoltage >= UPDATE_CELL_VOLTAGE_CYCLE)
	{
		Bms_HalMonitorCellVoltage();
		CountCellVoltage = 0;
	}
	else if (FlagCellVoltage == false)
	{
		CountCellVoltage = 0;
	}
	
	// 电池组电压
	CountBatVoltage += MONITOR_TASK_PERIOD;
	if (FlagBatVoltage == true && CountBatVoltage >= UPDAYE_BAT_VOLTAGE_CYCLE)
	{
		Bms_HalMonitorBatteryVoltage();
		CountBatVoltage = 0;		
	}
	else if (FlagBatVoltage == false)
	{
		CountBatVoltage = 0;
	}

	// 电池温度
	CountCellTemp += MONITOR_TASK_PERIOD;
	if (FlagCellTemp == true && CountCellTemp++ >= UPDATE_CELL_TEMP_CYCLE)
	{
		Bms_HalMonitorCellTemperature();
		CountCellTemp = 0;
	}
	else if (FlagCellTemp == false)
	{
		CountCellTemp = 0;
	}


	/* 电流采样由软件触发 
	CountBatCurrent += MONITOR_TASK_PERIOD;
	if (FlagBatCurrent == true && CountBatCurrent >= UPDATE_BAT_CURRENT_CYCLE)
	{
		CountBatCurrent = 0;
		Bms_HalMonitorBatteryCurrent();
	}
	else if (FlagBatCurrent == false)
	{
		CountBatCurrent = 0;
	}
	*/


	/* 电流采样由硬件中断触发,太麻烦了,每次烧写都得重新给BQ重新下上电 */
	if (FlagSampleIntCur == true && FlagBatCurrent == true)
	{
		Bms_HalMonitorBatteryCurrent();
		FlagSampleIntCur = false;		
	}
}



// 系统模式监控
// BatteryCurrent > 20mA || BatteryCurrent < -20mA  处于非睡眠模式
// BatteryCurrent < 20mA || BatteryCurrent > -20mA  处于待机模式或者睡眠模式
// BatteryCurrent <= -20mA 处于放电模式
// BatteryCurrent >=  20mA 处于充电模式
/* 电流单位为 A：正值表示充电，负值表示放电，±0.02 A 是待机死区。 */
/* 根据电流判断充电、放电、待机或睡眠模式。参数和返回值：无。
 * 注意：电流阈值使用A，±0.02A为待机死区。 */
/**
 * @brief 根据电池电流和保护状态更新系统运行模式。
 * @param 无。
 * @return 无；结果写入全局BMS状态，并在模式变化时输出调试信息。
 * @note 电流正负方向和待机死区必须与硬件电流采样定义保持一致。
 */
static void BMS_MonitorSysMode(void)
{
	static BMS_SysModeTypedef SysModeBackup = BMS_MODE_NULL;
	static uint32_t StandbyCount = 0;
	
	if (BMS_GlobalParam.SysMode == BMS_MODE_SLEEP)
	{
		if ((BMS_MonitorData.BatteryCurrent >= 0.02) || (BMS_MonitorData.BatteryCurrent <= -0.02))
		{
			// 可以加唤醒处理逻辑
			
			BMS_GlobalParam.SysMode = BMS_MODE_STANDBY;
			BMS_INFO("Wake Up");
		}
		return;
	}

	
	if (BMS_MonitorData.BatteryCurrent < 0.02 && BMS_MonitorData.BatteryCurrent > -0.02)
	{
		BMS_GlobalParam.SysMode = BMS_MODE_STANDBY;	
		
		if (StandbyCount >= BMS_ENTRY_SLEEP_TIME * 60000)
		{
			// 没有电芯正在均衡的情况下才进入睡眠
			if (osSemaphoreAcquire(BalanceSem, 0) == RT_EOK)
			{
				// 可以加睡眠低功耗处理逻辑
				
				StandbyCount = 0;
				
				BMS_GlobalParam.SysMode = BMS_MODE_SLEEP;

				osSemaphoreRelease(BalanceSem);
				
				BMS_INFO("Entry Sleep Mode");
			}
		}
		else
		{
			StandbyCount += MONITOR_TASK_PERIOD;
		}


		// 调试用
		if (SysModeBackup != BMS_MODE_STANDBY)
		{
			SysModeBackup = BMS_MODE_STANDBY;
			BMS_INFO("Entry Standby Mode");
		}
	}
	else if (BMS_MonitorData.BatteryCurrent >= 0.02)
	{
		StandbyCount = 0;
		BMS_GlobalParam.SysMode = BMS_MODE_CHARGE;

		// 调试用
		if (SysModeBackup != BMS_MODE_CHARGE)
		{
			SysModeBackup = BMS_MODE_CHARGE;
			BMS_INFO("Entry Charge Mode");
		}
	}
	else if (BMS_MonitorData.BatteryCurrent <= -0.02)
	{
		StandbyCount = 0;
		BMS_GlobalParam.SysMode = BMS_MODE_DISCHARGE;

		// 调试用
		if (SysModeBackup != BMS_MODE_DISCHARGE)
		{
			SysModeBackup = BMS_MODE_DISCHARGE;
			BMS_INFO("Entry Discharge Mode");
		}
	}
}










/**
 * @brief 开启或关闭电芯电压采样。
 * @param NewState BMS_STATE_ENABLE表示允许采样，BMS_STATE_DISABLE表示停止采样。
 * @return 无。
 * @note 这里只修改采样标志，实际采样由监控任务执行。
 */
void BMS_MonitorStateCellVoltage(BMS_StateTypedef NewState)
{
	if (NewState == BMS_STATE_ENABLE)
	{
		FlagCellVoltage = true;
	}
	else if (NewState == BMS_STATE_DISABLE)
	{
		FlagCellVoltage = false;
	}
}

/**
 * @brief 开启或关闭电池总电压采样。
 * @param NewState 采样功能状态。
 * @return 无。
 * @note 禁止采样后会清零对应的软件周期计数，重新启用后重新计时。
 */
void BMS_MonitorStateBatVoltage(BMS_StateTypedef NewState)
{
	if (NewState == BMS_STATE_ENABLE)
	{
		FlagBatVoltage = true;
	}
	else if (NewState == BMS_STATE_DISABLE)
	{
		FlagBatVoltage = false;
	}
}

/**
 * @brief 开启或关闭电芯温度采样。
 * @param NewState 采样功能状态。
 * @return 无。
 * @note 温度数据的有效性还取决于硬件温度通道配置和传感器连接状态。
 */
void BMS_MonitorStateCellTemp(BMS_StateTypedef NewState)
{
	if (NewState == BMS_STATE_ENABLE)
	{
		FlagCellTemp = true;
	}
	else if (NewState == BMS_STATE_DISABLE)
	{
		FlagCellTemp = false;
	}	
}

/**
 * @brief 开启或关闭电池电流采样。
 * @param NewState 采样功能状态。
 * @return 无。
 * @note 电流采样通常影响充放电模式判断，应避免无故关闭。
 */
void BMS_MonitorStateBatCurrent(BMS_StateTypedef NewState)
{
	if (NewState == BMS_STATE_ENABLE)
	{
		FlagBatCurrent = true;
	}
	else if (NewState == BMS_STATE_DISABLE)
	{
		FlagBatCurrent = false;
	}	
}





/* 硬件电流告警回调。参数和返回值：无。
 * 副作用：设置电流采样请求标志，由监控任务完成实际I2C读取。
 * 注意：该函数可能由中断回调路径调用，应保持短小。 */
void BMS_MonitorHwCurrent(void)
{
	FlagSampleIntCur = true;
}

