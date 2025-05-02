################################################################################
# @file    BOCHS_C.mk
# @brief   执行编译makefile.
# @details This is the detail description.
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

# binaries
cross_prefix =
ifeq ($(OS), WSL)
	cross_suffix	:= .exe
else
	cross_suffix	:=
endif

export CC		= $(cross_prefix)gcc$(cross_suffix)
export LINK		= $(cross_prefix)gcc$(cross_suffix)
# ar: Archive 用于创建、修改和管理 .a 文件   
# 示例： ar rcs libmath.a add.o sub.o mul.o
export AR       = $(cross_prefix)ar$(cross_prefix)
# as: 处理包含 #include #define #ifdef 等预处理命令时的汇编文件，或不是默认 .s 后缀的汇编文件
export AS		= $(CC) -x assembler-with-cpp
export GDB		= $(cross_prefix)gdb$(cross_suffix)
export SIZE		= $(cross_prefix)size$(cross_suffix)
export OBJCOPY	= $(cross_prefix)objcopy$(cross_suffix)
export OBJDUMP	= $(cross_prefix)objdump$(cross_suffix)
export BIN_CMD	= $(OBJCOPY) -O binary
export HEX_CMD	= $(OBJCOPY) -O ihex
export SREC_CMD	= $(OBJCOPY) -O srec

# 编译 elf 所需的支持库
export LIB_SUPPORT ?= LIB

# 编译参数
export CFLAGS += -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -no-pie -fno-pic -c 

# 链接参数
export LFLAGS += -m32 -shared -T $(LD_FILE)


