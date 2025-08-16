/******************************************************************************
* @file    Thread.c
* @brief   线程相关的文件.
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
#include "Thread.h"
#include "Scheduler.h"
extern void resume_Thread();
extern void switch_To_User_Mode();

static void kernel_Thread(threadFunc* function) {
    function();
    schedule_Thread_Exit();
}


/**
 * @brief 初始化一个线程控制块 (TCB)。
 *
 * 该函数用于初始化一个线程控制块，若传入的线程指针为空，则会分配新的内存。
 * 函数会设置线程的 PID、名称、状态、优先级等信息，并为线程分配内核栈。
 *
 * @param thread 指向线程控制块的指针，若为 nullptr 则分配新的内存。
 * @param name 线程的名称，若为 nullptr 则自动生成名称。
 * @param function 线程要执行的函数指针。
 * @param priority 线程的优先级。
 * @param user 用户标志（具体含义取决于实现）。
 * @return tcb_t* 初始化后的线程控制块指针。
 */
tcb_t* thread_Init(tcb_t* thread, char* name, void* function, uint32 priority, uint8 user)
{
    // 若传入的线程指针为空，则分配新的线程控制块内存并初始化为 0
    if (thread == nullptr) {
        // Allocate one page as tcb_t and kernel stack for each thread.
        // 为线程控制块分配内存，不按页对齐
        thread = (tcb_t*)kmalloc(sizeof(tcb_t), NOT_PAGE_ALIGNED);
        // 将分配的内存初始化为 0
        memset(thread, 0, sizeof(tcb_t));
        // 打印线程控制块的地址
        monitor_Printf("thread_Init: thread = %x\n", thread);
    }
    // 初始化线程的 PID 为 1
    uint32 pid = 1;
    // 设置线程的 PID
    thread->pid = pid;
    // 若传入的线程名称不为空，则复制该名称到线程控制块中
    if (name != nullptr)
    {
        // 将传入的线程名称复制到线程控制块中
        strcpy(thread->name, name);
    }
    // 若传入的线程名称为空，则自动生成一个线程名称
    else
    {
        // 用于存储自动生成的线程名称
        char buf[32];
        // 生成线程名称，格式为 "Thread_<PID>"
        sprintf(buf, "Thread_%u", thread->pid);
        // 将生成的线程名称复制到线程控制块中
        strcpy(thread->name, buf);
    }
    // 设置线程状态为就绪状态
    thread->status = THREAD_READY;
    // 初始化线程的时间片计数为 0
    thread->ticks = 0;
    // 设置线程的优先级
    thread->priority = priority;
    // 初始化用户栈索引为 -1
    thread->userStackIndex = -1;
    // 为线程分配内核栈内存，并按页对齐
    uint32 kernelStack = (uint32)kmalloc(KERNEL_STACK_SIZE, PAGE_ALIGNED);
    // 打印内核栈的地址
    monitor_Printf("kernelStack: kernelStack = %x\n", kernelStack);
    // 映射内核栈的每一页内存
    for (int32 i = 0; i < KERNEL_STACK_SIZE / PAGE_SIZE; i++)
    {
        // 映射内核栈的每一页到物理内存
        map_Page(kernelStack + i * PAGE_SIZE, -1);
    }
    // 将分配的内核栈内存初始化为 0
    memset((void*)kernelStack, 0, KERNEL_STACK_SIZE);
    // 记录线程的内核栈地址
    thread->kernelStack = kernelStack;
    // 计算线程的内核栈指针，预留切换栈和中断栈的空间
    thread->kernelEsp = kernelStack + KERNEL_STACK_SIZE - (sizeof(interrupt_stack_t)) + (sizeof(switch_stack_t));
    // 获取切换栈的指针
    switch_stack_t* switchStack = (switch_stack_t*)(thread->kernelEsp);
    // 初始化切换栈的寄存器值为 0
    switchStack->edi = 0;
    switchStack->esi = 0;
    switchStack->ebp = 0;
    switchStack->ebx = 0;
    switchStack->edx = 0;
    switchStack->ecx = 0;
    switchStack->eax = 0;
    // 设置切换栈的起始执行地址为 kernel_Thread 函数
    switchStack->eip = (uint32)kernel_Thread;
    // 设置切换栈要执行的函数指针
    switchStack->function = function;
    // 若用户标志为真，则进行用户模式切换相关设置
    if (user)
    {
        // 设置切换栈的起始执行地址为 switch_To_User_Mode 函数
        switchStack->eip = (uint32)switch_To_User_Mode;
        // 获取中断栈的指针
        interrupt_stack_t* interruptStack = (interrupt_stack_t*)((uint32)(thread->kernelEsp) + sizeof(switch_stack_t));
        // 设置中断栈的数据段选择子
        interruptStack->ds = SELECTOR_USER_DATA;
        // 再次初始化切换栈的寄存器值为 0
        switchStack->edi = 0;
        switchStack->esi = 0;
        switchStack->ebp = 0;
        switchStack->ebx = 0;
        switchStack->edx = 0;
        switchStack->ecx = 0;
        switchStack->eax = 0;
        // 设置中断栈的指令指针为线程要执行的函数地址
        interruptStack->eip = (uint32)function;
        // 设置中断栈的代码段选择子
        interruptStack->cs = SELECTOR_USER_CODE;
        // 设置中断栈的标志寄存器
        interruptStack->eflags = EFLAGS_IOPL_0 | EFLAGS_MBS | EFLAGS_IF_1;
        // 设置中断栈的栈段选择子
        interruptStack->ss = SELECTOR_USER_DATA;
    }
    // 返回初始化后的线程控制块指针
    return thread;
}

