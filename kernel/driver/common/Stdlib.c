/******************************************************************************
* @file    Stdlib.c
* @brief   通用库函数相关的文件.
* @details This is the detail description.
* @author  ywBai <yw_bai@outlook.com>
* @date    2025年05月06日 (created)
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
* Date          : 2025年05月06日;
* Revision         : 0.0.1;
* Author           : ywBai;
* Contents         :
******************************************************************************/

#include "Stdlib.h"

void memset(void* ptr, uint8 value, int num)
{
    for (int i = 0; i < num; i++)
    {
        *((uint8*)(ptr + i)) = value;
    }
}

void memcpy(void* dest, const void* src, int num)
{
    for (int i = 0; i < num; i++)
    {
        *((uint8*)(dest + i)) = *((uint8*)(src + i));
    }
}