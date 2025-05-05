################################################################################
# @file    Makefile
# @brief   最上层的主makefile.
# @details 执行功能如下：
# 1. 根据当前运行的操作系统修改系统指令
# 2. 获取要编译的目标、平台和编译命令
# 3. 根据获取到的信息执行对应的.mk文件执行编译
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

ifeq ($(OS), Windows_NT)
	export RM := /usr/bin/rm -rf
	export CP := /usr/bin/cp
	export MV := /usr/bin/mv
	export SHELL := /bin/bash
	export MAKE := /usr/bin/make --no-print-directory
	export FIND := /usr/bin/find
	# 将 /d/path 替换为 d:/path
	export PWD := $(shell /usr/bin/pwd | /usr/bin/sed 's/\/\(.\)/\1:/1')
else
	export RM := rm -rf
	export CP := cp
	export MV := mv
	export SHELL := /bin/bash
	export MAKE := make --no-print-directory
	export FIND := find
	export PWD := $(shell pwd)
	uname_a := $(shell uname -a)
ifneq ($(findstring Microsoft, $(uname_a)),)
		export OS := WSL
else ifneq ($(findstring Linux, $(uname_a)),)
		export OS := Linux
else ifneq ($(findstring Darwin, $(uname_a)),)
		export OS := MacOS
endif
endif
include config.mk

exclude_target			:= TemplateLib TemplateAPP
exclude_platform		:=
# wildcard: 匹配符合指定字符串的文件
# patsubst: 字符串替换
all_targets				:= $(patsubst $(TARGETS_DIR)/%.mk, %, $(wildcard $(TARGETS_DIR)/*.mk))
all_platforms			:= $(patsubst $(PLATFORMS_DIR)/%.mk, %, $(wildcard $(PLATFORMS_DIR)/*.mk))
# sort: 依据首字母排序
# filter-out: 从列表中删除排除项
platforms_to_make		:= $(filter-out $(exclude_platform), $(sort $(all_platforms)))
targets_to_make			:= $(filter-out $(exclude_target), $(sort $(all_targets)))
# targets_to_clean: 为每个元素添加指定后缀
targets_to_clean		:= $(addsuffix .clean, $(targets_to_make))

plat 					:=
mode					:=

ifneq ($(plat),)
	default_plat = $(plat)
endif
ifneq ($(mode),)
	default_mode = $(mode)
endif

ifeq ($(default_plat),ALL_PLAT)
	default_plat = $(platforms_to_make)
endif

all :
ifneq ($(targets_to_make),)
ifneq ($(default_plat),)
		@echo targets to make: $(targets_to_make) on platform $(default_plat)
		@$(MAKE) $(default_plat)
else
		@echo targets to make: $(targets_to_make)
		@$(MAKE) $(targets_to_make) mode=$(default_mode)
endif
else
	@echo no targets found...
endif


clean :
ifneq ($(BIN_DIR),)
	$(RM) $(BIN_DIR)/*
else
	$(error BIN_DIR not configured...)
endif
	@echo done...
ifneq ($(OBJ_DIR_BASE),)
	$(RM) $(OBJ_DIR_BASE)
else
	$(error OBJ_DIR_BASE not configured...)
endif

cleanbin :
ifneq ($(BIN_DIR),)
	$(RM) $(BIN_DIR)/*
else
	$(error BIN_DIR not configured...)
endif
	@echo done...

cleanobj :
ifneq ($(OBJ_DIR),)
	$(RM) $(OBJ_DIR)
else
	$(error OBJ_DIR not configured...)
endif
	@echo done...

rebuild :
	$(MAKE) clean
	$(MAKE) all

$(targets_to_make):
ifeq ($(plat),)
ifeq ($(mode), ALL_MODE)
		@$(MAKE) $@.debug $@.release
else ifeq ($(mode), Debug)
		@$(MAKE) $@.debug
else ifeq ($(mode), Release)
		@$(MAKE) $@.release
else ifeq ($(mode),)
		@echo $(MAKE) -f $(TARGETS_DIR)/$@.mk
		@$(MAKE) -f $(TARGETS_DIR)/$@.mk
else
		$(error mode not supported, which shoud be ALL_MODE/Debug/Release)
endif
else ifeq ($(plat), ALL_PLAT)
	@$(MAKE) $(addsuffix .plat, $(platforms_to_make)) currentTarget = $@
else
	@$(MAKE) $(addsuffix .plat, $(plat)) currentTarget = $@
endif

%.debug:
	@$(MAKE) -f $(TARGETS_DIR)/$(basename $@).mk MODE=Debug
%.release:
	@$(MAKE) -f $(TARGETS_DIR)/$(basename $@).mk MODE=Release

$(targets_to_clean):
	@echo clean $(basename $@)...
	@$(MAKE) -f $(TARGETS_DIR)/$(basename $@).mk clean MODE=Debug
	@$(MAKE) -f $(TARGETS_DIR)/$(basename $@).mk clean MODE=Release
	@echo done..

%.plat:
	@$(MAKE) $(currentTarget) plat= PLATFORM=$(basename $@)

$(all_platforms):
	@$(MAKE) $(targets_to_make) plat= mode=$(default_mode) PLATFORM=$@

help:
	@echo current support targets: all rebuild clean cleanobj help $(targets_to_make)

.PHOHY : mbr_build loader_build kernel_build debug clean help