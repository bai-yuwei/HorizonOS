/******************************************************************************
* @file    Timer.c
* @brief   时钟中断处理相关的文件.
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

#include "Timer.h"

static uint32 tick = 0;

static void timer_Handler(isr_params_t params)
{
    tcb_t* currentThread = get_Current_Thread();
    currentThread->ticks++;
    if (currentThread->ticks >= currentThread->priority)
    {
        currentThread->needReSchedule = true;
    }
    // monitor_Printf("tick = %d\n", tick++);
}

/**
 * @brief 初始化可编程间隔定时器（PIT），设置定时器中断频率。
 * 
 * 此函数通过计算除数并向 PIT 的控制寄存器和数据端口写入相应的值，
 * 来设置定时器的中断频率，同时注册定时器中断的回调函数。
 * 
 * @param frequency 期望的定时器中断频率，单位为赫兹（Hz）。
 */
void timer_Init(uint32 frequency)
{
    // 计算 PIT 的除数，1193180 是 PIT 的时钟频率（Hz）
    // 除数 = 时钟频率 / 期望的中断频率
    uint32 divisor = 1193180 / frequency;
    
    // 提取除数的低 8 位
    uint8 low = (uint8)(divisor & 0xFF);
    
    // 提取除数的高 8 位
    uint8 high = (uint8)((divisor >> 8) & 0xFF);
    
    // 注册定时器中断（IRQ0）的处理回调函数
    // IRQ0_INT_NUM 是定时器中断对应的中断号
    // timer_Handler 是中断发生时要调用的回调函数
    register_Interrupt_Handler(IRQ0_INT_NUM, &timer_Handler);
    
    // 向 PIT 的控制寄存器（端口 0x43）写入控制字 0x36
    // 0x36 表示选择计数器 0，先读写低字节再读写高字节，模式 3（方波发生器），二进制计数
    io_Out_Byte(0x43, 0x36);
    
    // 向 PIT 的计数器 0 数据端口（端口 0x40）写入除数的低 8 位
    io_Out_Byte(0x40, low);
    
    // 向 PIT 的计数器 0 数据端口（端口 0x40）写入除数的高 8 位
    io_Out_Byte(0x40, high);
}