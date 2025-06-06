/******************************************************************************
* @file    Ordered_Array.c
* @brief   有序数组相关的文件.
* @details This is the detail description.
* @author  ywBai <yw_bai@outlook.com>
* @date    2025年05月11日 (created)
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
* Date          : 2025年05月17日;
* Revision         : 0.0.1;
* Author           : ywBai;
* Contents         :
******************************************************************************/

#include "Ordered_Array.h"

int32 standard_Compare(void *a, void *b)
{
    return *(int32 *)a - *(int32 *)b;
}

ordered_array_t ordered_Array_Create(void* array, uint32 maxSize, comparator_t comparator)
{
    ordered_array_t orderedArray;
    orderedArray.array = array;
    orderedArray.maxSize = maxSize;
    orderedArray.size = 0;
    orderedArray.comparator = comparator;
    return orderedArray;
}

ordered_array_return_t ordered_Array_Insert(ordered_array_t *orderedArray, void *element)
{
    if (orderedArray->size >= orderedArray->maxSize)
    {
        return OUT_OF_RANGE;
    }
    uint32 i = 0;
    while (i < orderedArray->size && orderedArray->comparator(orderedArray->array[i], element) <= 0)
    {
        i++;
    }
    orderedArray->size++;
    for (uint32 j = orderedArray->size - 1; j > i; j--)
    {
        orderedArray->array[j] = orderedArray->array[j - 1];
    }
    orderedArray->array[i] = element;
    return OK;
}

ordered_array_return_t ordered_Array_Remove(ordered_array_t *orderedArray, void *element)
{
    if (orderedArray->size == 0)
    {
        return OUT_OF_RANGE;
    }
    uint32 i = 0;
    while (i < orderedArray->size && orderedArray->comparator(orderedArray->array[i], element) != 0)
    {
        i++;
    }
    if (i == orderedArray->size)
    {
        return 0;
    }
    for (uint32 j = i; j < orderedArray->size - 1; j++)
    {
        orderedArray->array[j] = orderedArray->array[j + 1];
    }
    orderedArray->size--;
    return OK;
}

uint32 ordered_Array_Search(ordered_array_t *orderedArray, void *element)
{
    uint32 index = orderedArray->size;
    for (uint32 i = 0; i < orderedArray->size; i++)
    {
        if (orderedArray->comparator(orderedArray->array[i], element) == 0)
        {
            index = i;
            break;
        }
    }
    return index;
}

void* ordered_Array_Get(ordered_array_t *orderedArray, uint32 index)
{
    if (index >= orderedArray->size)
    {
        return NULL;
    }
    return orderedArray->array[index];
}

ordered_array_return_t ordered_Array_Remove_Index(ordered_array_t *orderedArray, uint32 index)
{
    if (index >= orderedArray->size)
    {
        return OUT_OF_RANGE;
    }
    for (uint32 i = index; i < orderedArray->size - 1; i++)
    {
        orderedArray->array[i] = orderedArray->array[i + 1];
    }
    orderedArray->size--;
    return OK;
}

void ordered_Array_Test()
{
    uint32 array[11] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    uint32 element = 11;
    ordered_array_t orderedArray = ordered_Array_Create(array, 11, standard_Compare);
    monitor_Printf("orderedArray.array[9] = %d\n", orderedArray.array[9]);
    ordered_Array_Insert(&orderedArray, (void*)element);
    monitor_Printf("insert over\n");
    monitor_Printf("orderedArray.array[0] = %d\n", orderedArray.array[0]);
    ordered_Array_Remove(&orderedArray, (void*)element);
    monitor_Printf("orderedArray.array[0] = %d\n", orderedArray.array[0]);
}
