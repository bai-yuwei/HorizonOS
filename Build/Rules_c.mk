################################################################################
# @file    Rules_c.mk
# @brief   规则makefile.
# @details 将配置定义实际配置为编译器指令.
# @author  ywBai <yw_bai@outlook.com>
# @date    2025年02月16日 (created)
# @version 0.0.1
# @par Copyright (C):
#          Bai, yuwei. All Rights Reserved.
# @par Encoding:
#          UTF-8
# @par Description        :
# 1. Hardware Descriptions:
#      None.
# 2. Program Architecture:
#      None.
# 3. File Usage:
#      None.
# 4. Limitations:
#      None.
# 5. Else:
#      None.
# @par Modification:
# -# Date          : 2025年02月16日;
# Revision         : 0.0.1;
# Author           : ywBai;
# Contents         :
################################################################################

# 配置合法性检查&进一步处理
ifeq ($(ROOT_DIR),)
$(warning ROOT_DIR not defined***)
ROOT_DIR	:= .
endif

ROOT_RELATIVE_DIR	:= $(ROOT_DIR)
# Windows 系统中，编译使用绝对路径，便于在终端编译时定位源文件
ifeq ($(OS), Windows_NT)
ROOT_DIR	:= $(PWD)/$(ROOT_DIR)
endif

ifneq ($(PLATFORM).mk, $(notdir $(wildcard $(PLATFORMS_DIR)/$(PLATFORM).mk)))
	@echo $(PLATFORM)
	$(error platform $(PLATFORM) not supported...)
endif

include $(PLATFORMS_DIR)/$(PLATFORM).mk

ifeq ($(TOP_SRC_DIRS),)
TOP_SRC_DIRS := $(ROOT_DIR)
else
TOP_SRC_DIRS := $(addprefix $(ROOT_DIR)/, $(TOP_SRC_DIRS))
endif

ifeq ($(LIB_INC_SRC_DIR)$(TARGET_TYPE),a)
copy_inc_dir := FALSE
endif
ifeq ($(LIB_INC_RELEASE_DIR)$(TARGET_TYPE),a)
copy_inc_dir := FALSE
endif

EXCLUDE_DIRS		:= $(addprefix $(ROOT_DIR)/, $(EXCLUDE_DIRS))
INC_DIRS			:= $(addprefix $(ROOT_DIR)/, $(INC_DIRS))
LIB_DIRS			:= $(addprefix $(ROOT_DIR)/, $(LIB_DIRS))
EWL_LIB_DIR			:= $(addprefix $(ROOT_DIR)/, $(EWL_LIB_DIR))
RELEASE_DIR			:= $(addprefix $(ROOT_DIR)/, $(RELEASE_DIR))
LIB_INC_SRC_DIR		:= $(addprefix $(ROOT_DIR)/, $(LIB_INC_SRC_DIR))
ifneq ($(LD_FILE),)
LD_FILE				:= $(addprefix $(ROOT_DIR)/, $(LD_FILE))
endif

TARGET_BASENAME		:= $(RELEASE_DIR)/$(TARGET_NAME)
TARGETS				:= $(addprefix $(TARGET_BASENAME)., $(TARGET_TYPE))
# 查找源文件及头文件目录，默认排除 .svn 路径
SRC_FILES			:= $(shell $(FIND) $(TOP_SRC_DIRS) $(patsubst %,! -path "%/*", $(EXCLUDE_DIRS)) -iname "*.[cs]" -type f)
SRC_FILES			:= $(sort $(SRC_FILES))
INC_DIRS			:= $(shell $(FIND) $(INC_DIRS) $(patsubst %,! -path "%" ! -path "%/*",$(EXCLUDE_DIRS)) -type d)
INC_DIRS			:= $(sort $(INC_DIRS))

ifeq ($(SRC_FILES),)
$(error no src file found ***)
else
OBJ_FILES	:= $(patsubst $(ROOT_DIR)/%, $(OBJ_DIR)/%.o, $(SRC_FILES))
DEP_FILES	:= $(patsubst %.o, %.d, $(OBJ_FILES))
endif

# 配置编译参数
ifeq ($(MODE), Release)
CFLAGS += -$(R_OPTIMIZATION)
CFLAGS += -$(R_DEBUG_LEVEL)
AFLAGS += -$(R_OPTIMIZATION)
AFLAGS += -$(R_DEBUG_LEVEL)
else
CFLAGS += -$(D_OPTIMIZATION)
CFLAGS += -$(D_DEBUG_LEVEL)
CFLAGS += -g
AFLAGS += -$(D_OPTIMIZATION)
AFLAGS += -$(D_DEBUG_LEVEL)
endif

CFLAGS += $(patsubst %, -I%, $(INC_DIRS))
AFLAGS += $(patsubst %, -I%, $(INC_DIRS))


