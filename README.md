# HorizonOS
## 1. 简述
该工程使用**MAKEFILE**管理
### 2. 工程目录架构
```bash
.
├── Bin                                            --> 成果物路径
├── Build                                          --> 编译配置仓库 
│   ├── config.mk
│   ├── Makefile
│   ├── Objs
│   ├── Platforms
│   ├── Rules_c.mk
│   ├── Rules_nasm.mk
│   └── Targets
├── mbr                                            --> 主引导程序仓库
├── boot                                           --> 启动程序仓库
├── kernel                                         --> 内核仓库
├── README.md                                      --> 说明文档 
├── running.sh                                     --> 运行脚本
├── bochsrc.txt                                    --> 虚拟机配置文件
└── scroll.img                                     --> 虚拟机磁盘镜像
```