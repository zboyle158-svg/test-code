#include "board.h"

void Board_Initialize(void)
{
	/*
	 * 板级外设统一入口。
	 *
	 * 这里属于“板级适配层”：上层BMS只调用业务接口，不需要知道CAN
	 * 控制器、RS485收发器分别挂在哪个GPIO或串口上。以后更换硬件时，
	 * 优先在本层替换初始化函数，可以减少对BMS核心算法的影响。
	 */

	/* 当前工程启用CAN；RS485初始化仍保持原工程的关闭状态。 */
	//RS485_Initialize();
	CAN_Initialize();
}



