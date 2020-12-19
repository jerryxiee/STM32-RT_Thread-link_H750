/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-19     xqyjlj       the first version
 */
#ifndef _LD3320_H_
#define _LD3320_H_

#include <rtdevice.h>
#include "ld3320_base.h"

#define LD3320_CLK_IN                   22/* LD3320 chip input frequency */

#define LD3320_PLL_11                   (uint8_t)((LD3320_CLK_IN/2.0)-1)


#define LD3320_PLL_ASR_19               (uint8_t)(LD3320_CLK_IN*32.0/(LD3320_PLL_11+1) - 0.51)
#define LD3320_PLL_ASR_1B               0x48
#define LD3320_PLL_ASR_1D               0x1f

#define LD3320_SPEECH_END_POINT         0x22/* Adjust the end time of the voice endpoint (interval time of utterance), parameter (0x00~0xC3, unit 10MS) */
#define LD3320_SPEECH_START_TIME        0x0f/* Adjust the start time of the voice endpoint, parameters (0x00~0x30, unit 10MS) */
#define LD3320_SPEECH_END_TIME          0x3c/* Adjust the end time of the voice endpoint (interval time of utterance), parameter (0x00~0xC3, unit 10MS) */
#define LD3320_VOICE_MAX_LENGTH         0x3c/* The longest voice segment time, parameter (0x00~0xC3, unit 100MS) */
#define LD3320_NOISE_TIME               0x02/* Skip the power-on noise, parameter (0x00~0xff, unit 20MS) */
#define LD3320_MIC_VOL                  0x40/* Adjust ADC gain, parameters (0x00~0xFF, recommended 10-60) */

#define LD3320_MAX_COMMAND_LEN          20/* the max command len. unit: byte */

#define LD3320_MIX2SPVOLUME             3/* the max speaker volume */


void ld3320_soft_rst(struct rt_spi_device * spidev);
void ld3320_hw_rst(int rst_pin);
void ld3320_init_common(struct rt_spi_device * spidev);
void ld3320_init_asr(struct rt_spi_device * spidev);
void ld3320_init_chip(struct rt_spi_device * spidev);
void ld3320_addcommand(struct rt_spi_device * spidev, char *pass, int num);
uint8_t ld3320_asr_start(struct rt_spi_device * spidev);
#endif /* LD3320_LD3320_H_ */
