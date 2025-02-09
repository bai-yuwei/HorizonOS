/*
 * @Author       : Bai_Yuwei(bai_yuwei123@outlook.com)
 * @Version      : V1.0
 * @Date         : 2024-12-22 15:30:52
 * @Description  : 
 * @Modify       : 
 */


# shell：		调用 linux shell 指令
# patsubst：	替换字符串，将源字符串中的部分内容替换为目标字符串的内容
# %：			通配符
# notdir：		去掉文件的绝对路径，只保留文件名
# basename：	取前缀
# @：			目标文件
# <：			第一个依赖文件
# ^：			所有依赖文件
# dir：			取目录
# wildcard：	匹配文件并返回文件列表
output_folder := objs
mbr_srcs := $(shell find src -name mbr.S)
mbr_objs := $(basename $(notdir $(mbr_srcs)))
loader_srcs := $(shell find src -name loader.S)
loader_objs := $(basename $(notdir $(loader_srcs)))
inc_paths := $(dir $(shell find src -name *.inc))
#c_srcs := $(shell find src -name *.c)
#c_objs := $(patsubst %.c, %.o, $(notdir $(c_srcs)))
c_srcs := $(shell find src -name main.c)
c_objs := $(patsubst %.c, %.o, $(notdir $(c_srcs)))
# include_paths := $(patsubst %/, %, $(dir $(shell find src -name *.h)))
include_paths := $(dir $(shell find src -name *.h))
I_flags := $(include_paths:%=-I %)
I_flags += $(inc_paths:%=-I %)