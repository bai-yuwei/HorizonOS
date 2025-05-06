/******************************************************************************
* @file    Monitor.c
* @brief   kernel显示相关的文件.
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

#include "Monitor.h"

extern void* get_Ebp();

uint16* videoMemory = (uint16*)VIDEO_MEMORY_START;

int16 cursorX = 0;
int16 cursorY = 0;

const uint8 backColor = COLOR_BLACK;
const uint8 foreColor = COLOR_WHITE;

static void monitor_Set_Cursor(void)
{
    uint16 cursorLocation = cursorY * 80 + cursorX;
    io_Out_Byte(0x3D4, 14);
    io_Out_Byte(0x3D5, cursorLocation >> 8);
    io_Out_Byte(0x3D4, 15);
    io_Out_Byte(0x3D5, cursorLocation);
}

static void monitor_Scroll(void)
{
    const uint8 attributeByte = (backColor << 4) | (foreColor & 0x0F);
    const uint16 blank = 0x20 | (backColor << 4) | (attributeByte << 8);
    if (cursorY >= 25)
    {
        int i;
        for (i = 0; i < 24 * 80; i++)
        {
            videoMemory[i] = videoMemory[i + 80];
        }
        for (i = 24 * 80; i < 25 * 80; i++)
        {
            videoMemory[i] = blank;
        }
        cursorY = 24;
    }
}

void monitor_Init(void)
{}

void monitor_Move_Cursor(uint16 deltaX, uint16 deltaY)
{
    cursorX += deltaX;
    int32 offsetY = div(cursorX, 80);
    cursorX = mod(cursorX, 80);
    cursorY += deltaY + offsetY;
    if (cursorY < 0)
    {
        cursorX = 0;
        cursorY = 0;
    }
    else if (cursorY >= 25)
    {
        cursorX = 79;
        cursorY = 24;
    }
    monitor_Set_Cursor();
}

void monitor_Clear(void)
{
    const uint8 attributeByte = (backColor << 4) | (foreColor & 0x0F);
    const uint16 blank = 0x20 | (backColor << 4) | (attributeByte << 8);
    int i;
    for (i = 0; i < 25 * 80; i++)
    {
        videoMemory[i] = blank;
    }
    cursorX = 0;
    cursorY = 0;
    monitor_Set_Cursor();
}

/**
 * @brief 在屏幕上以指定颜色显示一个字符，并处理特殊字符。
 * @param c 要显示的字符。
 * @param color 字符的前景色。
 */
void monitor_Put_Char_With_Color(char c, uint8 color)
{
    // 计算属性字节，将背景色左移4位，与前景色的低4位组合
    const uint8 attributeByte = (backColor << 4) | (color & 0x0F);
    // 将属性字节左移8位，得到16位的属性值
    const uint16 attribute = attributeByte << 8;
    // 初始化指向视频内存起始地址的指针
    uint16* location = (uint16*)VIDEO_MEMORY_START;

    // 处理退格字符（ASCII码为0x08），且光标不在最左侧时
    if (c == 0x08 && cursorX)
    {
        // 光标左移一位
        cursorX--;
    }
    // 处理制表符（ASCII码为0x09）
    else if (c == 0x09)
    {
        // 将光标位置对齐到下一个8字符制表位
        cursorX = (cursorX + 8) & ~(8 - 1);
    }
    // 处理回车字符（ASCII码为'\r'）
    else if (c == '\r')
    {
        // 光标移到当前行的最左侧
        cursorX = 0;
    }
    // 处理换行字符（ASCII码为'\n'）
    else if (c == '\n')
    {
        // 光标移到下一行的最左侧
        cursorX = 0;
        cursorY++;
    }
    // 处理可打印字符（ASCII码大于等于空格字符 ' '）
    else if (c >= ' ')
    {
        // 计算字符在视频内存中的位置
        location = videoMemory + (cursorY * 80 + cursorX);
        // 将字符和属性组合后写入视频内存
        *location = c | attribute;
        // 光标右移一位
        cursorX++;
    }
}

