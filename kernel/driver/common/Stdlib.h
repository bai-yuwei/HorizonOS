/******************************************************************************
* @file    Stdlib.h
* @brief   通用库函数相关的头文件.
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
#ifndef STD_LIB_H
#define STD_LIB_H

#include "Std_Types.h"

void memset(void* ptr, uint8 value, int num);
void memcpy(void* dest, const void* src, int num);

#endif