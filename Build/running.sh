#!/bin/sh
dd if=../Bin/boot/mbr/Debug/mbr.bin of=scroll.img bs=512 count=1 seek=0 conv=notrunc
dd if=../Bin/boot/loader/Debug/loader.bin of=scroll.img bs=512 count=5 seek=2 conv=notrunc
# dd if=../kernel/kernel.bin of=scroll.img bs=512 count=2048 seek=9 conv=notrunc
dd if=../Bin/kernel/kernel/Debug/kernel.elf of=scroll.img bs=512 count=2048 seek=9 conv=notrunc
bochs -f ../bochsrc.txt