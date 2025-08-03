/******************************************************************************
* @file    Thread.h
* @brief   线程相关的头文件.
* @details This is the detail description.
* @author  ywBai <yw_bai@outlook.com>
* @date    2025年06月01日 (created)
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
* Date          : 2025年06月01日;
* Revision         : 0.0.1;
* Author           : ywBai;
* Contents         :
******************************************************************************/

#ifndef THREAD_H
#define THREAD_H

#include "Std_Types.h"
#include "Kheap.h"
#include "Stdlib.h"
#include "Interrupt.h"
#include "Linked_List.h"

#define THREAD_DEFAULT_PRIORITY  10

#define KERNEL_STACK_SIZE  8192

typedef void threadFunc();

typedef isr_params_t interrupt_stack_t;
typedef doubly_linked_list_node_t thread_node_t;

enum thread_status {
    THREAD_RUNNING,
    THREAD_READY,
    THREAD_BLOCKED,
    THREAD_WAITING,
    THREAD_HANGING,
    THREAD_EXITING,
    THREAD_DEAD
};

struct thread_struct
{
    // 内核栈指针（Kernel Stack Pointer），指向当前线程在内核模式下的栈顶位置。
    // 在进行内核模式操作（如系统调用、中断处理）时，会使用这个栈指针来管理栈帧。
    uint32 kernelEsp;

    // 内核栈的基地址，标识线程内核栈的起始位置。
    // 结合 kernelEsp 可以确定内核栈的使用范围。
    uint32 kernelStack;

    // 进程标识符（Process ID），每个线程都属于某个进程，这个值用于唯一标识该线程所属的进程。
    uint32 pid;

    // 线程的名称，最多可存储 31 个字符和一个字符串结束符 '\0'。
    // 方便在调试或日志记录时识别不同的线程。
    char name[32];

    // 线程的优先级，范围是 0 - 255。
    // 操作系统在进行任务调度时，会参考这个优先级来决定线程的执行顺序。
    uint8 priority;

    // 线程的状态，使用前面定义的 task_status 枚举类型。
    // 可能的状态包括运行中、就绪、阻塞、等待、挂起和死亡等。
    enum thread_status status;

    // 线程的时间片计数，代表线程在 CPU 上可以执行的时间片数量。
    // 当这个值减为 0 时，操作系统可能会进行任务调度，切换到其他线程执行。
    uint32 ticks;

    // 用户栈的基地址，指向线程在用户模式下的栈起始位置。
    // 用户模式下的函数调用、局部变量等会使用这个栈。
    uint32 userStack;

    // 用户栈的索引，可能用于标识用户栈中特定位置或管理用户栈的使用情况。
    // 负值可能表示特殊状态，比如栈为空等。
    int32 userStackIndex;

    // 一个布尔值，指示该线程是否需要重新调度。
    // 当这个值为 true 时，操作系统会在合适的时机进行任务调度，切换到其他线程。
    bool needReschedule;

    // 抢占计数器，用于控制内核抢占。
    // 当该值大于 0 时，内核抢占被禁止，线程不会被其他线程抢占执行。
    uint32 preemptCount;
};
typedef struct thread_struct tcb_t;

struct switch_stack
{
    // 扩展目的变址寄存器，常用于串操作指令，保存目的操作数的地址。
    // 在上下文切换时，保存该寄存器的值，以便恢复现场后能继续之前的操作。
    uint32 edi;

    // 扩展源变址寄存器，同样常用于串操作指令，保存源操作数的地址。
    // 保存此寄存器的值可确保上下文恢复后串操作能正确继续。
    uint32 esi;

    // 扩展基址指针寄存器，指向当前栈帧的底部。
    // 用于访问函数的局部变量和参数，保存它能在上下文切换后恢复栈帧。
    uint32 ebp;

    // 扩展基址寄存器，可作为通用寄存器使用，也常用于间接寻址。
    // 保存该寄存器的值能保证上下文恢复后相关操作的正确性。
    uint32 ebx;

    // 扩展数据寄存器，可作为通用寄存器，在乘除运算等操作中使用。
    // 保存该寄存器的值能确保恢复上下文后运算能继续进行。
    uint32 edx;

    // 扩展计数寄存器，可作为通用寄存器，在循环等操作中作为计数器。
    // 保存该寄存器的值能保证循环等操作在上下文恢复后正常执行。
    uint32 ecx;

    // 扩展累加寄存器，是最常用的通用寄存器，用于算术运算、函数返回值等。
    // 保存该寄存器的值能确保上下文恢复后相关操作正常进行。
    uint32 eax;

    // 扩展指令指针寄存器，指向下一条要执行的指令的地址。
    // 保存此寄存器的值可在上下文恢复后继续执行未完成的指令序列。
    uint32 eip;

    // 未使用的返回地址指针，可能是为了兼容某种调用约定或预留扩展。
    // 通常在函数调用时，返回地址会被压入栈中，这里可能是预留的相关位置。
    void (*unusedReturnAddr);

    // 函数指针，指向要执行的函数。
    // 可能用于在上下文切换后指定新的执行函数。
    threadFunc* function;
};
typedef struct switch_stack switch_stack_t;


tcb_t* thread_Init(tcb_t* thread, char* name, void* function, uint32 priority, uint8 user);

void thread_Test(void);
#endif // !THR