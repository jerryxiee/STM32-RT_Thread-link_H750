/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-13     xqyjlj       the first version
 */
#include "drv_spi.h"

#define DBG_TAG "ld3320"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include "../ld3320/ld3320.h"
#include "wifi.h"

#define LD3320_RST_PIN  GET_PIN(H, 2)
#define LD3320_IRQ_PIN  GET_PIN(H, 3)
struct rt_spi_device *spi_dev_ld3320;

extern void mp3_play_song(char *path);
extern void VS_Sine_Test(void);
void take_picture(char *path);
void la3320_command(void)
{
    ld3320_addcommand(spi_dev_ld3320, "a ti a ti", 1);
    ld3320_addcommand(spi_dev_ld3320, "a ", 2);
    ld3320_addcommand(spi_dev_ld3320, "ti", 2);
    ld3320_addcommand(spi_dev_ld3320, "bo fang ge qu", 3);
    ld3320_addcommand(spi_dev_ld3320, "da kai lan deng", 4);
    ld3320_addcommand(spi_dev_ld3320, "guan bi lan deng", 5);
    ld3320_addcommand(spi_dev_ld3320, "gan bi lan deng", 5);
    ld3320_addcommand(spi_dev_ld3320, "da kai hong deng", 6);
    ld3320_addcommand(spi_dev_ld3320, "guan bi hong deng", 7);
    ld3320_addcommand(spi_dev_ld3320, "gan bi hong deng", 7);
    ld3320_addcommand(spi_dev_ld3320, "fa song you jian", 8);
    ld3320_addcommand(spi_dev_ld3320, "pai she zhao pian", 9);
    ld3320_addcommand(spi_dev_ld3320, "ha", 20);
    ld3320_addcommand(spi_dev_ld3320, "a", 20);
    ld3320_addcommand(spi_dev_ld3320, "wo cao", 20);
    ld3320_addcommand(spi_dev_ld3320, "kao", 20);
    ld3320_addcommand(spi_dev_ld3320, "di", 20);
    ld3320_addcommand(spi_dev_ld3320, "kan", 20);
    ld3320_addcommand(spi_dev_ld3320, "pa", 20);
    ld3320_addcommand(spi_dev_ld3320, "giao", 20);
    ld3320_addcommand(spi_dev_ld3320, "duang", 20);
    ld3320_addcommand(spi_dev_ld3320, "e", 20);
    ld3320_addcommand(spi_dev_ld3320, "zhe", 20);
}

static void ld3320_asr_over_callback(uint8_t num)
{
    switch (num)
    {
    case 1:
        mp3_play_song("/sdcard/aixia/wozai.mp3");
        break;
    case 3:
        rt_enter_critical();
        mp3_play_song("/sdcard/music/1.mp3");
        rt_exit_critical();
        break;
    case 4:
        rt_pin_write(GET_PIN(I, 8), 0);
        mp3_play_song("/sdcard/aixia/yidakai.mp3");
        break;
    case 5:
        rt_pin_write(GET_PIN(I, 8), 1);
        mp3_play_song("/sdcard/aixia/yiguanbi.mp3");
        break;
    case 6:
        rt_pin_write(GET_PIN(C, 15), 0);
        mp3_play_song("/sdcard/aixia/yidakai.mp3");
        break;
    case 7:
        rt_pin_write(GET_PIN(C, 15), 1);
        mp3_play_song("/sdcard/aixia/yiguanbi.mp3");
        break;
    case 8:
        mp3_play_song("/sdcard/aixia/yifasong.mp3");
        break;
    case 9:
        rt_enter_critical();
        take_picture("/sdcard/jpg/1.jpg");
        rt_exit_critical();
        mp3_play_song("/sdcard/aixia/yipaizhao.mp3");
        break;

    default:
        break;
    }
}

static void ld3320_asr_thread(void * parameter)
{
    rt_thread_mdelay(5000);
    rt_hw_spi_device_attach("spi4", "spi40", GPIOA, GPIO_PIN_11);
    spi_dev_ld3320=(struct rt_spi_device *)rt_device_find("spi40");

    struct rt_spi_configuration cfg;
    cfg.data_width = 8;
    cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_2 | RT_SPI_MSB;
    cfg.max_hz = 1 * 1000 * 1000;
    rt_spi_configure(spi_dev_ld3320, &cfg);

    rt_pin_mode(LD3320_RST_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(GET_PIN(I, 8), PIN_MODE_OUTPUT);
    rt_pin_mode(GET_PIN(C, 15), PIN_MODE_OUTPUT);
    rt_pin_write(LD3320_RST_PIN, PIN_HIGH);

    rt_pin_mode(LD3320_IRQ_PIN, PIN_MODE_INPUT);

    ld3320_hw_rst(LD3320_RST_PIN);
    ld3320_init_chip(spi_dev_ld3320);
    la3320_command();
    ld3320_asr_start(spi_dev_ld3320);
    while (1)
    {
        if (rt_pin_read(LD3320_IRQ_PIN) == 0)
        {
            uint8_t Asr_Count = 0;
            uint8_t num = 0;
            /* if speech recognition is interrupted, DSP is idle, ASR ends normally */
            if ((ld3320_read_reg(spi_dev_ld3320, 0x2b) & 0x10) && ld3320_read_reg(spi_dev_ld3320, 0xb2) == 0x21
                    && ld3320_read_reg(spi_dev_ld3320, 0xbf) == 0x35)
            {
                ld3320_write_reg(spi_dev_ld3320, 0x29, 0); /* turn off interrupt */
                ld3320_write_reg(spi_dev_ld3320, 0x02, 0); /* turn off FIFO interrupt */
                Asr_Count = ld3320_read_reg(spi_dev_ld3320, 0xba); /* read interrupt auxiliary information */
                /* if there is a recognition result */
                if (Asr_Count > 0 && Asr_Count < 4)
                {
                    num = ld3320_read_reg(spi_dev_ld3320, 0xc5);
                    ld3320_asr_over_callback(num);
                }
                ld3320_write_reg(spi_dev_ld3320, 0x2b, 0); /* clear interrupt number */
                ld3320_write_reg(spi_dev_ld3320, 0x1C, 0); /* turn off the microphone */
                ld3320_write_reg(spi_dev_ld3320, 0x29, 0); /* turn off interrupt */
                ld3320_write_reg(spi_dev_ld3320, 0x02, 0); /* turn off FIFO interrupt */
                ld3320_write_reg(spi_dev_ld3320, 0x2B, 0);
                ld3320_write_reg(spi_dev_ld3320, 0xBA, 0);
                ld3320_write_reg(spi_dev_ld3320, 0xBC, 0);
                ld3320_write_reg(spi_dev_ld3320, 0x08, 1); /* clear FIFO_DATA */
                ld3320_write_reg(spi_dev_ld3320, 0x08, 0); /* after clearing FIFO_DATA, write 0 again */
            }
            ld3320_hw_rst(LD3320_RST_PIN);

            ld3320_init_chip(spi_dev_ld3320);

            la3320_command();
            ld3320_asr_start(spi_dev_ld3320);
        }

        rt_thread_mdelay(100);
    }
}

static int create_ld3320_asr_thread(void)
{

    rt_thread_t thread = RT_NULL;
    thread = rt_thread_create("ld_asr", ld3320_asr_thread, RT_NULL, 100*1024,    25, 100);
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    return RT_EOK;
}

INIT_APP_EXPORT(create_ld3320_asr_thread);