################################################################################################################
# 成果物入口
all : echo_info $(TARGETS)
	@echo $(TARGETS)
ifneq ($(filter elf srec bin hex, $(TARGET_TYPE)),)
ifneq ($(SIZE),)
	@$(SIZE) --format=berkeley $(patsubst $(ROOT_DIR)/%, $(ROOT_RELATIVE_DIR)/%, $(TARGET_BASENAME).elf)
endif
endif

echo_info:
	@echo "start making target $(TARGET_NAME) (platform: $(PLATFORM), mode: $(MODE))..."

# 编译和链接
$(TARGET_BASENAME).elf : $(relink) $(OBJ_FILES) $(LINK_FILE)
	@echo "linking..."
	@echo "generating $(notdir $@)..."
	@mkdir -p $(RELEASE_DIR)
	@ld -o $@ $(OBJ_FILES) $(LFLAGS)

# 生成 srec
$(TARGET_BASENAME).srec : $(TARGET_BASENAME).elf
ifneq ($(SREC_CMD),)
	@echo "generating $(notdir $@)..."
	@$(SREC_CMD) $< $@
else
	$(error can't generate srec, SREC_CMD not supported...)
endif

# 生成 hex
$(TARGET_BASENAME).hex : $(TARGET_BASENAME).elf
ifneq ($(HEX_CMD),)
	@echo "generating $(notdir $@)..."
	@$(HEX_CMD) $< $@
else
	$(error can't generate hex, HEX_CMD not supported...)
endif

# 生成 bin
$(TARGET_BASENAME).bin : $(TARGET_BASENAME).elf
ifneq ($(BIN_CMD),)
	@echo "generating $(notdir $@)..."
	@$(HEX_CMD) $< $@
else
	$(error can't generate hex, HEX_CMD not supported...)
endif

# 生成 sym
$(TARGET_BASENAME).sym : $(TARGET_BASENAME).elf
ifneq ($(SYM_CMD),)
	@echo "generating $(notdir $@)..."
	@$(SYM_CMD) $< $@
else
	$(error can't generate sym, SYM_CMD not supported...)
endif

# 生成 .a
$(TARGET_BASENAME).a : $(OBJ_FILES)
	@mkdir -p $(RELEASE_DIR)
	@$(RM) $@
ifneq ($(copy_inc_dir),FALSE)
	@[ -d $(LIB_INC_RELEASE_DIR) ] || mkdir -p $(LIB_INC_RELEASE_DIR)
	@$(RM) $(LIB_INC_RELEASE_DIR)/*
	@$(CP) -R `$(FIND) $(LIB_INC_SRC_DIR) -maxdepth 1 !  -path "*/.svn" | sed 1d ` $(LIB_INC_RELEASE_DIR)
else
    @echo "skip copy include dir..."
endif
	@(ECHO) "generating $(notdir $@)..."
	@$(AR) -rcsD $@ $(OBJ_FILES)

# 编译.S
$(OBJ_DIR)/%.S.o : $(ROOT_DIR)/%.S
	@mkdir -p $(@D)
	@echo "building $(patsubst $(ROOT_DIR)/%, $(ROOT_RELATIVE_DIR)/%, $<)"
	@$(NASM) $(AFLAGS) $< -o $@

# 编译.c
$(OBJ_DIR)/%.c.o : $(ROOT_DIR)/%.c
	@mkdir -p $(@D)
	@echo "building $(patsubst $(ROOT_DIR)/%, $(ROOT_RELATIVE_DIR)/%, $<)"
# @$(CC) -I$(dir $<) $(CFLAGS) -MMD -MF"$(@:%.o=%.d)" -MT"$@" -c -o $@.i $<
	@$(CC) -I$(dir $<) $(CFLAGS) -MMD -MF"$(@:%.o=%.d)" -MT"$@" -c -o $@ $<

clean:
ifneq ($(LIB_INC_RELEASE_DIR),)
	@$(RM) $(LIB_INC_RELEASE_DIR)
endif
	@$(RM) $(TARGETS) $(OBJ_FILES) $(DEP_FILES)

clean_elf:
	@-$(RM) $(TARGETS)

print:
	@echo ****************************
	@echo OBJ_DIR : $(OBJ_DIR)
	@echo
	@echo TARGETS: $(TARGETS)
	@echo
	@echo TOP_SRC_DIRS : $(TOP_SRC_DIRS)
	@echo
	@echo SRC_FILES: $(SRC_FILES)
	@echo
	@echo OBJ_FILES: $(OBJ_FILES)
	@echo
	@echo DEP_FILES: $(DEP_FILES)
	@echo
	@echo CFLAGS: $(CFLAGS)
	@echo ****************************

.PHONY: all echo_target_info clean clean_elf
# .SECONDARY: $(OBJ_FILES)

ifneq ($(MAKECMDGOALS),clean)
-include $(DEP_FILES)
endif