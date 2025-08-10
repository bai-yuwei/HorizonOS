#include "Spinlock.h"

extern uint32 atomic_Exchange(volatile uint32* addr, uint32 val);
extern uint32 get_Eflags();

void spinlock_Init(spinlock_t* lock)
{
    lock->lock = UNLOCKED;
    lock->interruptMask = 0;
}

/**
 * @brief 获取自旋锁，用于在多核心环境下保护临界区资源。
 * 
 * 该函数首先禁止内核抢占，确保当前任务在执行期间不会被其他任务打断。
 * 若系统为多核环境，会通过原子操作尝试获取自旋锁，若锁已被占用则进入忙等待状态，
 * 直到成功获取锁。
 * 
 * @param lock 指向自旋锁对象的指针。
 */
void spinlock_Lock(spinlock_t* lock)
{
    // 禁止内核抢占，防止当前任务在执行过程中被其他任务抢占，保证操作的原子性
    disable_Preempt();

    // 若系统不是单核心环境，需要处理多核间的锁竞争问题
    #ifndef SINGLE_CORE
    // 原子操作：将锁的状态设置为 LOCKED，并返回锁的旧状态
    // 若旧状态不等于 UNLOCKED，说明锁已被其他核心持有，当前核心进入忙等待循环
    // 持续尝试获取锁，直到成功获取（即旧状态为 UNLOCKED）
    while (atomic_Exchange(&lock->lock, LOCKED) != UNLOCKED) {}
    #endif // SINGLE_CORE
}

/**
 * @brief 获取自旋锁，并保存中断状态，同时禁用中断。
 * 
 * 此函数会先禁止内核抢占，保存当前的中断标志位，然后禁用中断，
 * 记录中断之前的状态，最后在多核环境下尝试获取自旋锁。
 * 
 * @param lock 指向自旋锁对象的指针
 */
void spinlock_Lock_Irq_Save(spinlock_t* lock)
{
    // 禁止内核抢占，防止当前任务在执行过程中被其他任务打断
    disable_Preempt();
    // 获取当前的 EFLAGS 寄存器值，其中包含中断标志位
    uint32 eflags = get_Eflags();
    // 禁用中断，避免在持有锁期间被中断处理程序打断
    disable_Interrupt();
    // 提取 EFLAGS 寄存器中的中断标志位（第 9 位），并保存到锁的 interruptMask 字段中
    lock->interruptMask = (eflags & (1 << 9));
    // 如果不是单核心系统，需要处理多核环境下的锁竞争问题
    #ifndef SINGLE_CORE
    // 原子操作：将锁的状态设置为 LOCKED，并获取其旧值
    // 若旧值不等于 UNLOCKED，说明锁已被其他核心持有，进入忙等待循环
    while (atomic_Exchange(&lock->lock, LOCKED) != UNLOCKED) {}
    #endif
}

void spinlock_Unlock(spinlock_t* lock)
{
    #ifndef SINGLE_CORE
    lock->lock = UNLOCKED;
    #endif // SINGLE_CORE
    enable_Preempt();
}

void spinlock_Unlock_Irq_Restore(spinlock_t* lock)
{
    #ifndef SINGLE_CORE
    lock->lock = UNLOCKED;
    #endif // SINGLE_CORE
    enable_Preempt();
    if (lock->interruptMask)
    {
        enable_Interrupt();
    }
}
