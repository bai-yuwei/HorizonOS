################################################################################
# @file    mbr.mk
# @brief   编译 mbr 的配置.
# @details 
# @author  ywBai <yw_bai@outlook.com>
# @date    2025年03月30日 (created)
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
# -# Date          : 2025年03月30日;
# Revision         : 0.0.1;
# Author           : ywBai;
# Contents         :
################################################################################

###############################################################################
#配置说明:
# 1. 目录名或文件名中不能有空格
# 2. 若有不需要的配置项，删除该配置项等号右边的内容即可
###############################################################################

# 工程名
PRODUCT				:= boot
# 要生成的目标文件名
TARGET_NAME			:= mbr
# 要生成的目标类型（后缀名），可以写多个，以空格隔开，支持bin a out so
TARGET_TYPE			:= bin
# 硬件平台，支持 Build/Platforms 中包含的平台
PLATFORM			:= BOCHS_NASM
# Debug模式时的优化等级和调试等级
# 优化等级 O0 O1 O2 O3 Os Og, 调试等级 g0 g1 g2 g3
D_OPTIMIZATION		:= O0
D_DEBUG_LEVEL		:= g3
# Release模式时的优化等级和调试等级
R_OPTIMIZATION		:= O0
R_DEBUG_LEVEL		:= g0
# 默认模式，如果 makefile 中没有指定，则采用此处定义的模式，可设置为 Debug/Release
MODE				?= Debug
# 指定 C 编译额外编译参数
CFLAGS				+=
# 指定汇编编译额外编译参数
AFLAGS				+=
# 指定额外链接参数
LFLAGS				+=

###############################################################################
# 配置相对于工程根目录的代码路径，若有多个目录，以空格隔开
###############################################################################
# 工程中的源码目录（要编译的源码的最上层目录，若有多个，以空格隔开）
TOP_SRC_DIRS		:= $(PRODUCT)
# 要排除的编译目录（要排除编译的源码的最上层目录，若有多个，以空格隔开）
EXCLUDE_DIRS		:=
# 使用静态库，若有多个，以空格隔开
LIBS				:=
# 静态库目录（要使用的静态库最上层目录，若有多个，以空格隔开）
LIB_DIRS			:=
# 头文件目录（要搜索的头文件最上层目录，若有多个，以空格隔开）
INC_DIRS			:= $(PRODUCT)
# 目标文件存放目录
RELEASE_DIR			:= Bin/$(PRODUCT)/$(MODE)
# 编译中间文件存放目录
OBJ_DIR				:= $(OBJ_DIR_BASE)/$(PRODUCT)/$(MODE)

# 若生成目标是静态库，则还需要配置以下内容
# 对外头文件所在目录
LIB_INC_SRC_DOR		:=
# 对外头文件存放目录
LIB_INC_RELEASE_DIR	:=

# 若生成目标是可执行文件，还需要配置以下内容
# 链接文件相对路径及文件名
LD_FILE				:=



include Rules_nasm.mk

