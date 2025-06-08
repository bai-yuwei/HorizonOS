/******************************************************************************
* @file    Linked_List.c
* @brief   链表相关的文件.
* @details This is the detail description.
* @author  ywBai <yw_bai@outlook.com>
* @date    2025年05月24日 (created)
* @version 0.0.1
* @par Copyright (C):
*          Bai, yuwei. All Rights Reserved.
* @par Encoding:
*          UTF-8
* @par Description        :
* 1. Hardware Descriptions:
*      None.
* 2. Program Architecture:
*      None.
* 3. File Usage:
*      None.
* 4. Limitations:
*      None.
* 5. Else:
*      None.
* @par Modification:
* Date          : 2025年05月24日;
* Revision         : 0.0.1;
* Author           : ywBai;
* Contents         :
******************************************************************************/

#include "Linked_List.h"

void doubly_Linked_List_Init(doubly_linked_list_t *list)
{
    list->head = nullptr;
    list->tail = nullptr;
    list->size = 0;
}

doubly_linked_list_t create_Doubly_Linked_List(void)
{
    doubly_linked_list_t list;
    doubly_Linked_List_Init(&list);
    return list;
}

void doubly_List_Move(doubly_linked_list_t *dst, doubly_linked_list_t *src)
{
    dst->head = src->head;
    dst->tail = src->tail;
    dst->size = src->size;
    doubly_Linked_List_Init(src);
}

/**
 * @brief 将一个双向链表拼接到另一个双向链表的尾部。
 * 
 * 此函数将源双向链表 src 拼接到目标双向链表 dst 的尾部。
 * 拼接完成后，源链表会被重置为空链表。
 * 
 * @param dst 指向目标双向链表的指针，源链表将被拼接到该链表尾部。
 * @param src 指向源双向链表的指针，该链表将被拼接到目标链表尾部。
 */
void doubly_Lost_Concatenate(doubly_linked_list_t *dst, doubly_linked_list_t *src)
{
    // 若源链表的节点数量为 0，即源链表为空，无需拼接，直接返回
    if (src->size == 0)
    {
        return;
    }
    // 若目标链表的节点数量为 0，即目标链表为空
    if (dst->size == 0)
    {
        // 直接将源链表移动到目标链表，实现拼接
        doubly_List_Move(dst, src);
        return;
    }
    // 将目标链表的尾节点的 next 指针指向源链表的头节点
    dst->tail->next = src->head;
    // 将源链表的头节点的 prev 指针指向目标链表的尾节点
    src->head->prev = dst->tail;
    // 更新目标链表的尾节点为源链表的尾节点
    dst->tail = src->tail;
    // 更新目标链表的节点数量，加上源链表的节点数量
    dst->size += src->size;
    // 初始化源链表，将其重置为空链表
    doubly_Linked_List_Init(src);
}

void doubly_Linked_List_Append(doubly_linked_list_t *list, doubly_linked_list_node_t *node)
{
    node->next = nullptr;
    if (list->size == 0)
    {
        node->prev = nullptr;
        list->head = node;
        list->tail = node;
    }
    else if (list->tail != nullptr)
    {
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
    }
    else
    {
        monitor_Printf("Error: doubly_Linked_List_Append() failed\n");
        return;
    }
    list->size++;
}

void doubly_Linked_List_Insert(doubly_linked_list_t *list, doubly_linked_list_node_t *node, doubly_linked_list_node_t *prevNode)
{
    if (list == nullptr || node == nullptr)
    {
        monitor_Printf("Error: doubly_Linked_List_Insert() failed\n");
        return;
    }
    if (prevNode == nullptr)
    {
        node->prev = nullptr;
        node->next = list->head;
        if (list->head != nullptr)
        {
            list->head->prev = node;
        }
        list->head = node;
        if (list->tail == nullptr)
        {
            list->tail = node;
        }
    }
    else
    {
        node->prev = prevNode;
        node->next = prevNode->next;
        if (prevNode->next != nullptr)
        {
            prevNode->next->prev = node;
        }
        prevNode->next = node;
        if (prevNode == list->tail)
        {
            list->tail = node;
        }
    }
    list->size++;
}

