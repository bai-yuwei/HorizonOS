/**
 * @brief 获取自旋锁，防止多核心环境下的并发访问问题。
 * 
 * 此函数会先禁止内核抢占，若系统为多核环境，会通过原子操作尝试获取自旋锁，
 * 若锁已被占用则会进入忙等待状态，直到成功获取锁。
 * 
 * @param lock 指向自旋锁对象的指针
 */
void spinlock_Lock(spinlock_t* lock)
{
    // 禁止内核抢占，确保当前任务在执行期间不会被其他任务打断
    disable_Preempt();

    // 如果不是单核心系统，需要处理多核环境下的锁竞争问题
    #ifndef SINGLE_CORE
    // 原子操作：将锁的状态设置为 LOCKED，并获取其旧值
    // 若旧值不等于 UNLOCKED，说明锁已被其他核心持有，进入忙等待循环
    while (atomic_Exchange(&lock->lock, LOCKED) != UNLOCKED) {}
    #endif // SINGLE_CORE
}
