#ifndef YIELDLOCK_H
#define YIELDLOCK_H

#include "Std_Types.h"
#include "Interrupt.h"
#include "Thread.h"
#include "Scheduler.h"
#include "Debug.h"

#define LOCKED  1
#define UNLOCKED 0

#define SINGLE_CORE

typedef struct yieldlock
{
    volatile uint32 lock;
} yieldlock_t;

void yieldlock_Init(yieldlock_t* lock);
void yieldlock_Lock(yieldlock_t* lock);
bool yieldlock_TryLock(yieldlock_t* lock);
void yieldlock_Unlock(yieldlock_t* lock);


#endif // !YIELDLOCK_H
