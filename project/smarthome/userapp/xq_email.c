/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-12     xqyjlj       the first version
 */

#define DBG_COLOR
#define DBG_TAG "email"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include "smtp_client.h"
#include "rtthread.h"
#include <easyflash.h>
#include "string.h"
#include "stdio.h"

#include "stdlib.h"
#include "wifi.h"
#include <dfs_fs.h>
#include <rtdevice.h>
#include <drv_common.h>
char *stmp_start_content = "你的设备已开机\r\n"
        "目前已经开机%d次\r\n"
        "--------------------------------------\r\n"
        "基于 --->   RT-Thread\r\n"
        "开发人员  --->   小权一句两句\r\n"
        "联系方式 --->    xqyjlj@126.com";

void xq_send_mail_start(void)
{
    char *smtp_addr, *smtp_port, *smtp_name, *smtp_pass;
    smtp_addr ="smtp.126.com";
    smtp_port ="25";
    smtp_name ="xqyjlj@126.com";
    smtp_pass ="";


    smtp_client_init();
    smtp_set_server_addr(smtp_addr, ADDRESS_TYPE_DOMAIN, smtp_port);
    smtp_set_auth(smtp_name, smtp_pass);
    smtp_add_receiver("2284531143@qq.com");

    char *content = rt_malloc(400);

    uint32_t i_boot_times = 0;
    char *c_old_boot_times = { 0 };

    c_old_boot_times = ef_get_env("boot_times");/* get the boot count number from Env */
    RT_ASSERT(c_old_boot_times);
    i_boot_times = atol(c_old_boot_times);

    sprintf(content, stmp_start_content, i_boot_times);

    if (smtp_send_mail("你的设备已开机", content) == 0)
    {
        LOG_I("send mail success!");
    }
    else
    {
        LOG_E("send mail fail!");

    }
    rt_free(smtp_addr);
    rt_free(smtp_port);
    rt_free(smtp_name);
    rt_free(smtp_pass);
    rt_free(content);
}

void smtp_thread(void *param)
{
    while(!wifi_is_ready())
    {
        rt_thread_mdelay(100);
    }
    rt_thread_mdelay(2000);
//    xq_send_mail_start();
}

int smtp_thread_entry(void)
{
    rt_thread_t smtp_client_tid;
    smtp_client_tid = rt_thread_create("smtp", smtp_thread, RT_NULL, 40*1024, 20, 5);
    if (smtp_client_tid != RT_NULL)
    {
        rt_thread_startup(smtp_client_tid);
    }
    return RT_EOK;
}
INIT_APP_EXPORT(smtp_thread_entry);
