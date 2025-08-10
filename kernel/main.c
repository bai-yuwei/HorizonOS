/******************************************************************************
* @file    main.c
* @brief   kernel main.
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
#include "Std_Types.h"
#include "Monitor.h"
#include "Gdt.h"
#include "Interrupt.h"
#include "Timer.h"
#include "Kheap.h"
#include "Page_Table.h"
#include "Linked_List.h"
#include "Scheduler.h"

char* helloWorld = "Hello World!\n";
static void system_Init()
{
    monitor_Init();
    monitor_Clear();
    monitor_Printf(helloWorld);
    gdt_Init();
    idt_Init();
    page_Table_Init();
    kheap_Init();
    timer_Init(TIMER_FREQUENCY);
    schedule_Init();
} 

int main(void)
{
    system_Init();
    // thread_Test();
    // ordered_Array_Test();
    // page_Table_Test();
    // kheap_Test();
    // doubly_Linked_Test();
    // while(1);
    return 0;
}