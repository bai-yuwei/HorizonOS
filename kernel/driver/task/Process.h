/******************************************************************************
* @file    Process.h
* @brief   进程相关的头文件.
* @details This is the detail description.
* @author  ywBai <yw_bai@outlook.com>
* @date    2025年06月01日 (created)
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
* Date          : 2025年06月01日;
* Revision         : 0.0.1;
* Author           : ywBai;
* Contents         :
******************************************************************************/
#ifndef PROCESS_H
#define PROCESS_H
#include "Std_Types.h"

enum process_status {
    PROCESS_NORMAL,
    PROCESS_EXIT,
    PROCESS_EXIT_ZOMBIE
};

struct process_struct
{
    uint32 pid;
    char name[32];
    struct process_struct* parent;
    enum process_status status;
};

#endif //!PROCESS_H