/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-19     xqyjlj       the first version
 */
#include "ld3320.h"

void rt_hw_us_delay(rt_uint32_t us);

void ld3320_soft_rst(struct rt_spi_device * spidev)
{
    ld3320_write_reg(spidev, 0x17, 0x35);/* ld3320 software reset */
}
void ld3320_hw_rst(int rst_pin)
{
    rt_pin_write(rst_pin, PIN_LOW);
    rt_hw_us_delay(10);
    rt_pin_write(rst_pin, PIN_HIGH);
    rt_hw_us_delay(10);
}

void ld3320_init_common(struct rt_spi_device * spidev)
{
    ld3320_read_reg(spidev, 0x06);/* read fifo status */
    ld3320_soft_rst(spidev);/* software reset */
    rt_hw_us_delay(10);
    ld3320_read_reg(spidev, 0x06);/* read fifo status */

    ld3320_write_reg(spidev, 0x89, 0x03);/* analog circuit control initialization */
    rt_hw_us_delay(10);
    ld3320_write_reg(spidev, 0xcf, 0x43);/* internal power saving mode initialization */

    rt_hw_us_delay(10);
    ld3320_write_reg(spidev, 0xcb, 0x02);/* read ASR results (alternate 4)*/

    /*PLL setting*/
    ld3320_write_reg(spidev, 0x11, LD3320_PLL_11);/* set clock frequency 1 */

    ld3320_write_reg(spidev, 0x1e, 0x00);/* ADC control initialization */
    ld3320_write_reg(spidev, 0x19, LD3320_PLL_ASR_19);/* set clock frequency 2 */
    ld3320_write_reg(spidev, 0x1b, LD3320_PLL_ASR_1B);/* set clock frequency 3 */
    ld3320_write_reg(spidev, 0x1d, LD3320_PLL_ASR_1D);/* set clock frequency 4 */

    rt_hw_us_delay(1);
    ld3320_write_reg(spidev, 0xcd, 0x04);/* allow DSP to sleep */
    ld3320_write_reg(spidev, 0x17, 0x4c);/* DSP sleep */
    rt_hw_us_delay(10);
    ld3320_write_reg(spidev, 0xb9, 0x00);/* ASR string length initialization */
    ld3320_write_reg(spidev, 0xcf, 0x4f);/* internal power saving mode (initialize MP3 and ASR) */
    ld3320_write_reg(spidev, 0x6f, 0xff);/* initialize chip */
}

void ld3320_init_asr(struct rt_spi_device * spidev)
{
    ld3320_write_reg(spidev, 0xbd, 0x00);/* initialize the ASR controller */
    ld3320_write_reg(spidev, 0x17, 0x48);/* activate DSP */
    rt_hw_us_delay(1);
    ld3320_write_reg(spidev, 0x3c, 0x80);/* initialize FIFO EXT */
    ld3320_write_reg(spidev, 0x3e, 0x07);/* initialize FIFO EXT */
    ld3320_write_reg(spidev, 0x38, 0xff);/* initialize FIFO EXT */
    ld3320_write_reg(spidev, 0x3a, 0x07);/* initialize FIFO EXT */
    ld3320_write_reg(spidev, 0x40, 0x00);/* initialize FIFO EXT MCU */
    ld3320_write_reg(spidev, 0x42, 0x08);/* initialize FIFO EXT MCU */
    ld3320_write_reg(spidev, 0x44, 0x00);/* initialize FIFO EXT DSP */
    ld3320_write_reg(spidev, 0x46, 0x08);/* initialize FIFO EXT DSP */
    rt_hw_us_delay(1);
}

