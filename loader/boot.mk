output_folder := objs
mbr_srcs := $(shell find src -name mbr.S)
mbr_objs := $(basename $(notdir $(mbr_srcs)))
loader_srcs := $(shell find src -name loader.S)
loader_objs := $(basename $(notdir $(loader_srcs)))
inc_paths := $(dir $(shell find src -name *.inc))
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

all : mbr_build loader_build

help :
	@echo "mbr_build"
	@echo "loader_build"
	@echo "clean"

clean :
	@ rm -rf objs *.o

.PHOHY : mbr_build loader_build debug clean help