/**
 * @brief 准备用户栈，为用户模式下的线程设置参数和栈布局。
 *
 * 该函数会计算命令行参数的总长度，将参数复制到用户栈，
 * 并在栈上设置参数指针数组、参数数量和返回地址，
 * 最后更新中断栈中的用户栈指针。
 *
 * @param thread 指向线程控制块的指针。
 * @param userStackTop 用户栈的顶部地址。
 * @param argc 命令行参数的数量。
 * @param argv 命令行参数数组。
 * @param returnAddress 线程执行完毕后的返回地址。
 * @return uint32 更新后的用户栈顶部地址。
 */
uint32 prepare_User_Stack(
    tcb_t* thread,
    uint32 userStackTop,
    uint32 argc,
    char** argv, 
    uint32 returnAddress
)
{
    // 初始化命令行参数的总长度为 0
    uint32 totalArgvLength = 0;
    // 遍历所有命令行参数，计算总长度
    for (int32 i = 0; i < argc; i++)
    {
        char c;
        int j = 0;
        // 计算单个参数的长度
        while ((c = argv[i][j++]) != '\0')
        {
            totalArgvLength++;
        }
        // 加上字符串结束符 '\0' 的长度
        totalArgvLength++;
    }
    // 为命令行参数在用户栈上预留空间
    userStackTop -= totalArgvLength;
    // 将用户栈地址按 4 字节对齐
    userStackTop = userStackTop / 4 * 4;
    // 定义一个数组，用于存储参数的指针，数组长度为 argc + 1
    char* args[argc + 1];
    // 第一个参数设置为线程的名称
    args[0] = thread->name;
    // 获取命令行参数在用户栈上的起始地址
    char* argvCharsAddr = (char*)userStackTop;
    // 将命令行参数复制到用户栈上
    for (int i = 0; i < argc; i++)
    {
        // 复制参数到用户栈，并获取复制的长度
        uint32 length = strcpy(argvCharsAddr, argv[i]);
        // 记录参数在用户栈上的地址
        args[i + 1] = (char*)argvCharsAddr;
        // 移动到下一个参数的存储位置
        argvCharsAddr += length + 1;
    }
    // 为参数指针数组在用户栈上预留空间
    userStackTop -= ((argc + 1) * 4);
    // 获取参数指针数组在用户栈上的起始地址
    uint32 argvStart = userStackTop;
    // 将参数指针数组写入用户栈
    for (int i = 0; i < argc + 1; i++)
    {
        *((char**)(argvStart + i * 4)) = args[i];
    }
    // 为参数指针数组的起始地址预留 4 字节空间
    userStackTop -= 4;
    // 将参数指针数组的起始地址写入用户栈
    *((uint32*)userStackTop) = argvStart;
    // 为参数数量预留 4 字节空间
    userStackTop -= 4;
    // 将参数数量（包含线程名称）写入用户栈
    *((uint32*)userStackTop) = argc + 1;
    // 为返回地址预留 4 字节空间
    userStackTop -= 4;
    // 将返回地址写入用户栈
    *((uint32*)userStackTop) = returnAddress;
    // 获取中断栈的指针
    interrupt_stack_t* interruptStack = (interrupt_stack_t*)((uint32)thread->kernelEsp + sizeof(switch_stack_t));
    // 更新中断栈中的用户栈指针
    interruptStack->userEsp = userStackTop;
    // 返回更新后的用户栈顶部地址
    return userStackTop;
}

static void test_Thread()
{
    monitor_Printf("test_Thread!\n");
    while(1) {}
}

void thread_Test(void)
{
    tcb_t* testThread = thread_Init(nullptr, "testThread", test_Thread, THREAD_DEFAULT_PRIORITY, false);
    asm volatile (
        "movl %0, %%esp; \
        jmp resume_Thread": : "g" (testThread->kernelEsp) : "memory");
}

void destroy_Thread(tcb_t* thread)
{
    kfree((void*)thread->kernelStack);
    kfree(thread);
}