/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-17     xqyjlj       the first version
 */
#include "rtthread.h"
#include "vs1053.h"
#include <dfs_posix.h>

void mp3_play_song(char *path)
{
    FILE *fp = RT_NULL;
    VS_Restart_Play();
    VS_Set_All();
    VS_Reset_DecodeTime();
    fp = fopen(path, "r");
    if (fp)
    {
        VS_SPI_SpeedHigh();
        uint8_t *buf = RT_NULL;
        buf = rt_malloc(1024 * 10);
        if (buf)
        {
            int readsize = fread(buf, 1, 1024 * 10, fp);
            if (readsize < 1024 * 10 && readsize > 0)
            {
                uint32_t i = 0;
                do
                {
                    if (VS_Send_MusicData(buf + i) == 0)
                    {
                        i += 32;
                    }
                } while (i <readsize);
            }
            while (readsize == 1024 * 10)
            {
                if (readsize > 0)
                {
                    uint32_t i = 0;
                    do
                    {
                        if (VS_Send_MusicData(buf + i) == 0)
                        {
                            i += 32;
                        }
                    } while (i < readsize);
                }
                readsize = fread(buf, 1, 1024 * 10, fp);
            }
            fclose(fp);

        }
        rt_free(buf);
    }
}
