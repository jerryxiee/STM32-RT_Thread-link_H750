/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-12     xqyjlj       the first version
 */
#include "wifi.h"
#include "rtthread.h"
#include "wlan_mgnt.h"
#include <easyflash.h>

void xq_wifi_connect(void)
{
    char *wifi_ssid, *wifi_pass;
    size_t saved_len;

    wifi_ssid=rt_malloc(50);
    wifi_pass=rt_malloc(50);
    ef_get_env_blob("wifi_ssid", wifi_ssid, 50, &saved_len);
    ef_get_env_blob("wifi_pass", wifi_pass, 50, &saved_len);

    rt_wlan_connect(wifi_ssid, wifi_pass);

    rt_free(wifi_ssid);
    rt_free(wifi_pass);
}
