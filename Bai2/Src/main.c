/*******************************************************************************
 *				 _ _                                             _ _
				|   |                                           (_ _)
				|   |        _ _     _ _   _ _ _ _ _ _ _ _ _ _   _ _
				|   |       |   |   |   | |    _ _     _ _    | |   |
				|   |       |   |   |   | |   |   |   |   |   | |   |
				|   |       |   |   |   | |   |   |   |   |   | |   |
				|   |_ _ _  |   |_ _|   | |   |   |   |   |   | |   |
				|_ _ _ _ _| |_ _ _ _ _ _| |_ _|   |_ _|   |_ _| |_ _|
								(C)2022 Lumi
 * Copyright (c) 2022
 * Lumi, JSC.
 * All Rights Reserved
 *
 * File name: main.h
 *
 * Description:
 *
 *
 * Last Changed By:  $Author: trungnt $
 * Revision:         $Revision: $
 * Last Changed:     $Date: $April 23, 2022
 *
 * Code sample:
 ******************************************************************************/
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include <stdint.h>
#include <stm32f401re_rcc.h>
#include <stm32f401re_tim.h>
#include <stm32f401re_gpio.h>
#include <stm32f401re_usart.h>
#include <timer.h>
#include <misc.h>
#include <typedefs.h>
/******************************************************************************/
/*                     PRIVATE TYPES and DEFINITIONS                         */
/******************************************************************************/


/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
#define USART2_TX									GPIO_Pin_2
#define TIME_LIMIT_SEND_DATA						2000 // ms
#define HIGH_STATUS									1
#define LOW_STATUS										1

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/
u32_t g_dwNumberPress = 0;
u32_t g_dwTimRising = 0;
u8_t g_byStatus = 0;
/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
static void_t appInitCommon(void_t);
static void_t timer_InitCapture(void_t);
static void_t checkTimePress(void_t);
static void_t sendNumberPress(void_t);
static void_t USART2_Init(void_t);
/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

/******************************************************************************/

int main(void_t)
{
	appInitCommon();
	while(1)
		{
			sendNumberPress();
			processTimerScheduler();
		}
}

/*
 * @func   appInitCommon
 * @brief  Initialize peripheral
 * @param  None
 * @retval None
 */
static void_t appInitCommon(void_t)
{
	SystemCoreClockUpdate();
	TimerInit();
	USART2_Init();
	timer_InitCapture();
}


/*
 * @func   timer_InitCapture
 * @brief  Initialize timer mode inputcapture
 * @param  None
 * @retval None
 */
static void_t timer_InitCapture(void_t)
{
	GPIO_InitTypeDef 			GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef 	TIM_TimeBaseInitStruct;
	TIM_ICInitTypeDef			TIM_ICInitStruct;
	NVIC_InitTypeDef			NVIC_InitStruct;

	// GPIO Configure
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_TIM2);

	//TimeBase Configure

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 41999;
	TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);


	//TimeIC Configure

	TIM_ICInitStruct.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStruct.TIM_ICFilter = 0x0;
	TIM_ICInit(TIM2, &TIM_ICInitStruct);

	TIM_Cmd(TIM2, ENABLE);

	// Interrupt Configure

	TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);

	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

}


/*
 * @func   TIM2_IRQHandler
 * @brief  Timer interrupt
 * @param  None
 * @retval None
 */
void_t TIM2_IRQHandler(void_t)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)
	{
		checkTimePress();
	}
	TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
}


/*
 * @func   checkTimePress
 * @brief  Check number when press button
 * @param  None
 * @retval None
 */
static void_t checkTimePress(void_t)
{
	g_byStatus = 0;
	g_byStatus = !g_byStatus;

	if(g_byStatus == HIGH_STATUS)
	{
		g_dwNumberPress++;
	}
	else if(g_byStatus == LOW_STATUS)
	{
		g_dwTimRising = TIM_GetCapture2(TIM2);
		g_byStatus = HIGH_STATUS;
	}

}

/*
 * @func   sendNumberPress
 * @brief  Send number to PC
 * @param  None
 * @retval None
 */

static void_t sendNumberPress(void_t)
{
	u32_t dwTimeCurrent;
	static u32_t dwTimeTotal, dwTimeInit;
	if(g_byStatus == HIGH_STATUS)
	{


		dwTimeCurrent = GetMilSecTick();

		if(dwTimeCurrent >= dwTimeInit)
		{
			dwTimeTotal += dwTimeCurrent - dwTimeInit;
		}
		else
		{
			dwTimeTotal += 0xFFFFFFFFU - dwTimeCurrent + dwTimeInit;
		}

		if(dwTimeTotal >= TIME_LIMIT_SEND_DATA)
		{
			dwTimeTotal = 0;
			USART_SendData(USART2, g_dwNumberPress);
			g_byStatus = LOW_STATUS;
			g_dwNumberPress = 0;
		}
		dwTimeInit = dwTimeCurrent;
	}
}

/**
 * @func   USART2_Init
 * @brief  Initialize USART2
 * @param  None
 * @retval None
 */
static void_t USART2_Init(void_t)
{
	GPIO_InitTypeDef 					GPIO_InitStruct;
	USART_InitTypeDef					USART_InitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);

	//Enable USART

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	USART_InitStruct.USART_BaudRate = 9600;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;

	USART_Init(USART2, &USART_InitStruct);

	USART_Cmd(USART2, ENABLE);
}

