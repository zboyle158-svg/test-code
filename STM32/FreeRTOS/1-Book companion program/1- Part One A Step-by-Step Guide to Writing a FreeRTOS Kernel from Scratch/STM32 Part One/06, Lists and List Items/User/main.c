/**
  ************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   《FreeRTOS内核实现与应用开发实战指南》书籍例程
  *           列表与列表项
  ************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 系列 开发板
  * 
  * 官网    :www.embedfire.com
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ************************************************************************
  */
  
/*
*************************************************************************
*                             包含的头文件
*************************************************************************
*/
#include "list.h"                 /* 链表类型和链表操作函数的声明 */

/*
*************************************************************************
*                              全局变量
*************************************************************************
*/

/* 定义链表对象：保存节点数量、遍历索引和尾部哨兵节点。 */
struct xLIST       List_Test;

/* 定义三个普通链表项，内存由全局变量静态分配。 */
struct xLIST_ITEM  List_Item1;
struct xLIST_ITEM  List_Item2;
struct xLIST_ITEM  List_Item3;



/*
************************************************************************
*                                main函数
************************************************************************
*/
/*
* 注意事项：1、该工程使用软件仿真，debug需选择 Ude Simulator
*           2、在Target选项卡里面把晶振Xtal(Mhz)的值改为25，默认是12，
*              改成25是为了跟system_ARMCM3.c中定义的__SYSTEM_CLOCK相同，确保仿真的时候时钟一致
*/
int main(void)
{	
	
    /* 初始化空链表；尾部哨兵的前后指针会先指向自身。 */
    vListInitialise( &List_Test );
    
    /* 初始化链表项，并设置它参与升序排列的辅助值。 */
    vListInitialiseItem( &List_Item1 );
    List_Item1.xItemValue = 1;
    
    /* 链表项初始化后尚未属于任何链表，pvContainer为NULL。 */
    vListInitialiseItem( &List_Item2 );
    List_Item2.xItemValue = 2;
    
    /* 第三个测试节点的排序值为3。 */
    vListInitialiseItem( &List_Item3 );
    List_Item3.xItemValue = 3;
    
    /* 按xItemValue升序插入；调用顺序虽为2、1、3，最终顺序为1、2、3。 */
    vListInsert( &List_Test, &List_Item2 );    
    vListInsert( &List_Test, &List_Item1 );
    vListInsert( &List_Test, &List_Item3 );    
    
    for(;;)
	{
		/* 本例只观察链表结果，不创建任务或启动调度器，因此停留在此处。 */
	}
}
