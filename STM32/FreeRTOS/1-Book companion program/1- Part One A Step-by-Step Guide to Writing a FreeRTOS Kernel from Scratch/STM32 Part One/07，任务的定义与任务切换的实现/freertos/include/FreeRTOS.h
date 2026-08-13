#ifndef INC_FREERTOS_H
#define INC_FREERTOS_H

#include "FreeRTOSConfig.h"
#include "projdefs.h"
#include "portable.h"
#include "list.h"
/* TCB只保存本节内核切换必需的数据：pxTopOfStack必须是第一个成员，因为PendSV汇编
 * 通过TCB首地址直接取栈顶；xStateListItem把任务连接到就绪链表；pxStack用于重新计算
 * 栈顶；pcTaskName用于调试观察。 */

typedef struct tskTaskControlBlock
{
	volatile StackType_t    *pxTopOfStack;    /* 栈顶 */

	ListItem_t			    xStateListItem;   /* 任务节点 */
    
    StackType_t             *pxStack;         /* 任务栈起始地址 */
	                                          /* 任务名称，字符串形式 */
	char                    pcTaskName[ configMAX_TASK_NAME_LEN ];  
} tskTCB;
typedef tskTCB TCB_t;

#endif /* INC_FREERTOS_H */
