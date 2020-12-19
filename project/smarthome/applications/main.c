/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-09-02     RT-Thread    first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "drv_common.h"
#include <netdev_ipaddr.h>
#include <netdev.h>
#include <dfs_fs.h>

#include "web.h"
#include "basic.h"
#include "monitor.h"
#include "bt_module.h"
#include "wifi.h"

#include <easyflash.h>
#include "string.h"
#include "drv_dcmi.h"

#define DBG_COLOR
#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

extern void xq_ef_boot_times(void);
extern void xq_wifi_connect(void);
extern void mp3_play_song(char *path);
int main(void)
{
    wifi_init();
    basic_init();
    sys_monitor_init();
    web_init();
    bluetooth_init();
    easyflash_init();
    xq_ef_boot_times();

    rt_thread_mdelay(3000);
    xq_wifi_connect();
    mp3_play_song("/sdcard/aixia/yikaiji.mp3");
    return RT_EOK;
}

#include "stm32h7xx.h"
static int vtor_config(void)
{
    /* Vector Table Relocation in Internal QSPI_FLASH */
    SCB->VTOR = QSPI_BASE;
    return 0;
}
INIT_BOARD_EXPORT(vtor_config);

void xq_ef_boot_times(void)
{
    uint32_t i_boot_times = 0;
    char *c_old_boot_times, c_new_boot_times[11] = { 0 };

    c_old_boot_times = ef_get_env("boot_times");/* get the boot count number from Env */
    RT_ASSERT(c_old_boot_times);
    i_boot_times = atol(c_old_boot_times);

    i_boot_times++;/* boot count +1 */

    sprintf(c_new_boot_times, "%ld", i_boot_times);/* interger to string */

    ef_set_env("boot_times", c_new_boot_times);/* set and store the boot count number to Env */
}
