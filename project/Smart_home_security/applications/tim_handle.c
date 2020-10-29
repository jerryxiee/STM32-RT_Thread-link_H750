/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-28     xqyjlj       the first version
 */
#include "rtthread.h"
#include "drv_common.h"

extern void ir_it_update(void);
extern void ir_ic_update(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        ir_it_update();
    }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) /* 捕获中断发生时执行 */
{
    if (htim->Instance == TIM3)
    {
        ir_ic_update();
    }
}
