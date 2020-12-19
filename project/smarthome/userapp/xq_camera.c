/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-13     xqyjlj       the first version
 */
#include "rtthread.h"
#include "drv_dcmi.h"
#include <dfs_posix.h>

extern volatile rt_uint32_t jpeg_data_len;
extern volatile rt_uint8_t  jpeg_data_ok;
extern rt_uint32_t *jpeg_data_buf;

void take_picture(char *path)
{
    int fd = -1;
    rt_uint32_t i, jpg_start = 0, jpg_len;
    rt_uint8_t jpg_head = 0;
    rt_uint8_t *p = RT_NULL;

    DCMI_Start();
    while (1)
    {
        while (jpeg_data_ok != 1);
        jpeg_data_ok = 2;
        while (jpeg_data_ok != 1);
        DCMI_Stop();
        p = (rt_uint8_t *) jpeg_data_buf;
        jpg_len = 0;
        jpg_head = 0;
        for (i = 0; i < jpeg_data_len * 4; i++)
        {
            /* jpg head */
            if ((p[i] == 0xFF) && (p[i + 1] == 0xD8))
            {
                jpg_start = i;
                jpg_head = 1;
            }
            /* jpg end */
            if ((p[i] == 0xFF) && (p[i + 1] == 0xD9) && jpg_head)
            {
                jpg_len = i - jpg_start + 2; /* a picture len */
                break;
            }
        }
        if (jpg_len)
        {
            p += jpg_start;
            fd = open(path, O_WRONLY | O_TRUNC);
            if (fd < 0)
            {
                rt_kprintf("open file for recording failed!\n");
                break;
            }
            else
            {
                write(fd, p, jpg_len);
                rt_kprintf("%s picture capture complate!\n", path);
                close(fd);
                break;
            }
        }
        else
        {
            rt_kprintf("jpg_len error!\n");
            break;
        }
    }
}
