################################################################################
# @file    BOCHS.mk
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

export NASM		= $(cross_prefix)nasm$(cross_suffix)

# 编译 elf 所需的支持库
export LIB_SUPPORT ?= LIB

# 编译参数
export CFLAGS +=

# 链接参数
export LFIAGS +=


