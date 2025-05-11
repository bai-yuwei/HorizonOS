/******************************************************************************
* @file    Std_Types.h
* @brief   kernel格式相关的定义.
* @details This is the detail description.
* @author  ywBai <yw_bai@outlook.com>
* @date    2025年04月27日 (created)
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
* Date          : 2025年04月27日;
* Revision         : 0.0.1;
* Author           : ywBai;
* Contents         :
******************************************************************************/
#ifndef STD_TYPES_H
#define STD_TYPES_H

typedef unsigned long long  uint64;
typedef long long           int64;
typedef unsigned int        uint32;
typedef int                 int32;
typedef unsigned short      uint16;
typedef short               int16;
typedef unsigned char       uint8;
typedef char                int8;
typedef uint8               bool;

#define true 1
#define false 0
#define NULL 0
#define NULL_PTR ((void*)0)

#endif