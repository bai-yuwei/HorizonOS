################################################################################
# @file    config.mk
# @brief   This is a brief description of file config.mk.
# @details This is the detail description.
# @author  ywBai <yw_bai@outlook.com>
# @date    2025年02月05日 (created)
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
# -# Date          : 2025年02月05日;
# Revision         : 0.0.1;
# Author           : ywBai;
# Contents         :
################################################################################

# 工程根目录（相对于 makefile 文件的路径）
export ROOT_DIR				:= $(shell if [ -d ../Build ]; then echo ".."; else echo "../.."; fi)
export BIN_DIR				:= $(ROOT_DIR)/Bin

# 编译中间件路径（相对 makefile 文件的路径）
export OBJ_DIR				:= objs
export PLATFORMS_DIR		:= Platforms
export TARGETS_DIR			:= Targets
# 配置 make 时是否在每一行前显示当前编译的目标信息：TRUE/FALSE
export ECHO_PREFIX			:= TRUE
# 配置源文件不需要重新编译时是否重新链接库生成elf
export RE_LINK				:= TRUE

# 配置 make all 时的编译平台
# 若值为 ALL_PLAT，则 make all 时可编译全部支持的平台
# 若值为空，则 make all 时由 Targets 中的目标决定
default_plat := 
# 配置 make all 时的编译模式，可以配置为 ALL_MODE，Debug 和 Release
# 若为空，则由 Targets 中的目标决定
default_mode := 
