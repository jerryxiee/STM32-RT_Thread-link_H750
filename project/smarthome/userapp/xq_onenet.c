/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-13     xqyjlj       the first version
 */
#include <stdlib.h>
#include "wifi.h"
#include <onenet.h>
#include <dfs_fs.h>


#define DBG_COLOR
#define DBG_TAG "onenet"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

extern short Get_Temprate(void);


void upload_cpu_temperature(void)
{
    short cpu_temperature = Get_Temprate();
    onenet_mqtt_upload_digit("CPU温度", cpu_temperature / 100);
}

void upload_mem(void)
{
    rt_uint32_t total_mem = 0, used_mem = 0, max_used_mem = 0;
    rt_object_t mem_obj = rt_object_find("heap", RT_Object_Class_MemHeap);
    if (mem_obj)
    {
        struct rt_memheap *mh = (struct rt_memheap *)mem_obj;
        total_mem += mh->pool_size;
        used_mem += (mh->pool_size - mh->available_size);
        max_used_mem += mh->max_used_size;
    }
    mem_obj = rt_object_find("sdram", RT_Object_Class_MemHeap);
    if (mem_obj)
    {
        struct rt_memheap *mh = (struct rt_memheap *)mem_obj;
        total_mem += mh->pool_size;
        used_mem += (mh->pool_size - mh->available_size);
        max_used_mem += mh->max_used_size;
    }

    onenet_mqtt_upload_digit("总内存大小", total_mem);
    onenet_mqtt_upload_digit("使用的内存大小", used_mem);
    onenet_mqtt_upload_digit("内存最大使用量", max_used_mem);
}

void upload_norsize(void)
{
    struct statfs usage_stat;
    if (dfs_statfs("/flash", &usage_stat)== 0)
    {
        uint64_t total_fbsize = ((uint64_t) usage_stat.f_bsize) * ((uint64_t) usage_stat.f_blocks);
        onenet_mqtt_upload_digit("norflash总大小", total_fbsize/(1024));
        uint64_t used_fbsize = ((uint64_t) usage_stat.f_bsize) * ((uint64_t) (usage_stat.f_blocks - usage_stat.f_bfree));
        onenet_mqtt_upload_digit("norflash已使用大小", used_fbsize/(1024));
    }
}

void upload_sdsize(void)
{
    struct statfs usage_stat;
    if (dfs_statfs("/sdcard", &usage_stat)== 0)
    {
        uint64_t total_fbsize = ((uint64_t) usage_stat.f_bsize) * ((uint64_t) usage_stat.f_blocks);
        onenet_mqtt_upload_digit("SD卡总大小", total_fbsize/(1024));
        uint64_t used_fbsize = ((uint64_t) usage_stat.f_bsize) * ((uint64_t) (usage_stat.f_blocks - usage_stat.f_bfree));
        onenet_mqtt_upload_digit("SD卡已使用大小", used_fbsize/(1024));
    }
}

void upload_wifi_rssi(void)
{
    int rssi = rt_wlan_get_rssi();
    onenet_mqtt_upload_digit("WIFI信号强度", rssi);
}

/* upload random value to temperature*/
static void onenet_upload_entry(void *parameter)
{
    while (!wifi_is_ready())
    {
        rt_thread_mdelay(100);
    }
    onenet_mqtt_init();
    rt_thread_mdelay(2000);
    while (1)
    {
        upload_cpu_temperature();
        upload_mem();
        upload_norsize();
        upload_sdsize();
        upload_wifi_rssi();
        rt_thread_mdelay(1000);
    }
}

int onenet_upload_cycle(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("onenet_send",
                           onenet_upload_entry,
                           RT_NULL,
                           100 * 1024,
                           24,
                           5);
    if (tid)
    {
        rt_thread_startup(tid);
    }

    return 0;
}
INIT_APP_EXPORT(onenet_upload_cycle);

