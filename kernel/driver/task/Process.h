#ifndef PROCESS_H
#define PROCESS_H
#include "Std_Types.h"
#include "Thread.h"
#include "Page_Table.h"
#include "Yieldlock.h"

#define USER_STACK_TOP 0xBFC00000
#define USER_STACK_SIZE 65536
#define USER_PROCESS_THREAD_MAX 4096

enum process_status {
    PROCESS_NORMAL,
    PROCESS_EXIT,
    PROCESS_EXIT_ZOMBIE
};

struct process_struct
{
    uint32 pid;
    char name[32];
    struct process_struct* parent;
    enum process_status status;
};

#endif //!PROCESS_H