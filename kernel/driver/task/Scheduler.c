/******************************************************************************
* @file    Scheduler.c
* @brief   线程调度相关的文件.
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

#include "Scheduler.h"

extern void context_Switch(tcb_t* prev, tcb_t* next);
extern void resume_Thread();


static bool mainThreadInReadyQueue = false;

static thread_node_t* currentThreadNode = nullptr;

static thread_node_t* mainThreadNode;
static thread_node_t* cleanThreadNode;

static doubly_linked_list_t deadThreadList;
static doubly_linked_list_t readyThreadList;

static void kernel_Main_Thread()
{
    monitor_Printf("kernel main thread start!\n");
    while(1) {}
}

/**
 * @brief 执行上下文切换操作。
 *
 * 此函数负责从就绪线程列表中选取下一个要执行的线程，
 * 并将当前线程状态更新后，执行上下文切换到下一个线程。
 */
static void do_Context_Switch()
{
    // 获取当前正在运行的线程控制块指针
    tcb_t* oldThread = get_Current_Thread();
    // 获取就绪线程列表的头节点
    thread_node_t* head = readyThreadList.head;
    // 从就绪线程列表的头节点中获取下一个要执行的线程控制块指针
    tcb_t* nextThread = (tcb_t*)head->dataPtr;

    // 若当前线程状态为运行中，且当前线程节点不是主线程节点
    if (oldThread->status == THREAD_RUNNING && currentThreadNode != mainThreadNode)
    {
        // 将当前线程状态更新为就绪状态
        oldThread->status = THREAD_READY;
        // 将当前线程节点重新添加到就绪线程列表的尾部
        doubly_Linked_List_Append(&readyThreadList, currentThreadNode);
    }

    // 将当前线程的时间片计数重置为 0
    oldThread->ticks = 0;
    // 标记当前线程不需要重新调度
    oldThread->needReschedule = false;

    // 将下一个要执行的线程状态更新为运行中
    nextThread->status = THREAD_RUNNING;
    // 更新当前线程节点为就绪线程列表的头节点
    currentThreadNode = head;

    // 若就绪线程列表的头节点是主线程节kernel_Main_Thread点
    if (head == mainThreadNode)
    {
        // 标记主线程不在就绪队列中
        mainThreadInReadyQueue = false;
    }

    // 更新 TSS（任务状态段）中的栈指针，使其指向新线程的内核栈顶部
    updateTssEsp(nextThread->kernelStack + KERNEL_STACK_SIZE);

    // 调用上下文切换函数，从当前线程切换到下一个线程
    context_Switch(oldThread, nextThread);
}

tcb_t* get_Current_Thread()
{
    // 获取当前线程的操作
    if (currentThreadNode == nullptr)
    {
        return nullptr;
    }
    return (tcb_t*)currentThreadNode->dataPtr;
}

void add_Dead_Thread(tcb_t* thread)
{
    // 添加死亡线程的操作
    thread->status = THREAD_DEAD;
    doubly_Linked_List_Append_Data(&deadThreadList, thread);
}


void schedule_Thread_Exit()
{
    // 线程退出时的操作
    tcb_t* current = get_Current_Thread();
    current->status = THREAD_EXITING;
    add_Dead_Thread(current);
    disable_Interrupt();
    
}

/**
 * @brief 初始化调度器。
 *
 * 此函数负责对调度器进行初始化操作，包括初始化就绪线程列表和死亡线程列表，
 * 创建主线程并将其加入调度器，最后通过汇编代码切换到主线程执行。
 */
void schedule_Init()
{
    // 初始化调度器的操作
    // 初始化就绪线程列表，为后续添加就绪线程做准备
    doubly_Linked_List_Init(&readyThreadList);
    // 初始化死亡线程列表，用于存放退出的线程
    doubly_Linked_List_Init(&deadThreadList);
    // 创建一个新的线程，作为内核主线程
    // 参数依次为：父线程指针（nullptr 表示无父线程）、线程名称、线程入口函数、线程默认优先级、是否为内核线程
    // thread_Test();
    // tcb_t* mainThread = thread_Init(nullptr, "kernel_main", kernel_Main_Thread, THREAD_DEFAULT_PRIORITY, false);
    // 为线程节点分配内存，用于存储主线程的信息
    mainThreadNode = (thread_node_t*)kmalloc(sizeof(thread_node_t), NOT_PAGE_ALIGNED);
    // 将主线程的控制块指针存储到线程节点的数据指针中
    // mainThreadNode->dataPtr = mainThread;
    // 将当前线程节点设置为主线程节点，即当前正在运行的线程为主线程
    currentThreadNode = mainThreadNode;
    // asm volatile (
    //     "movl %0, %%esp; \
    //     jmp resume_Thread": : "g" (mainThread->kernelEsp) : "memory");
}
