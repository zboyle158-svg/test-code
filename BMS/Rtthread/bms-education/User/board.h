#ifndef __BOARD_H__
#define __BOARD_H__
	
#include "drv_rs485.h"
#include "drv_can.h"

/* 初始化当前电池板使用的通信和板级外设。 */
void Board_Initialize(void);

#endif

