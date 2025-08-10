#ifndef COND_VAR_H
#define COND_VAR_H

#include "Std_Types.h"
#include "Yieldlock.h"
#include "Linked_List.h"

struct cond_var
{
    doubly_linked_list_t waitingThreadQueue;
};
typedef struct cond_var cond_var_t;
typedef bool (*cv_predicator_func)();

void cond_Var_Init(cond_var_t* condVar);
void cond_Var_Wait(cond_var_t* condVar, yieldlock_t* lock, cv_predicator_func predicator);
void cond_Var_Notify(cond_var_t* condVar);

#endif // !COND_VAR_H
#define COND_VAR_H