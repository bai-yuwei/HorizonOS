/******************************************************************************
* @file    Debug.c
* @brief   调试相关的文件.
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

#include "Debug.h"

void oh_Panic(const char* file, const char* func, int line)
{
    disable_Interrupt();
    monitor_Print_With_Color("PANIC!", COLOR_LIGHT_RED);
    monitor_Printf(" in %s:%s():%d\n", file, func, line);
    while(1);
}
