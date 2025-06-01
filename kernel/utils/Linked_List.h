// /******************************************************************************
// * @file    Linked_List.h
// * @brief   链表相关的头文件.
// * @details This is the detail description.
// * @author  ywBai <yw_bai@outlook.com>
// * @date    2025年05月24日 (created)
// * @version 0.0.1
// * @par Copyright (C):
// *          Bai, yuwei. All Rights Reserved.
// * @par Encoding:
// *          UTF-8
// * @par Description        :
// * 1. Hardware Descriptions:
// *      None.
// * 2. Program Architecture:
// *      None.
// * 3. File Usage:
// *      None.
// * 4. Limitations:
// *      None.
// * 5. Else:
// *      None.
// * @par Modification:
// * Date          : 2025年05月24日;
// * Revision         : 0.0.1;
// * Author           : ywBai;
// * Contents         :
// ******************************************************************************/
// #ifndef LINKED_LIST_H
// #define LINKED_LIST_H

// #include "Std_Types.h"
// #include "Monitor.h"
// #include "Kheap.h"

// struct doubly_linked_list_node {
//     void* dataPtr;
//     struct doubly_linked_list_node *next;
//     struct doubly_linked_list_node *prev;
// } __attribute__((packed));
// typedef struct doubly_linked_list_node doubly_linked_list_node_t;

// struct doubly_linked_list {
//     doubly_linked_list_node_t *head;
//     doubly_linked_list_node_t *tail;
//     uint32 size;
// } __attribute__((packed));
// typedef struct doubly_linked_list doubly_linked_list_t;

// void doubly_Linked_List_Init(doubly_linked_list_t *list);
// doubly_linked_list_t create_Doubly_Linked_List(void);
// void doubly_List_Move(doubly_linked_list_t *dst, doubly_linked_list_t *src);
// void doubly_Lost_Concatenate(doubly_linked_list_t *dst, doubly_linked_list_t *src);
// void doubly_Linked_List_Append(doubly_linked_list_t *list, doubly_linked_list_node_t *node);
// void doubly_Linked_List_Insert(doubly_linked_list_t *list, doubly_linked_list_node_t *node, doubly_linked_list_node_t *prevNode);
// void doubly_Linked_List_Insert_Head(doubly_linked_list_t *list, doubly_linked_list_node_t *node);
// void doubly_Linked_List_Remove(doubly_linked_list_t *list, doubly_linked_list_node_t *node);
// void doubly_List_Append_Data(doubly_linked_list_t *list, void *dataPtr);
// void doubly_Linked_List_Remove_Data(doubly_linked_list_t *list, void *dataPtr);

// #endif // !LINKED_LIST_H