################################################################################
# @file    Rules.mk
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
SRC_FILES			:= $(shell $(FIND) $(TOP_SRC_DIRS) ! -path "*/.svn/*" $(patsubst %,! -path "%/*", $(EXCLUDE_DIRS)) -iname "*.[cs]" -type f)
SRC_FILES			:= $(sort $(SRC_FILES))
INC_DIRS			:= $(shell $(FIND) $(INC_DIRS) ! -path "*/.svn" ! -path "*/.svn/*" $(patsubst %,! -path "%" ! -path "%/*",$(EXCLUDE_DIRS)) -type d)
INC_DIRS			:= $(sort $(INC_DIRS))

ifeq ($(SRC_FILES),)
$(error no src file found ***)
else
RUN_FILES	:= $(patsubst $(ROOT_DIR)/%, $(OBJ_DIR)/%, $(SRC_FILES))
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
AFLAGS += -$(D_OPTIMIZATION)
AFLAGS += -$(D_DEBUG_LEVEL)
endif
INC_DIRS_SLASH := $(addsuffix /, $(INC_DIRS))
CFLAGS += $(patsubst %, -I%, $(INC_DIRS))
AFLAGS += $(patsubst %, -I%, $(INC_DIRS_SLASH))


################################################################################################################
# 成果物入口
all : echo_info $(TARGETS)
	
ifneq ($(filter elf srec bin hex, $(TARGET_TYPE)),)
ifneq ($(SIZE),)
	@$(SIZE) --format=berkeley $(patsubst $(ROOT_DIR)/%, $(ROOT_RELATIVE_DIR)/%, $(TARGET_BASENAME).elf)
endif
endif

echo_info:
	@echo "start making target $(TARGET_NAME) (platform: $(PLATFORM), mode: $(MODE))..."


# 生成 bin
$(TARGETS) : $(SRC_FILES)
	@echo $(INC_DIRS)
	@echo $(AFLAGS)
	@echo "building $(TARGETS) by $(SRC_FILES)"
	@mkdir -p $(RELEASE_DIR)
	@$(NASM) $(AFLAGS) $< -o $@ 


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