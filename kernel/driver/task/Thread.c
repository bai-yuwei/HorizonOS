/******************************************************************************
* @file    Thread.c
* @brief   线程相关的文件.
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
#include "Thread.h"

static void kernel_Thread(threadFunc* function) {
    function();
    // schedule_thread_exit();
}


tcb_t* thread_Init(tcb_t* thread, char* name, void* function, uint32 priority, uint8 user)
{
    if (thread == nullptr)
    {
        thread = (tcb_t*)kmalloc(sizeof(tcb_t), NOT_PAGE_ALIGNED);
        memset(thread, 0, sizeof(tcb_t));
    }
    uint32 pid = 0;
    thread->pid = pid;
    if (name != nullptr)
    {
        strcpy(thread->name, name);
    }
    else
    {
        char buf[32];
        sprintf(buf, "Thread_%u", thread->pid);
        strcpy(thread->name, buf);
    }
    thread->status = THREAD_READY;
    thread->ticks = 0;
    thread->priority = priority;
    thread->userStackIndex = -1;
    uint32 kernelStack = (uint32)kmalloc(KERNEL_STACK_SIZE, PAGE_ALIGNED);
    for (int32 i = 0; i < KERNEL_STACK_SIZE / PAGE_SIZE; i++)
    {
        map_Page(kernelStack + i * PAGE_SIZE, -1);
    }
    memset((void*)kernelStack, 0, KERNEL_STACK_SIZE);
    thread->kernelStack = kernelStack;
    thread->kernelEsp = kernelStack + KERNEL_STACK_SIZE - sizeof(switch_stack_t) - sizeof(interrupt_stack_t);
    switch_stack_t* switchStack = (switch_stack_t*)(thread->kernelEsp);
    switchStack->edi = 0;
    switchStack->esi = 0;
    switchStack->ebp = 0;
    switchStack->ebx = 0;
    switchStack->edx = 0;
    switchStack->ecx = 0;
    switchStack->eax = 0;
    switchStack->startEip = (uint32)kernel_Thread;
    switchStack->function = function;
}

