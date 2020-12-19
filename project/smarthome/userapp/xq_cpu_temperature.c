/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-19     xqyjlj       the first version
 */

#include "rtthread.h"
#include "drv_common.h"
ADC_HandleTypeDef hadc3;



/**
 * @brief ADC3 Initialization Function
 * @param None
 * @retval None
 */
static int MX_ADC3_Init(void)
{
    hadc3.Instance = ADC3;
    hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc3.Init.Resolution = ADC_RESOLUTION_16B;
    hadc3.Init.ScanConvMode = DISABLE;
    hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc3.Init.LowPowerAutoWait = DISABLE;
    hadc3.Init.ContinuousConvMode = DISABLE;
    hadc3.Init.NbrOfConversion = 1;
    hadc3.Init.DiscontinuousConvMode = DISABLE;
    hadc3.Init.NbrOfDiscConversion = 0;
    hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc3.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
    hadc3.Init.OversamplingMode = DISABLE;
    hadc3.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
    HAL_ADC_Init(&hadc3);

    HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);

    return RT_EOK;
}

INIT_BOARD_EXPORT(MX_ADC3_Init);

/*
 * 获得ADC值
 * ch: 通道值 0~16，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
 * 返回值:转换结果
 */
uint16_t Get_Adc(ADC_HandleTypeDef ADCx, uint32_t ch)
{
    ADC_ChannelConfTypeDef ADC_ChanConf;

    ADC_ChanConf.Channel = ch; /* 通道 */
    ADC_ChanConf.Rank = ADC_REGULAR_RANK_1; /* 1个序列 */
    ADC_ChanConf.SamplingTime = ADC_SAMPLETIME_810CYCLES_5; /* 采样时间 */
    ADC_ChanConf.SingleDiff = ADC_SINGLE_ENDED; /* 单边采集 */
    ADC_ChanConf.OffsetNumber = ADC_OFFSET_NONE;
    ADC_ChanConf.Offset = 0;
    HAL_ADC_ConfigChannel(&ADCx, &ADC_ChanConf); /* 通道配置 */

    HAL_ADC_Start(&ADCx); /* 开启ADC */

    HAL_ADC_PollForConversion(&ADCx, 10); /* 轮询转换 */
    return (uint16_t) HAL_ADC_GetValue(&ADCx); /* 返回最近一次ADC规则组的转换结果 */
}
/*
 * 获取指定通道的转换值，取times次,然后平均
 * times:获取次数
 * 返回值:通道ch的times次转换结果平均值
 */
uint16_t Get_Adc_Average(ADC_HandleTypeDef ADCx, uint32_t ch, uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;
    for (t = 0; t < times; t++)
    {
        temp_val += Get_Adc(ADCx, ch);
        rt_thread_mdelay(5);
    }
    return temp_val / times;
}

/* 得到温度值
 * 返回值:温度值(扩大了100倍,单位:℃.)
 */
short Get_Temprate(void)
{
    uint32_t adcx;
    short result;
    double temperate;
    float temp = 0;
    uint16_t ts_cal1, ts_cal2;

    ts_cal1 = *(__IO uint16_t *) (0X1FF1E820);
    ts_cal2 = *(__IO uint16_t *) (0X1FF1E840);
    temp = (float) ((110.0f - 30.0f) / (ts_cal2 - ts_cal1));
    adcx = Get_Adc_Average(hadc3, ADC_CHANNEL_TEMPSENSOR, 10); /* 读取内部温度传感器通道,10次取平均 */
    temperate = (float) (temp * (adcx - ts_cal1) + 30);
    result = temperate *= 100; /* 扩大100倍. */
    return result;
}
