################################################################################
# @file    Makefile
# @brief   This is a brief description of file Makefile.
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

exclude_target			:= TemplateLib TemplateApp
exclude_platform		:= LINUX MACOS
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
ifneq ($(plat), )
	default_plat = $(plat)
endif
ifneq ($(mode), )
	default_mode = $(mode)
endif

ifeq ($(default_plat), ALL_PLAT)
	default_plat = $(platforms_to_make)
endif

all :
ifneq ($(targets_to_make), )
	ifeq ($(default_plat), )
		@echo targets to make: $(targets_to_make)
		@$(MAKE) $(targets_to_make) mode=$(default_mode)
	else
		@echo targets to make: $(targets_to_make) on platform $(default_plat)
		@$(MAKE) $(default_plat)
	endif
else
	@echo no targets found...
endif


clean :
ifneq ($(OBJ_DIR), )
	$(RM) $(OBJ_DIR)
else
	$(error OBJ_DIR not configured...)
endif
ifneq ($(BIN_DIR), )
	$(RM) $(BIN_DIR)/*
else
	$(error BIN_DIR not configured...)
endif
	@echo done...

cleanbin :
ifneq ($(BIN_DIR), )
	$(RM) $(BIN_DIR)/*
else
	$(error BIN_DIR not configured...)
endif
	@echo done...

cleanobj :
ifneq ($(OBJ_DIR), )
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
		@$(MAKE) -f $(TARGETS_DIR)/$@.mk

.PHOHY : mbr_build loader_build kernel_build debug clean help