# 连接到本地 1234 端口的 GDB 调试服务器
target remote :1234
# 加载 sym 文件
symbol-file ../Bin/kernel/kernel/Debug/kernel.sym
# 在 main 函数处设置断点
break main
# 继续执行程序直到遇到断点
continue
