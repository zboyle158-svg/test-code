#include "FreeRTOS.h"
#include <stdlib.h>
#include "list.h"


/* 链表根节点初始化 */
void vListInitialise( List_t * const pxList )
{
	/* 空链表从尾部哨兵开始遍历，下一步即可到达第一个普通节点。 */
	pxList->pxIndex = ( ListItem_t * ) &( pxList->xListEnd );

	/* 尾部哨兵使用最大值，保证升序插入时始终位于普通节点之后。 */
	pxList->xListEnd.xItemValue = portMAX_DELAY;

	/* 空循环双向链表：尾部哨兵的前后指针均指向自身。 */
	pxList->xListEnd.pxNext = ( ListItem_t * ) &( pxList->xListEnd );
	pxList->xListEnd.pxPrevious = ( ListItem_t * ) &( pxList->xListEnd );

	/* 计数器只统计普通链表项，不统计尾部哨兵。 */
	pxList->uxNumberOfItems = ( UBaseType_t ) 0U;
}

/* 节点初始化 */
void vListInitialiseItem( ListItem_t * const pxItem )
{
	/* NULL表示该链表项尚未插入任何链表。 */
	pxItem->pvContainer = NULL;
}


/* 将节点插入到链表的尾部 */
void vListInsertEnd( List_t * const pxList, ListItem_t * const pxNewListItem )
{
	/* 在索引节点之前插入，并维护双向链表的四条连接关系。 */
	ListItem_t * const pxIndex = pxList->pxIndex;

	pxNewListItem->pxNext = pxIndex;
	pxNewListItem->pxPrevious = pxIndex->pxPrevious;
	pxIndex->pxPrevious->pxNext = pxNewListItem;
	pxIndex->pxPrevious = pxNewListItem;

	/* 记录归属关系，删除时可直接找到所属链表。 */
	pxNewListItem->pvContainer = ( void * ) pxList;

	/* 普通节点数量加一。 */
	( pxList->uxNumberOfItems )++;
}


/* 将节点按照升序排列插入到链表 */
void vListInsert( List_t * const pxList, ListItem_t * const pxNewListItem )
{
	ListItem_t *pxIterator;
	
	/* 保存新节点的排序值，用于寻找插入位置。 */
	const TickType_t xValueOfInsertion = pxNewListItem->xItemValue;

	/* 从尾部哨兵开始向后查找最后一个不大于新值的节点。 */
	if( xValueOfInsertion == portMAX_DELAY )
	{
		pxIterator = pxList->xListEnd.pxPrevious;
	}
	else
	{
		for( pxIterator = ( ListItem_t * ) &( pxList->xListEnd );
		     pxIterator->pxNext->xItemValue <= xValueOfInsertion; 
			 pxIterator = pxIterator->pxNext )
		{
			/* 循环体为空，迭代表达式负责让指针向后移动。 */
		}
	}

	/* 将新节点插入pxIterator和原下一个节点之间。 */
	pxNewListItem->pxNext = pxIterator->pxNext;
	pxNewListItem->pxNext->pxPrevious = pxNewListItem;
	pxNewListItem->pxPrevious = pxIterator;
	pxIterator->pxNext = pxNewListItem;

	/* 记录该节点的所属链表。 */
	pxNewListItem->pvContainer = ( void * ) pxList;

	/* 插入完成，普通节点数量加一。 */
	( pxList->uxNumberOfItems )++;
}


/* 将节点从链表中删除 */
UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove )
{
	/* 通过pvContainer直接取得节点所属链表，不必从头搜索。 */
	List_t * const pxList = ( List_t * ) pxItemToRemove->pvContainer;

	pxItemToRemove->pxNext->pxPrevious = pxItemToRemove->pxPrevious;
	pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;

	/* 如果索引正指向待删除节点，将索引退回前一个有效节点。 */
	if( pxList->pxIndex == pxItemToRemove )
	{
		pxList->pxIndex = pxItemToRemove->pxPrevious;
	}

	/* 清除归属关系，表示该节点已经脱离链表。 */
	pxItemToRemove->pvContainer = NULL;
	
	/* 普通节点数量减一。 */
	( pxList->uxNumberOfItems )--;

	/* 返回删除后的剩余节点数量。 */
	return pxList->uxNumberOfItems;
}
