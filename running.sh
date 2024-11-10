#!/bin/sh
dd if=./objs/mbr of=scroll.img bs=512 count=1 seek=0 conv=notrunc
dd if=./objs/loader of=scroll.img bs=512 count=5 seek=2 conv=notrunc
bochs -f bochsrc.txt