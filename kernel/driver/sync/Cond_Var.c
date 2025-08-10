#include "Cond_Var.h"

extern uint32 atomic_Exchange(volatile uint32* addr, uint32 val);

void cond_Var_Init(cond_var_t* condVar)
{
    doubly_Linked_List_Init(&condVar->waitingThreadQueue);
}

/**
 * @brief 让当前线程等待条件变量满足特定条件。
 * 
 * 此函数会持续检查谓词函数的返回值，若条件不满足，则将当前线程加入等待队列，
 * 阻塞当前线程并释放锁，等待调度器重新调度。
 * 
 * @param condVar 指向条件变量对象的指针。
 * @param lock 指向 yieldlock 锁对象的指针，用于同步操作。
 * @param predicator 指向谓词函数的指针，用于判断条件是否满足。
 */
void cond_Var_Wait(cond_var_t* condVar, yieldlock_t* lock, cv_predicator_func predicator)
{
    // 获取 yieldlock 锁，确保操作的原子性
    yieldlock_Lock(lock);

    // 循环检查谓词函数的返回值，若谓词函数存在且返回 false，则继续循环
    while (predicator != nullptr && predicator() == false)
    {
        // 获取当前线程的线程节点
        thread_node_t* threadNode = get_Current_Thread_Node();
        // 将当前线程节点添加到条件变量的等待线程队列中
        doubly_Linked_List_Append(&condVar->waitingThreadQueue, threadNode);
        // 标记当前线程为阻塞状态，等待条件满足
        schedule_Mark_Thread_Block();
        // 释放 yieldlock 锁，允许其他线程访问共享资源
        yieldlock_Unlock(lock);
        // 主动让出 CPU 时间片，让调度器调度其他线程运行
        schedule_Thread_Yield();
        // 重新获取 yieldlock 锁，继续检查条件
        yieldlock_Lock(lock);
    }

    // 条件满足，释放 yieldlock 锁
    yieldlock_Unlock(lock);
}

/**
 * @brief 通知一个等待在条件变量上的线程。
 * 
 * 该函数会检查条件变量的等待队列中是否有等待的线程，
 * 若有，则从队列中移除队首线程节点，并将其添加到调度队列中，
 * 让调度器重新调度该线程运行。
 * 
 * @param condVar 指向条件变量对象的指针。
 */
void cond_Var_Notify(cond_var_t* condVar)
{
    // 检查条件变量的等待队列中是否有等待的线程
    if (condVar->waitingThreadQueue.size != 0)
    {
        // 获取等待队列的队首线程节点
        thread_node_t* head = condVar->waitingThreadQueue.head;
        // 从条件变量的等待队列中移除队首线程节点
        doubly_Linked_List_Remove(&condVar->waitingThreadQueue, head);
        // 将移除的线程节点添加到调度队列中，等待调度器调度
        add_Thread_Node_To_Schedule(head);
    }
}