void monitor_Put_String_With_Color(char *str, uint8 color)
{
    while (*str)
    {
        monitor_Put_Char_With_Color(*str++, color);
    }
}

/**
 * @brief 以指定颜色在屏幕上显示一个 32 位无符号整数的十六进制表示。
 * 
 * 该函数会在十六进制数前添加 "0x" 前缀，并且会自动忽略前导零。
 * 
 * @param n 要显示的 32 位无符号整数。
 * @param color 显示的十六进制数的前景色。
 */
void monitor_Put_Hex_With_Color(uint32 n, uint8 color)
{
    // 标志位，用于判断是否还存在前导零
    char noZeroes = 1;
    // 存储当前处理的 4 位二进制对应的十六进制值
    uint8 current = 0;
    int i = 0;
    // 先显示十六进制前缀 "0x"
    monitor_Put_String_With_Color("0x", color);
    // 从 32 位整数的最高 4 位开始，逐 4 位处理，直到最低 4 位
    for (i = 28; i >= 0; i -= 4)
    {
        // 提取当前 4 位二进制对应的十六进制值
        current = (n >> i) & 0xF;
        // 如果当前十六进制值为 0 且还存在前导零，则跳过本次循环
        if (current == 0 && noZeroes != 0)
        {
            continue;
        }
        // 如果当前十六进制值大于等于 10（即 A - F）
        if (current >= 0xA)
        {
            // 表示已经处理完前导零
            noZeroes = 0;
            // 显示对应的大写字母 A - F
            monitor_Put_Char_With_Color(current - 0xA + 'A', color);
        }
        else
        {
            // 表示已经处理完前导零
            noZeroes = 0;
            // 显示对应的数字 0 - 9
            monitor_Put_Char_With_Color(current + '0', color);
        }
    }
    // 处理最低 4 位二进制对应的十六进制值
    current = n & 0xF;
    // 如果最低 4 位对应的十六进制值大于等于 10（即 A - F）
    if (current >= 0xA)
    {
        // 显示对应的大写字母 A - F
        monitor_Put_Char_With_Color(current - 0xA + 'A', color);
    }
    else
    {
        // 显示对应的数字 0 - 9
        monitor_Put_Char_With_Color(current + '0', color);
    }
}

/**
 * @brief 以指定颜色在屏幕上显示一个 32 位有符号整数的十进制表示。
 * 
 * 该函数会处理负数，在负数前添加负号 '-'，并将整数转换为对应的十进制字符串显示。
 * 
 * @param n 要显示的 32 位有符号整数。
 * @param color 显示的十进制数的前景色。
 */
void monitor_Put_Dec_With_Color(int32 n, uint8 color)
{
    // 若数字为 0，直接显示字符 '0' 并返回
    if (n == 0)
    {
        monitor_Put_Char_With_Color('0', color);
        return;
    }
    // 若数字为负数，先显示负号 '-'，再将其转换为正数处理
    if (n < 0)
    {
        monitor_Put_Char_With_Color('-', color);
        n = -n;
    }
    // 复制 n 的值到 acc，用于后续计算
    int acc = n;
    // 用于记录数字的位数
    int32 i = 0;
    // 用于存储数字的每一位字符，逆序存储
    int32 digits[32];
    // 循环将数字的每一位转换为字符并存储到 digits 数组中
    while (acc > 0)
    {
        // 将当前位数字转换为对应的字符 '0' - '9' 并存储
        digits[i++] = '0' + acc % 10;
        // 去掉已经处理的最低位
        acc /= 10;
    }
    // 在 digits 数组末尾添加字符串结束符
    digits[i] = 0;
    // 用于存储正序的数字字符
    char digits2[32];
    // 在 digits2 数组末尾添加字符串结束符
    digits2[i--] = 0;
    // 用于遍历 digits 数组的索引
    int j = 0;
    // 将 digits 数组中的字符逆序复制到 digits2 数组中
    while (i >= 0)
    {
        digits2[i--] = digits[j++];
    }
    // 调用 monitor_Put_String_With_Color 函数显示转换后的十进制字符串
    monitor_Put_String_With_Color(digits2, color);
}

