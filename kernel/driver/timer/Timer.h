/******************************************************************************
* @file    Timer.h
* @brief   时钟中断处理相关的头文件.
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
* Date          : 2025年05月11日;
* Revision         : 0.0.1;
* Author           : ywBai;
* Contents         :
******************************************************************************/
#ifndef TIMER_H
#define TIMER_H

#include "Interrupt.h"
#include "Monitor.h"
#include "Thread.h"
#include "Scheduler.h"

#define TIMER_FREQUENCY 50

void timer_Init(uint32 frequency);

#endif // !TIMER_H
