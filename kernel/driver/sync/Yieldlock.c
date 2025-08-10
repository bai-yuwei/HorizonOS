#include "Yieldlock.h"

extern uint32 atomic_Exchange(volatile uint32* addr, uint32 val);

void yieldlock_Init(yieldlock_t* lock)
{
    lock->lock = UNLOCKED;
}

void yieldlock_Lock(yieldlock_t* lock)
{
    while (atomic_Exchange(&lock->lock, LOCKED) != UNLOCKED)
    {
        schedule_Thread_Yield();
    }
}

bool yieldlock_TryLock(yieldlock_t* lock)
{
    return atomic_Exchange(&lock->lock, LOCKED) == UNLOCKED;
}

void yieldlock_Unlock(yieldlock_t* lock)
{
    lock->lock = UNLOCKED;
}