void doubly_Linked_List_Insert_Head(doubly_linked_list_t *list, doubly_linked_list_node_t *node)
{
    doubly_Linked_List_Insert(list, node, nullptr);
}

void doubly_Linked_List_Remove(doubly_linked_list_t *list, doubly_linked_list_node_t *node)
{
    if (list == nullptr || node == nullptr)
    {
        monitor_Printf("Error: doubly_Linked_List_Remove() failed\n");
        return;
    }
    if (node->prev != nullptr)
    {
        node->prev->next = node->next;
    }
    if (node->next!= nullptr)
    {
        node->next->prev = node->prev;
    }
    if (node == list->head)
    {
        list->head = node->next;
    }
    if (node == list->tail)
    {
        list->tail = node->prev;
    }
    list->size--;
}

void doubly_Linked_List_Append_Data(doubly_linked_list_t *list, void *dataPtr)
{
    doubly_linked_list_node_t* node = (doubly_linked_list_node_t*)kmalloc(sizeof(doubly_linked_list_node_t), NOT_PAGE_ALIGNED);
    node->dataPtr = (void*)dataPtr;
    doubly_Linked_List_Append(list, node);
}

void doubly_Linked_List_Remove_Data(doubly_linked_list_t *list, void *dataPtr)
{
    doubly_linked_list_node_t* node = list->head;
    while(node != nullptr)
    {
        if (node->dataPtr == dataPtr)
        {
            doubly_Linked_List_Remove(list, node);
            break;
        }
        node = node->next;
    }
}

void doubly_Linked_Test()
{
    monitor_Printf("linked_list test ...\n");
    doubly_linked_list_t list = create_Doubly_Linked_List();
    uint64 test = sizeof(doubly_linked_list_node_t);
    doubly_linked_list_node_t* node1 = (doubly_linked_list_node_t*)kmalloc(sizeof(doubly_linked_list_node_t), NOT_PAGE_ALIGNED);
    doubly_linked_list_node_t* node2 = (doubly_linked_list_node_t*)kmalloc(sizeof(doubly_linked_list_node_t), NOT_PAGE_ALIGNED);
    doubly_linked_list_node_t* node3 = (doubly_linked_list_node_t*)kmalloc(sizeof(doubly_linked_list_node_t), NOT_PAGE_ALIGNED);
    doubly_linked_list_node_t* node4 = (doubly_linked_list_node_t*)kmalloc(sizeof(doubly_linked_list_node_t), NOT_PAGE_ALIGNED);
    doubly_Linked_List_Append(&list, node1);
    ASSERT(list.size == 1);
    ASSERT(list.head == node1);
    ASSERT(list.tail == node1);
    ASSERT(node1->prev == nullptr);
    ASSERT(node1->next == nullptr);
    doubly_Linked_List_Remove(&list, node1);
    ASSERT(list.size == 0);
    ASSERT(list.head == nullptr);
    ASSERT(list.tail == nullptr);
    doubly_Linked_List_Append(&list, node1);
    ASSERT(list.size == 1);
    ASSERT(list.head == node1);
    ASSERT(list.tail == node1);
    ASSERT(node1->prev == nullptr);
    ASSERT(node1->next == nullptr);
    doubly_Linked_List_Append(&list, node2);
    ASSERT(list.size == 2);
    ASSERT(list.head == node1);
    ASSERT(list.tail == node2);
    ASSERT(node1->prev == nullptr);
    ASSERT(node1->next == node2);
    ASSERT(node2->next == nullptr);
    ASSERT(node2->prev == node1);
    doubly_Linked_List_Insert(&list, node3, node1);
    ASSERT(list.size == 3);
    ASSERT(list.head == node1);
    ASSERT(list.tail == node2);
    ASSERT(node1->prev == nullptr);
    ASSERT(node1->next == node3);
    ASSERT(node2->next == nullptr);
    ASSERT(node2->prev == node3);
    ASSERT(node3->next == node2);
    ASSERT(node3->prev == node1);
    doubly_Linked_List_Remove(&list, node2);
    doubly_Linked_List_Remove(&list, node3);
    doubly_Linked_List_Remove(&list, node1);
    kfree(node1);
    kfree(node2);
    kfree(node3);
    monitor_Print_With_Color("ok\n", COLOR_GREEN);
}