void monitor_Print(char* str)
{
    monitor_Put_String_With_Color(str, foreColor);
}


void monitor_Print_Line(char* str)
{
    monitor_Put_String_With_Color(str, foreColor);
    monitor_Put_Char_With_Color('\n', foreColor);
}

void monitor_Print_With_Color(char* str, uint8 color)
{
    monitor_Put_String_With_Color(str, color);
}

void monitor_Printf(char* str, ...)
{
    void* ebp = get_Ebp();
    void* argPtr = ebp + 12;
    monitor_Printf_Args(str, argPtr);
}

/**
 * @brief 格式化输出函数，支持类似 printf 的格式化字符串功能。
 * 
 * 该函数会解析传入的格式化字符串，根据不同的格式说明符（如 %d、%u、%x、%s）
 * 从参数指针中获取相应的值并调用对应的显示函数进行输出。
 * 
 * @param str 格式化字符串，包含普通字符和格式说明符。
 * @param argPtr 指向可变参数列表的指针，用于获取格式说明符对应的参数值。
 */
void monitor_Printf_Args(char* str, void* argPtr)
{
    // 用于遍历格式化字符串的索引
    int i = 0;
    // 无限循环，直到遇到字符串结束符
    while (1)
    {
        // 获取当前字符
        char c = str[i];
        // 如果遇到字符串结束符 '\0'，退出循环
        if (c == '\0')
        {
            break;
        }
        // 如果当前字符是 '%'，表示可能遇到格式说明符
        if (c == '%')
        {
            // 移动到下一个字符
            i++;
            // 获取下一个字符
            char next = str[i];
            // 如果下一个字符是字符串结束符 '\0'，退出循环
            if (next == '\0')
            {
                break;
            }
            // 处理 '%d' 格式说明符，输出有符号十进制整数
            if (next == 'd')
            {
                // 将参数指针转换为 int32* 类型并解引用，调用显示十进制整数的函数
                monitor_Put_Dec_With_Color(*(int32*)argPtr, foreColor);
                // 参数指针向后移动 4 个字节，指向下一个参数
                argPtr += 4;
            }
            // 处理 '%u' 格式说明符，输出无符号十进制整数
            else if (next == 'u')
            {
                // 将参数指针转换为 uint32* 类型并解引用，调用显示十进制整数的函数
                monitor_Put_Dec_With_Color(*(uint32*)argPtr, foreColor);
                // 参数指针向后移动 4 个字节，指向下一个参数
                argPtr += 4;
            }
            // 处理 '%x' 格式说明符，输出无符号十六进制整数
            else if (next == 'x')
            {
                // 将参数指针转换为 uint32* 类型并解引用，调用显示十六进制整数的函数
                monitor_Put_Hex_With_Color(*(uint32*)argPtr, foreColor);
                // 参数指针向后移动 4 个字节，指向下一个参数
                argPtr += 4;
            }
            // 处理 '%s' 格式说明符，输出字符串
            else if (next == 's')
            {
                // 将参数指针转换为 char** 类型并解引用，调用显示字符串的函数
                monitor_Put_String_With_Color(*(char**)argPtr, foreColor);
                // 参数指针向后移动 4 个字节，指向下一个参数
                argPtr += 4;
            }
        }
        // 如果当前字符不是 '%'，则直接输出该字符
        else
        {
            monitor_Put_Char_With_Color(c, foreColor);
        }
        // 索引加 1，移动到下一个字符
        i++;
    }
}