void ld3320_set_micvol(struct rt_spi_device * spidev, uint8_t micvol)
{
    ld3320_write_reg(spidev, 0x35, micvol);
}
void ld3320_set_speechEndTime(struct rt_spi_device * spidev, uint8_t speech_end_time)
{
    ld3320_write_reg(spidev, 0xb5, speech_end_time);
}
void ld3320_set_speechStartTime(struct rt_spi_device * spidev, uint8_t speech_start_time)
{
    ld3320_write_reg(spidev, 0xb4, speech_start_time);
}
void ld3320_set_speechEndpoint(struct rt_spi_device * spidev, uint8_t speech_endpoint)
{
    ld3320_write_reg(spidev, 0xb3, speech_endpoint);
}
void ld3320_set_voiceMaxLength(struct rt_spi_device * spidev, uint8_t voice_max_length)
{
    ld3320_write_reg(spidev, 0xb6, voice_max_length);
}
void ld3320_set_noiseTime(struct rt_spi_device * spidev, uint8_t noise_time)
{
    ld3320_write_reg(spidev, 0xb7, noise_time);
}
void ld3320_init_chip(struct rt_spi_device * spidev)
{
    ld3320_init_common(spidev);

    ld3320_init_asr(spidev);
    ld3320_set_micvol(spidev, LD3320_MIC_VOL);
    ld3320_set_speechEndpoint(spidev, LD3320_SPEECH_END_POINT);
    ld3320_set_speechStartTime(spidev, LD3320_SPEECH_START_TIME);
    ld3320_set_speechEndTime(spidev, LD3320_SPEECH_END_TIME);
    ld3320_set_voiceMaxLength(spidev, LD3320_VOICE_MAX_LENGTH);
    ld3320_set_noiseTime(spidev, LD3320_NOISE_TIME);
}
uint8_t ld3320_check_asrbusy_flag_b2(struct rt_spi_device * spidev)
{
    uint8_t j;
    uint8_t flag = 0;

    for (j = 0; j < 10; j++)
    {
        if (ld3320_read_reg(spidev, 0xb2) == 0x21)
        {
            flag = 1;
            break;
        }
        rt_hw_us_delay(10);
    }
    return flag;
}
void ld3320_addcommand(struct rt_spi_device * spidev, char *pass, int num)
{
    int i;
    if (ld3320_check_asrbusy_flag_b2(spidev) != 1) return;
    ld3320_write_reg(spidev, 0xc1, num); /* character number */
    ld3320_write_reg(spidev, 0xc3, 0); /* enter 00 when adding */
    ld3320_write_reg(spidev, 0x08, 0x04); /* clear FIFO_EXT */

    rt_hw_us_delay(1);
    ld3320_write_reg(spidev, 0x08, 0x00); /* need to write a 0x00 after clearing the FIFO */
    rt_hw_us_delay(1);
    for (i = 0; i <= 80; i++)
    {
        if (pass[i] == 0) break;
        ld3320_write_reg(spidev, 0x5, pass[i]); /* write FIFO_EXT */
    }
    ld3320_write_reg(spidev, 0xb9, i); /* write the length of the currently added string */
    ld3320_write_reg(spidev, 0xb2, 0xff); /* B2 all write ff */
    ld3320_write_reg(spidev, 0x37, 0x04); /* add statement */
}

uint8_t ld3320_asr_start(struct rt_spi_device * spidev)
{
    ld3320_write_reg(spidev, 0x1c, 0x09);/* ADC switch control, Reserved */
    ld3320_write_reg(spidev, 0xbd, 0x20);/* control mode  */
    ld3320_write_reg(spidev, 0x08, 0x01);/* clear FIFO DATA */
    rt_hw_us_delay(1);

    ld3320_write_reg(spidev, 0x08, 0x00);/* need to write 0x00 again after clearing the FIFO */
    rt_hw_us_delay(1);

    if (ld3320_check_asrbusy_flag_b2(spidev) == 0)
    {
        return 0;
    }

    ld3320_write_reg(spidev, 0xb2, 0xff);/* DSP busy state */
    ld3320_write_reg(spidev, 0x37, 0x06);/* notify ASR to start recognizing voice */
    rt_hw_us_delay(5);

    ld3320_write_reg(spidev, 0x1c, 0x0b);/* microphone input ADC channel */
    ld3320_write_reg(spidev, 0x29, 0x10);/* FIFO interrupt enable */
    ld3320_write_reg(spidev, 0xbd, 0x00);/* activate in ASR mode */

    return 1;
}
