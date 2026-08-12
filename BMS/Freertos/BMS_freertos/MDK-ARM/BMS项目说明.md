# BMS项目说明

## 项目定位

本工程运行在 `STM32F103C8` 上，底层使用 STM32 HAL，系统调度使用 CMSIS-RTOS2 接口映射的 FreeRTOS，电池监测与保护芯片为 BQ769x0。用户业务代码集中在 `User` 目录。

## 软件分层

```mermaid
%%{init:{'theme':'default','themeVariables':{'fontSize':'12px'}}}%%
flowchart TD
    A["StartDefaultTask"] --> B["BMS_SysInitialize"]
    B --> C["drv_soft_i2c\nGPIO模拟I2C"]
    C --> D["BQ769x0驱动\n寄存器/采样/保护报警"]
    B --> E["监测任务\n电芯电压/温度/电流"]
    B --> F["保护任务\n过压/欠压/过流/温度"]
    B --> G["分析任务\n统计/SOC/容量"]
    B --> H["能量任务\nCHG/DSG/均衡"]
    D --> E
    E --> F
    E --> G
    F --> H
    G --> H
    H --> I["bms_hal_control\n控制充放电与均衡硬件"]
    style B fill:#4f81bd,color:#fff
    style D fill:#70ad47,color:#fff
    style F fill:#c0504d,color:#fff
    style H fill:#ed7d31,color:#fff
```

## 启动与运行

- `MX_FREERTOS_Init()` 创建 `defaultTask`。
- `StartDefaultTask()` 调用 `BMS_SysInitialize()`。
- `I2C_BusInitialize()` 配置开漏 GPIO；`BQ769X0_Initialize()` 注册报警回调并写入保护配置。
- 监测、保护、分析、能量模块分别创建任务；通信模块当前在应用入口中关闭。
- 默认任务每 500 ms 翻转 LED。它只是运行指示，不代表 BMS 安全状态正常。

## 关键数据流

```mermaid
%%{init:{'theme':'default','themeVariables':{'fontSize':'12px'}}}%%
sequenceDiagram
    participant BQ as BQ769x0
    participant MON as 监测任务
    participant PRO as 保护任务
    participant ANA as 分析任务
    participant ENG as 能量任务
    participant HAL as 控制HAL
    BQ->>MON: 读取电芯/温度/电流
    MON->>PRO: 更新实时测量值
    MON->>ANA: 更新SOC和统计输入
    PRO->>ENG: 报警状态与允许状态
    ANA->>ENG: SOC、模式和均衡判断
    ENG->>HAL: CHG/DSG/均衡开关
    HAL->>BQ: 控制寄存器或外部MOS
```

## 参数与风险提示

当前配置是最多 5 节电芯、1 路温度，默认按三元锂参数设置。`User/Bms/bms_config.h` 中的电压、温度、电流和延时是安全相关参数，必须结合实际电芯、采样电阻、MOS 驱动极性和硬件原理图重新校准。工程已有构建产物不能替代实机验证，尤其要单独验证报警回调、CHG/DSG 极性、均衡电流和断线场景。
