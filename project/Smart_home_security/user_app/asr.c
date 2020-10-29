/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-27     xqyjlj       the first version
 */
#include "rtthread.h"
#include "ld3320.h"
#include "drv_spi.h"

#define DBG_TAG "asr"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define LD3320_WR_PIN       LD3320_PIN_NONE
#define LD3320_RST_PIN      GET_PIN(B, 1)
#define LD3320_IRQ_PIN      GET_PIN(H, 2)

static void ld3320_a_asr_over_callback(uint8_t num)
{
    switch (num)
    {
    case 1:
        LOG_D("开始");
        break;
    case 2:
        LOG_D("关闭");
        break;
    case 3:
        LOG_D("暂停");
        break;
    case 4:
        LOG_D("小权");
        break;
    case 5:
        LOG_D("打开空调");
        break;
    default:
        break;
    }
}

static void ld3320_asr_thread(void * parameter)
{
    static ld3320_t _ld3320;

    _ld3320 = ld3320_create("spi40", LD3320_WR_PIN, LD3320_RST_PIN, LD3320_IRQ_PIN, LD3320_MODE_ASR);
    ld3320_set_asr_over_callback(_ld3320, ld3320_a_asr_over_callback);

    ld3320_addcommand_tolist(_ld3320, "kai shi", 1);
    ld3320_addcommand_tolist(_ld3320, "guan bi", 2);
    ld3320_addcommand_tolist(_ld3320, "zan ting", 3);
    ld3320_addcommand_tolist(_ld3320, "xiao quan", 4);
    ld3320_addcommand_tolist(_ld3320, "da kai kong tiao", 5);
    ld3320_addcommand_fromlist(_ld3320);

    ld3320_asr_start(_ld3320);
    while (1)
    {
        ld3320_run(_ld3320, LD3320_MODE_ASR);
        rt_thread_mdelay(100);
    }
}

int init_asr(void)
{
    rt_thread_t thread = RT_NULL;
    rt_hw_spi_device_attach("spi4", "spi40", GPIOE, GPIO_PIN_4);
    thread = rt_thread_create("asr", ld3320_asr_thread, RT_NULL, 500, 15, 100);
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    return RT_EOK;
}

INIT_APP_EXPORT(init_asr);
