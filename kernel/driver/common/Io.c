/******************************************************************************
* @file    Io.c
* @brief   总线输入输出的文件.
* @details This is the detail description.
* @author  ywBai <yw_bai@outlook.com>
* @date    2025年05月01日 (created)
* @version 0.0.1
* @par Copyright (C):
*          Bai, yuwei. All Rights Reserved.
* @par Encoding:
*          UTF-8
* @par Description        :
* 1. Hardware Descriptions:
*      None.
* 2. Program Architecture:
*      None.
* 3. File Usage:
*      None.
* 4. Limitations:
*      None.
* 5. Else:
*      None.
* @par Modification:
* Date          : 2025年05月01日;
* Revision         : 0.0.1;
* Author           : ywBai;
* Contents         :
******************************************************************************/
#include "Io.h"

void io_Out_Byte(uint16 port, uint8 data)
{
    __asm__ volatile("outb %1, %0" : : "dN" (port), "a" (data));
}

void io_In_Byte(uint16 port, uint8* data)
{
    __asm__ volatile("inb %1, %0" : "=a" (*data) : "dN" (port));
}

void io_In_Word(uint16 port, uint16* data)
{
    __asm__ volatile("inw %1, %0" : "=a" (*data) : "dN" (port));
}
