// /******************************************************************************
// * @file    Hash_Table.c
// * @brief   哈希表相关的文件.
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

// #include "Hash_Table.h"

// void hash_Table_Init(hash_table_t *table)
// {
//     table->size = 0;
//     table->bucketsNum = INITIAL_BUCKETS_NUM;
//     table->buckets = (doubly_linked_list_t *)kmalloc(table->bucketsNum * sizeof(doubly_linked_list_t), NOT_PAGE_ALIGNED);
//     for (int32 i = 0; i < INITIAL_BUCKETS_NUM; i++)
//     {
//         doubly_Linked_List_Init(&table->buckets[i]);
//     }
// }

// hash_table_t create_Hash_Table(void)
// {
//     hash_table_t table;
//     hash_Table_Init(&table);
//     return table;
// }

// void hash_Table_Clear(hash_table_t *table)
// {
//     for (int32 i = 0; i < table->bucketsNum; i++)
//     {
//         doubly_linked_list_t bucket = table->buckets[i];
//         doubly_linked_list_node_t *node = bucket.head;
//         while (node != NULL)
//         {
//             doubly_linked_list_node_t *cur = node;
//             node = node->next;
//             hash_table_pair_t *pair = (hash_table_pair_t *)cur->dataPtr;
//             kfree(pair->key);
//             kfree(pair->value);
//             kfree(pair);
//             doubly_Linked_List_Remove(&bucket, cur);
//             kfree(cur);
//         }
//     }
//     table->size = 0;
// }

// void hash_Table_Destroy(hash_table_t *table)
// {
//     hash_Table_Clear(table);
//     kfree(table->buckets);
//     table->size = 0;
// }

