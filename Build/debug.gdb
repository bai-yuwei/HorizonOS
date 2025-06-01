
# 连接到本地 1234 端口的 GDB 调试服务器
target remote :1234
set language c
# 加载 sym 文件
symbol-file ../Bin/kernel/kernel/Debug/kernel.sym
# 在 main 函数处设置断点
break main
# 避免缺页中断直接退出
# handle all nostop noprint pass
handle SIGSEGV nostop noprint pass
# handle SIGINT nostop noprint pass
break isr14
commands
    continue
end
# 查看所有信号处理情况
# info signals
# 继续执行程序直到遇到断点
continue
