/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-28     xqyjlj       the first version
 */
#include "rtthread.h"
#include "drv_common.h"
#include "string.h"

#define MSG_DATA_LEN 200

#define DBG_TAG "ir"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim5;

static rt_sem_t dynamic_sem = RT_NULL;
static uint16_t msg_data[MSG_DATA_LEN] = { 0 };
static uint16_t msg_sta = 0;
static uint16_t msg_sta_tmp = 0;


void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
static int MX_TIM3_Init(void)
{

    /* USER CODE BEGIN TIM3_Init 0 */

    /* USER CODE END TIM3_Init 0 */

    TIM_MasterConfigTypeDef sMasterConfig = { 0 };
    TIM_IC_InitTypeDef sConfigIC = { 0 };

    /* USER CODE BEGIN TIM3_Init 1 */

    /* USER CODE END TIM3_Init 1 */
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 239;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 40000;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_IC_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter = 3;
    if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM3_Init 2 */

    /* USER CODE END TIM3_Init 2 */

    return RT_EOK;
}

void TIM3_IRQHandler(void)
{
    /* USER CODE BEGIN TIM3_IRQn 0 */

    /* USER CODE END TIM3_IRQn 0 */
    HAL_TIM_IRQHandler(&htim3);
    /* USER CODE BEGIN TIM3_IRQn 1 */

    /* USER CODE END TIM3_IRQn 1 */
}

INIT_BOARD_EXPORT(MX_TIM3_Init);

static int MX_TIM5_Init(void)
{

    /* USER CODE BEGIN TIM5_Init 0 */

    /* USER CODE END TIM5_Init 0 */

    TIM_MasterConfigTypeDef sMasterConfig = { 0 };
    TIM_OC_InitTypeDef sConfigOC = { 0 };

    /* USER CODE BEGIN TIM5_Init 1 */

    /* USER CODE END TIM5_Init 1 */
    htim5.Instance = TIM5;
    htim5.Init.Prescaler = 239;
    htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim5.Init.Period = 26;
    htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_PWM_Init(&htim5) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 18;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM5_Init 2 */

    /* USER CODE END TIM5_Init 2 */
    HAL_TIM_MspPostInit(&htim5);

    HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);
    return RT_EOK;
}

INIT_BOARD_EXPORT(MX_TIM5_Init);

void ir_it_update(void)
{
    if (dynamic_sem != RT_NULL)
    {
        rt_sem_release(dynamic_sem); /* 释放二值信号量 */
    }

    msg_sta = msg_sta_tmp;
    msg_sta_tmp = 0;
}

void ir_ic_update(void)
{
    uint16_t val;
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0))
    {
        val = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_3);
        TIM_RESET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_3);
        TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_3, TIM_ICPOLARITY_FALLING);
        __HAL_TIM_SET_COUNTER(&htim3, 0);
        if (msg_sta_tmp < MSG_DATA_LEN)
        {
            msg_data[msg_sta_tmp++] = val; /* 获得低电平时间 */
        }
    }
    else
    {
        val = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_3);
        TIM_RESET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_3);
        TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_3, TIM_ICPOLARITY_RISING);
        __HAL_TIM_SET_COUNTER(&htim3, 0);
        if (msg_sta_tmp < MSG_DATA_LEN)
        {
            msg_data[msg_sta_tmp] |= 0x8000; /* 标记高电平 */
            msg_data[msg_sta_tmp++] |= val; /* 获得高电平时间 */
        }
    }
//    rt_kprintf("%d ",val);
}


void ir_send_thread(void * parameter)
{
    while(1)
    {
//        rt_enter_critical();
//        HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);
//        rt_exit_critical();
        rt_thread_mdelay(20);
//        rt_enter_critical();
//        HAL_TIM_PWM_Stop(&htim5, TIM_CHANNEL_1);
//        rt_exit_critical();
        rt_thread_mdelay(20);
    }
}

void ir_rev_thread(void * parameter)
{
    while(1)
    {
        rt_sem_take(dynamic_sem, RT_WAITING_FOREVER);/* 获取信号量 */
        if(msg_sta)
        {
            uint16_t *msg = RT_NULL;
            uint16_t msg_len = msg_sta;
            msg = rt_malloc(MSG_DATA_LEN);
            memcpy(msg, msg_data, MSG_DATA_LEN);
            memset(msg_data, 0, MSG_DATA_LEN);

            for (int i = 0; i < msg_len; i++)
            {
                rt_kprintf("%d,", msg[i]);
            }
            rt_kprintf("\r\n");
            rt_free(msg);
            msg = RT_NULL;
        }
    }
}


int init_ir(void)
{
    HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_3);
    __HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE);

    dynamic_sem = rt_sem_create("ir", 0, RT_IPC_FLAG_FIFO);
    if (dynamic_sem == RT_NULL)
    {
        LOG_E("ir sem create fail");
        return RT_ERROR;
    }

    rt_thread_t thread = RT_NULL;

    thread = rt_thread_create("ir_rev", ir_rev_thread, RT_NULL, 500, 14, 100);
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }

    thread = rt_thread_create("ir_send", ir_send_thread, RT_NULL, 500, 14, 100);
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    return RT_EOK;
}

INIT_APP_EXPORT(init_ir);
