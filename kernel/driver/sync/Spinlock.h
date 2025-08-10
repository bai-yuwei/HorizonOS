#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "Std_Types.h"
#include "Interrupt.h"
#include "Thread.h"
#include "Scheduler.h"
#include "Debug.h"

#define LOCKED 1
#define UNLOCKED 0

#define SINGLE_CORE

typedef struct spinlock
{
    volatile uint32 lock;
    volatile uint32 interruptMask;
} spinlock_t;

void spinlock_Init(spinlock_t* lock);
void spinlock_Lock(spinlock_t* lock);
void spinlock_Unlock(spinlock_t* lock);
void spinlock_Lock_Irq_Save(spinlock_t* lock);
void spinlock_Unlock_Irq_Restore(spinlock_t* lock);

#endif // !SPINLOCK_H
