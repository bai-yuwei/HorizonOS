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
#include "Cond_Var.h"

extern void cpu_Idle();
extern void context_Switch(tcb_t* prev, tcb_t* next);
extern void resume_Thread();


static bool mainThreadInReadyQueue = false;

static thread_node_t* currentThreadNode = nullptr;
static bool multiThreadEnabled = false;
static thread_node_t* mainThreadNode;
static thread_node_t* cleanThreadNode;

static doubly_linked_list_t deadThreadList;
static doubly_linked_list_t readyThreadList;
static yieldlock_t deadThreadLock;
static cond_var_t deadThreadCondVar;

static bool is_Dead_thread()
{
    return deadThreadList.size > 0;
}

static void kernel_Clean_Thread()
{
    while(1) {
        cond_Var_Wait(&deadThreadCondVar, &deadThreadLock, is_Dead_thread);
        doubly_linked_list_t deadThreadReceiver;
        doubly_Linked_List_Move(&deadThreadReceiver, &deadThreadList);
        yieldlock_Unlock(&deadThreadLock);
        if (deadThreadReceiver.size > 0)
        {
            while (deadThreadReceiver.size > 0)
            {
                thread_node_t* deadThreadNode = (thread_node_t*)deadThreadReceiver.head;
                doubly_Linked_List_Remove(&deadThreadReceiver, deadThreadNode);
                tcb_t* thread = (tcb_t*)deadThreadNode->dataPtr;
                destroy_Thread(thread);
                kfree(deadThreadNode);
            }
        }
        schedule_Thread_Yield();
    }
}

static void kernel_Init_Thread()
{

}

static void kernel_Main_Thread()
{
    tcb_t* cleanThread = thread_Init(nullptr, "cleanThread", kernel_Clean_Thread, THREAD_DEFAULT_PRIORITY, false);
    cleanThreadNode = (thread_node_t*)kmalloc(sizeof(thread_node_t), NOT_PAGE_ALIGNED);
    cleanThreadNode->dataPtr = cleanThread;
    add_Thread_Node_To_Schedule(cleanThreadNode);
    tcb_t* initThread = thread_Init(nullptr, "initThread", kernel_Init_Thread, THREAD_DEFAULT_PRIORITY, false);
    add_Thread_To_Schedule(initThread);
    multiThreadEnabled = true;
    enable_Interrupt();
    monitor_Printf("kernel main thread start!\n");
    while(1) {
        cpu_Idle();
    }
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
    oldThread->needReSchedule = false;

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

thread_node_t* get_Current_Thread_Node()
{
    return currentThreadNode;
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

void add_Thread_To_Schedule(tcb_t* thread)
{
    thread_node_t* threadNode = (thread_node_t*)kmalloc(sizeof(thread_node_t), NOT_PAGE_ALIGNED);
    threadNode->dataPtr = (void*)thread;
    add_Thread_Node_To_Schedule(threadNode);
}

void add_Thread_Node_To_Schedule(thread_node_t* threadNode)
{
    disable_Interrupt();
    tcb_t* thread = (tcb_t*)threadNode->dataPtr;
    if (thread->status != THREAD_DEAD)
    {
        thread->status = THREAD_READY;
    }
    // 添加线程到调度器的操作
    doubly_Linked_List_Append(&readyThreadList, threadNode);
    enable_Interrupt();
}

void add_Thread_To_Schedule_Head(thread_node_t* threadNode)
{
    disable_Interrupt();
    tcb_t* thread = (tcb_t*)threadNode->dataPtr;
    thread->status = THREAD_READY;
    // 添加线程到调度器的操作
    doubly_Linked_List_Append(&readyThreadList, threadNode);
    enable_Interrupt();
}

/**
 * @brief 让当前线程主动让出 CPU 使用权。
 *
 * 此函数会检查就绪线程列表是否为空，如果为空，则将主线程节点添加到就绪线程列表中，
 * 以确保系统中有可运行的线程。在操作过程中会禁用中断，防止并发问题。
 */
void schedule_Thread_Yield()
{
    // 禁用中断，避免在操作就绪线程列表时被中断干扰，保证操作的原子性
    disable_Interrupt();

    // 检查就绪线程列表的大小是否为 0，即列表中是否没有就绪线程
    if (readyThreadList.size == 0)
    {
        // 若就绪线程列表为空，将主线程节点添加到就绪线程列表的尾部
        doubly_Linked_List_Append(&readyThreadList, mainThreadNode);
        // 标记主线程已在就绪队列中
        mainThreadInReadyQueue = true;
    }

    // 操作完成后，重新启用中断，允许系统响应外部中断
    do_Context_Switch();
}

void schedule_Mark_Thread_Block()
{
    tcb_t* current = get_Current_Thread();
    current->status = THREAD_BLOCKED;
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
    tcb_t* mainThread = thread_Init(nullptr, "mainThread", kernel_Main_Thread, THREAD_DEFAULT_PRIORITY, false);
    mainThreadNode = (thread_node_t*)kmalloc(sizeof(thread_node_t), NOT_PAGE_ALIGNED);
    mainThreadNode->dataPtr = mainThread;
    // 将主线程的控制块指针存储到线程节点的数据指针中
    // mainThreadNode->dataPtr = mainThread;
    // 将当前线程节点设置为主线程节点，即当前正在运行的线程为主线程
    currentThreadNode = mainThreadNode;
    asm volatile (
        "movl %0, %%esp; \
        jmp resume_Thread": : "g" (mainThread->kernelEsp) : "memory");
}

void disable_Preempt()
{
    get_Current_Thread()->preemptCount += 1;
}

void enable_Preempt()
{
    get_Current_Thread()->preemptCount -= 1;
}

void schedule()
{
    disable_Interrupt();
    tcb_t* currentThread = get_Current_Thread();
    if (currentThread->preemptCount > 0)
    {
        return;
    }
    bool needContextSwitch = false;
    if (readyThreadList.size > 0)
    {
        needContextSwitch = currentThread->needReSchedule;
    }
    if (needContextSwitch)
    {
        do_Context_Switch();
    }
    else
    {
        enable_Interrupt();
    }
}
