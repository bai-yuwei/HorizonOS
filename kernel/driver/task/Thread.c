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
#include "Scheduler.h"
extern void resume_Thread();

static void kernel_Thread(threadFunc* function) {
    function();
    schedule_Thread_Exit();
}


/**
 * @brief 初始化一个线程控制块 (TCB)。
 *
 * 该函数用于初始化一个线程控制块，若传入的线程指针为空，则会分配新的内存。
 * 函数会设置线程的 PID、名称、状态、优先级等信息，并为线程分配内核栈。
 *
 * @param thread 指向线程控制块的指针，若为 nullptr 则分配新的内存。
 * @param name 线程的名称，若为 nullptr 则自动生成名称。
 * @param function 线程要执行的函数指针。
 * @param priority 线程的优先级。
 * @param user 用户标志（具体含义取决于实现）。
 * @return tcb_t* 初始化后的线程控制块指针。
 */
tcb_t* thread_Init(tcb_t* thread, char* name, void* function, uint32 priority, uint8 user)
{
    // 若传入的线程指针为空，则分配新的线程控制块内存并初始化为 0
    if (thread == nullptr) {
    // Allocate one page as tcb_t and kernel stack for each thread.
    thread = (tcb_t*)kmalloc(sizeof(tcb_t), NOT_PAGE_ALIGNED);
    memset(thread, 0, sizeof(tcb_t));
    monitor_Printf("thread_Init: thread = %x\n", thread);
    }
    // 初始化线程的 PID 为 1
    uint32 pid = 1;
    thread->pid = pid;
    // 若传入的线程名称不为空，则复制该名称到线程控制块中
    if (name != nullptr)
    {
        strcpy(thread->name, name);
    }
    // 若传入的线程名称为空，则自动生成一个线程名称
    else
    {
        char buf[32];
        sprintf(buf, "Thread_%u", thread->pid);
        strcpy(thread->name, buf);
    }
    // 设置线程状态为就绪状态
    thread->status = THREAD_READY;
    // 初始化线程的时间片计数为 0
    thread->ticks = 0;
    // 设置线程的优先级
    thread->priority = priority;
    // 初始化用户栈索引为 -1
    thread->userStackIndex = -1;
    // 为线程分配内核栈内存，并按页对齐
    uint32 kernelStack = (uint32)kmalloc(KERNEL_STACK_SIZE, PAGE_ALIGNED);
    monitor_Printf("kernelStack: kernelStack = %x\n", kernelStack);
    // 映射内核栈的每一页内存
    for (int32 i = 0; i < KERNEL_STACK_SIZE / PAGE_SIZE; i++)
    {
        map_Page(kernelStack + i * PAGE_SIZE, -1);
    }
    // 将分配的内核栈内存初始化为 0
    memset((void*)kernelStack, 0, KERNEL_STACK_SIZE);
    // 记录线程的内核栈地址
    thread->kernelStack = kernelStack;
    // 计算线程的内核栈指针，预留切换栈和中断栈的空间
    thread->kernelEsp = kernelStack + KERNEL_STACK_SIZE - (sizeof(interrupt_stack_t)) + (sizeof(switch_stack_t));
    // 获取切换栈的指针
    switch_stack_t* switchStack = (switch_stack_t*)(thread->kernelEsp);
    // 初始化切换栈的寄存器值为 0
    switchStack->edi = 0;
    switchStack->esi = 0;
    switchStack->ebp = 0;
    switchStack->ebx = 0;
    switchStack->edx = 0;
    switchStack->ecx = 0;
    switchStack->eax = 0;
    // 设置切换栈的起始执行地址为 kernel_Thread 函数
    switchStack->eip = (uint32)kernel_Thread;
    // 设置切换栈要执行的函数指针
    switchStack->function = function;
    return thread; // 补充返回值，原代码缺少返回语句
}

static void test_Thread()
{
    monitor_Printf("test_Thread!\n");
    while(1) {}
}

void thread_Test(void)
{
    tcb_t* testThread = thread_Init(nullptr, "testThread", test_Thread, THREAD_DEFAULT_PRIORITY, false);
    asm volatile (
        "movl %0, %%esp; \
        jmp resume_Thread": : "g" (testThread->kernelEsp) : "memory");
}

void destroy_Thread(tcb_t* thread)
{
    kfree((void*)thread->kernelStack);
    kfree(thread);
}