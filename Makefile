# shell：		调用 linux shell 指令
# patsubst：	替换字符串，将源字符串中的部分内容替换为目标字符串的内容
# %：			通配符
# notdir：		去掉文件的绝对路径，只保留文件名
# basename：	取前缀
# @：			目标文件
# <：			第一个依赖文件
# ^：			所有依赖文件
# dir：			取目录
# wildcard：	匹配文件并返回文件列表
output_folder := objs
mbr_srcs := $(shell find src -name mbr.S)
mbr_objs := $(basename $(notdir $(mbr_srcs)))
loader_srcs := $(shell find src -name loader.S)
loader_objs := $(basename $(notdir $(loader_srcs)))
inc_paths := $(dir $(shell find src -name *.inc))
#c_srcs := $(shell find src -name *.c)
#c_objs := $(patsubst %.c, %.o, $(notdir $(c_srcs)))
c_srcs := $(shell find src -name main.c)
c_objs := $(patsubst %.c, %.o, $(notdir $(c_srcs)))
# include_paths := $(patsubst %/, %, $(dir $(shell find src -name *.h)))
include_paths := $(dir $(shell find src -name *.h))
I_flags := $(include_paths:%=-I %)
I_flags += $(inc_paths:%=-I %)

# 编译 mbr
$(mbr_objs) : $(mbr_srcs)
	nasm -o $@ $< $(I_flags)

mbr_build : $(mbr_objs)
ifeq ("$(shell if [ -d $(output_folder) ]; then echo "exist"; else echo "noexist"; fi)", "noexist")
	$(shell mkdir $(output_folder))
endif
	$(shell mv $(mbr_objs) ./$(output_folder)/$(mbr_objs))
	@echo "Mbr compilation successful!"

# 编译 loader
$(loader_objs) : $(loader_srcs)
	nasm -o $@ $< $(I_flags)

loader_build : $(loader_objs)
ifeq ("$(shell if [ -d $(output_folder) ]; then echo "exist"; else echo "noexist"; fi)", "noexist")
	$(shell mkdir $(output_folder))
endif
	$(shell mv $(loader_objs) ./$(output_folder)/$(loader_objs))
	@echo "Loader compilation successful!"

# 编译 kernel
$(c_objs) : $(c_srcs)
	gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -no-pie -fno-pic -c $^ $(I_flags)
$(output_folder)/main.o : $(c_objs)
ifeq ("$(shell if [ -d $(output_folder) ]; then echo "exist"; else echo "noexist"; fi)", "noexist")
	$(shell mkdir $(output_folder))
endif
	gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -no-pie -fno-pic -T link.ld $^ -o $@ $(I_flags)

$(output_folder)/kernel.bin : $(output_folder)/main.o
	ld  -shared -m elf_i386 -T link.ld -o $@ $<

kernel_build : $(output_folder)/kernel.bin
	@echo "Kernel compilation successful!"


debug :
	@echo "loader_srcs = $(loader_srcs)"
	@echo "loader_objs = $(loader_objs)"
	@echo "mbr_srcs = $(mbr_srcs)"
	@echo "mbr_objs = $(mbr_objs)"
	@echo "c_srcs = $(c_srcs)"
	@echo "c_objs = $(c_objs)"
	@echo "I_flags = $(I_flags)"
	@echo "$(output_folder_detect)"
	@echo "$(output_folder)$(mbr_objs)"

help :
	@echo "mbr_build"
	@echo "loader_build"
	@echo "kernel_build"
	@echo "debug"
	@echo "clean"

clean :
	@ rm -rf objs *.o

.PHOHY : mbr_build loader_build kernel_build debug clean help