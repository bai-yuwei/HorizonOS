#c_srcs := $(shell find src -name *.c)
#c_objs := $(patsubst %.c, %.o, $(notdir $(c_srcs)))
c_srcs := $(shell find src -name main.c)
c_objs := $(patsubst %.c, %.o, $(notdir $(c_srcs)))
# include_paths := $(patsubst %/, %, $(dir $(shell find src -name *.h)))
include_paths := $(dir $(shell find src -name *.h))
I_flags := $(include_paths:%=-I %)

$(c_objs) : $(c_srcs)
	gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -no-pie -fno-pic -c $^ $(I_flags)
objs/main.o : $(c_objs)
	@mkdir -p $(dir $@)
	gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -no-pie -fno-pic -T link.ld $^ -o $@ $(I_flags)

objs/kernel.bin : objs/main.o
	ld  -shared -m elf_i386 -T link.ld -o $@ $<

run : objs/kernel.bin
	@echo "Compilation successful！"


debug :
	@echo "c_srcs = $(c_srcs)"
	@echo "c_objs = $(c_objs)"
	@echo "I_flags = $(I_flags)"

clean :
	@ rm -rf objs *.o
.PHOHY : run debug clean