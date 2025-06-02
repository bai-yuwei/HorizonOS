/******************************************************************************
* @file    Stdlib.c
* @brief   通用库函数相关的文件.
* @details This is the detail description.
* @author  ywBai <yw_bai@outlook.com>
* @date    2025年05月06日 (created)
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
* Date          : 2025年05月06日;
* Revision         : 0.0.1;
* Author           : ywBai;
* Contents         :
******************************************************************************/

#include "Stdlib.h"

extern void* get_Ebp();

void memset(void* ptr, uint8 value, int num)
{
    for (int i = 0; i < num; i++)
    {
        *((uint8*)(ptr + i)) = value;
    }
}

void memcpy(void* dest, const void* src, int num)
{
    for (int i = 0; i < num; i++)
    {
        *((uint8*)(dest + i)) = *((uint8*)(src + i));
    }
}

int32 strcpy(char* dst, const char* src)
{
    int32 i = 0;
    while (src[i] != '\0')
    {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
    return i;
}

int32 strcmp(const char* str1, const char* str2)
{
    int32 i = 0;
    while (str1[i] != '\0' && str2[i] != '\0')
    {
        if (str1[i] != str2[i])
        {
            return str1[i] - str2[i];
        }
        i++;
    }
    return str1[i] - str2[i];
}

int strlen(const char* str)
{
    int32 i = 0;
    while (str[i]!= '\0')
    {
        i++;
    }
    return i;
}

/**
 * @brief 将一个 32 位整数转换为字符串。
 *
 * 该函数将一个 32 位有符号整数转换为对应的字符串表示。如果输入的整数为负数，
 * 则会在字符串开头添加负号。
 *
 * @param str 用于存储转换后字符串的缓冲区指针。
 * @param num 要转换的 32 位有符号整数。
 * @return int32 转换后字符串的长度（包括负号）。
 */
int32 int_To_Str(char* str, int32 num)
{
    // 初始化字符串的起始索引
    uint32 start = 0;

    // 检查数字是否为负数
    if (num < 0)
    {
        // 如果为负数，在字符串开头添加负号
        str[start++] = '-';
        // 将数字转换为正数以便后续处理
        num = -num;
    }

    // 用于临时存储转换后的数字字符的缓冲区
    char buf[16];
    // 在缓冲区末尾添加字符串结束符
    buf[15] = '\0';
    // 初始化临时缓冲区的结束索引
    uint32 end = 14;

    // 循环将数字的每一位转换为字符
    while (num > 0)
    {
        // 将数字的最后一位转换为对应的字符并存储到缓冲区
        buf[end--] = '0' + (num % 10);
        // 去掉数字的最后一位
        num /= 10;
    }

    // 将临时缓冲区中的字符复制到目标字符串中
    strcpy(str + start, buf + end + 1);

    // 返回转换后字符串的长度
    return start + 14 - end;
}

int32 int_To_Hex(char* str, int32 num)
{
    // 初始化字符串的起始索引
    uint32 start = 0;
    if (num < 0)
    {
        // 如果为负数，在字符串开头添加负号
        str[start++] = '-';
        // 将数字转换为正数以便后续处理
        num = -num;
    }
    str[start++] = '0';
    str[start++] = 'x';
    // 用于临时存储转换后的数字字符的缓冲区
    char buf[16];
    // 在缓冲区末尾添加字符串结束符
    buf[15] = '\0';
    // 初始化临时缓冲区的结束索引
    uint32 end = 14;
    // 循环将数字的每一位转换为字符
    while (num > 0)
    {
        uint32 remain = num % 16;
        if (remain < 10)
        {
            buf[end--] = '0' + remain;
        }
        else
        {
            buf[end--] = 'a' + remain - 10;
        }
        // 去掉数字的最后一位
        num /= 16;
    }
    // 将临时缓冲区中的字符复制到目标字符串中
    strcpy(str + start, buf + end + 1);
    // 返回转换后字符串的长度
    return start + 14 - end;
}

/**
 * @brief 格式化输出到指定字符串缓冲区。
 * 
 * 该函数模仿标准库中的 sprintf 函数，将格式化的字符串输出到指定的字符缓冲区中。
 * 支持的格式说明符包括 %d（有符号整数）、%u（无符号整数）、%x（十六进制整数）和 %s（字符串）。
 * 
 * @param str 用于存储格式化结果的字符缓冲区指针。
 * @param format 格式化字符串，包含普通字符和格式说明符。
 * @param ... 可变参数列表，对应格式说明符的实际参数。
 */
void sprintf(char* str, const char* format, ...)
{
    // 获取当前函数的基址指针，用于定位可变参数
    void* ebp = get_Ebp();
    // 计算可变参数的起始地址，假设参数在栈上的偏移为 16 字节
    void* argPtr = ebp + 16;
    // 记录当前在目标字符串中写入的索引位置
    uint32 writeIndex = 0;

    // 遍历格式化字符串，直到遇到字符串结束符
    while (*format != '\0')
    {
        // 检查是否遇到格式说明符的起始字符 %
        if (*format == '%')
        {
            // 移动到格式说明符字符
            format++;
            if (*format == 'd')
            {
                // 处理有符号整数格式说明符 %d
                // 从可变参数中获取一个 32 位有符号整数
                int32 num = *((int32*)argPtr);
                // 用于临时存储转换后的字符串
                char buf[16];
                // 移动到下一个可变参数
                argPtr += 4;
                // 将整数转换为字符串
                int_To_Str(buf, num);
                // 将转换后的字符串复制到目标缓冲区，并更新写入索引
                writeIndex += strcpy(str + writeIndex, buf);
            }
            else if (*format == 'u')
            {
                // 处理无符号整数格式说明符 %u
                // 从可变参数中获取一个 32 位无符号整数
                uint32 num = *((uint32*)argPtr);
                // 用于临时存储转换后的字符串
                char buf[16];
                // 移动到下一个可变参数
                argPtr += 4;
                // 将无符号整数转换为字符串
                int_To_Str(buf, (int32)num);
                // 将转换后的字符串复制到目标缓冲区，并更新写入索引
                writeIndex += strcpy(str + writeIndex, buf);
            }
            else if (*format == 'x')
            {
                // 处理十六进制整数格式说明符 %x
                // 从可变参数中获取一个 32 位无符号整数
                uint32 num = *((uint32*)argPtr);
                // 用于临时存储转换后的字符串
                char buf[16];
                // 移动到下一个可变参数
                argPtr += 4;
                // 将无符号整数转换为十六进制字符串
                int_To_Hex(buf, num);
                // 将转换后的字符串复制到目标缓冲区，并更新写入索引
                writeIndex += strcpy(str + writeIndex, buf);
            }
            else if (*format == 's')
            {
                // 处理字符串格式说明符 %s
                // 从可变参数中获取一个字符串指针
                char* strPtr = *(char**)argPtr;
                // 移动到下一个可变参数，这里的偏移计算有误，正确的是移动 4 字节（指针大小）
                argPtr += 4; 
                // 将字符串复制到目标缓冲区，并更新写入索引
                writeIndex += strcpy(str + writeIndex, strPtr);
            }
            // 移动到下一个格式化字符
            format++;
        }
        else
        {
            // 处理普通字符，直接复制到目标缓冲区
            str[writeIndex++] = *format;
            // 移动到下一个格式化字符
            format++;
        }
    }
    // 在目标字符串末尾添加字符串结束符
    str[writeIndex] = '\0';
}