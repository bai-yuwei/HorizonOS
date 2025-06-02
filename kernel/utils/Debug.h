/******************************************************************************
* @file    Debug.h
* @brief   调试相关的头文件.
* @details This is the detail description.
* @author  ywBai <yw_bai@outlook.com>
* @date    2025年06月02日 (created)
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
* Date          : 2025年06月02日;
* Revision         : 0.0.1;
* Author           : ywBai;
* Contents         :
******************************************************************************/

#ifndef DEBUG_H
#define DEBUG_H

#include "Monitor.h"
#include "Interrupt.h"


#define PANIC() oh_Panic(__FILE__, __FUNCTION__, __LINE__)

#ifndef DEBUG
#define ASSERT(CONDITION) ((void)0)
#else
#define ASSERT(CONDITION) if (CONDITION) {} else {PANIC();}
#endif


void oh_Panic(const char* file, const char* func, int line);

#endif // DEBUG_H