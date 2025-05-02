/******************************************************************************
* @file    main.c
* @brief   kernel main.
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
#include "Std_Type.h"
#include "Monitor.h"

char* helloWorld = "Hello World!\n";

int main(void)
{
    monitor_Init();
    monitor_Put_String_With_Color(helloWorld, COLOR_LIGHT_GREEN);
    while(1);
    return 0;
}