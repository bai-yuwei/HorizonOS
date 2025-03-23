output_folder := objs

s_srcs := $(shell find src -name *.s)
s_objs := $(patsubst %.c, %.o, $(notdir $(s_srcs)))
c_srcs := $(shell find src -name *.c)
c_objs := $(patsubst %.c, %.o, $(notdir $(c_srcs)))
# include_paths := $(patsubst %/, %, $(dir $(shell find src -name *.h)))
include_paths := $(dir $(shell find src -name *.h))
I_flags := $(include_paths:%=-I %)
I_flags += $(inc_paths:%=-I %)

# 编译 kernel
$(s_objs) : $(s_srcs)
	nasm -o $@ $< $(I_flags)
$(c_objs) : $(c_srcs)
	gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -no-pie -fno-pic -c $^ $(I_flags)

$(output_folder)/main.o : $(c_objs) $(s_srcs)

$(output_folder)/kernel.bin : $(s_objs) $(c_objs)
	ifeq ("$(shell if [ -d $(output_folder) ]; then echo "exist"; else echo "noexist"; fi)", "noexist")
	$(shell mkdir $(output_folder))
endif
	ld  -shared -m elf_i386 -T link.ld -o $@ $^

kernel_build : $(output_folder)/kernel.bin
	@echo "Kernel compilation successful!"
