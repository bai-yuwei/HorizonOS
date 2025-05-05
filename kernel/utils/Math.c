/******************************************************************************
* @file    Math.c
* @brief   数学相关工具函数的文件.
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

#include "Math.h"
/**
 * @brief 自定义除法运算，当被除数为正数时，结果向零取整；当被除数为负数时，结果向下取整。
 * @param x 被除数
 * @param n 除数
 * @return uint32 除法运算的结果
 */
int32 div(int32 x, int32 n)
{
    if (x > 0)
    {
        return x / n;
    }
    else
    {
        return (x - n + 1) / n;
    }
}

/**
 * @brief 计算两个整数相除后的非负取模结果。
 * @param x 被除数。
 * @param n 除数。
 * @return x 除以 n 的非负余数，结果范围在 [0, n) 内。
 */
int32 mod(int32 x, int32 n)
{
    // 先计算 x 除以 n 的余数
    // 再加上 n 确保中间结果为非负数
    // 最后再次取余得到 [0, n) 范围内的结果
    return (x % n + n) % n;
}