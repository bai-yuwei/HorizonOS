/******************************************************************************
* @file    Monitor.h
* @brief   kernel显示相关的头文件.
* @details This is the detail description.
* @author  ywBai <yw_bai@outlook.com>
* @date    2025年04月27日 (created)
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
* Date          : 2025年04月27日;
* Revision         : 0.0.1;
* Author           : ywBai;
* Contents         :
******************************************************************************/
#ifndef MONITOR_H
#define MONITOR_H

#include "Std_Type.h"
#include "Io.h"
#include "Math.h"

#define VIDEO_MEMORY_START 0xC00B8000

#define COLOR_BLACK             0
#define COLOR_BLUE              1
#define COLOR_GREEN             2
#define COLOR_CYAN              3
#define COLOR_RED               4
#define COLOR_MAGENTA           5
#define COLOR_BROWN             6
#define COLOR_WHITE             7
#define COLOR_LIGHT_BLACK       8
#define COLOR_LIGHT_BLUE        9
#define COLOR_LIGHT_GREEN       10
#define COLOR_LIGHT_CYAN        11
#define COLOR_LIGHT_RED         12
#define COLOR_LIGHT_MAGENTA     13
#define COLOR_LIGHT_BROWN       14
#define COLOR_LIGHT_WHITE       15

void monitor_Init(void);
void monitor_Move_Cursor(uint16 deltaX, uint16 deltaY);
void monitor_Clear(void);
void monitor_Put_Char_With_Color(char c, uint8 color);
void monitor_Put_String_With_Color(char *str, uint8 color);
void monitor_Put_Hex_With_Color(uint32 n, uint8 color);
void monitor_Put_Dec_With_Color(int32 n, uint8 color);
void monitor_Print(char* str);
void monitor_Print_Line(char* str);
void monitor_Print_With_Color(char* str, uint8 color);
void monitor_Printf_Args(char* str, void* argPtr);

#endif
