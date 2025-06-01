/******************************************************************************
* @file    Math.h
* @brief   数学相关工具函数的头文件.
* @details This is the detail description.
* @author  ywBai <yw_bai@outlook.com>
* @date    2025年05月01日 (created)
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
* Date          : 2025年05月01日;
* Revision         : 0.0.1;
* Author           : ywBai;
* Contents         :
******************************************************************************/
#ifndef MATH_H
#define MATH_H

#include "Std_Types.h"

int32 div(int32 x, int32 n);
int32 mod(int32 x, int32 n);
uint32 max(uint32 a, uint32 b);
uint32 min(uint32 a, uint32 b);
#endif