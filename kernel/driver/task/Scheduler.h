/******************************************************************************
* @file    Scheduler.h
* @brief   线程调度相关的头文件.
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
* Revision         : 0.0.2;
* Author           : ywBai;
* Contents         :
******************************************************************************/

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "Thread.h"
#include "Gdt.h"


tcb_t* get_Current_Thread();
thread_node_t* get_Current_Thread_Node();
void add_Dead_Thread(tcb_t* thread);
void schedule_Thread_Exit();
void schedule_Init();
void schedule_Mark_Thread_Block();
void schedule_Thread_Yield();
void add_Thread_To_Schedule_Head(thread_node_t* threadNode);
void add_Thread_Node_To_Schedule(thread_node_t* threadNode);
void add_Thread_To_Schedule(tcb_t* thread);
void schedule_Thread_Exit();
void add_Dead_Thread(tcb_t* thread);
void disable_Preempt();
void enable_Preempt();
void schedule();
#endif // !SCHEDULER_H
