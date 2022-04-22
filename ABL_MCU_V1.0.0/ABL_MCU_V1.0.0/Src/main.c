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
 * Project name: ABL_MCU_V1.0.0
 *
 * Description: Assignment
 *
 *
 * Last Changed By:  $Author: trungnt $
 * Revision:         $Revision: $
 * Last Changed:     $Date: $April 22, 2022
 *
 * Code sample:
 ******************************************************************************/
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stm32f401re_rcc.h>
#include <stm32f401re_adc.h>
#include <stm32f401re_gpio.h>
#include <stm32f401re_tim.h>
#include <timer.h>
#include <lightsensor.h>
#include <kalman_filter.h>
#include <Ucglib.h>
#include <ucg.h>
#include <typedefs.h>
/******************************************************************************/
/*                     PRIVATE TYPES and DEFINITIONS                         */
/******************************************************************************/

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define ADC_PORT				    		GPIOC
#define ADC_PIN             			    GPIO_Pin_5
#define TIME_PERIOD      			  		8399

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/
u16_t g_wAdcValue=0;
static u16_t g_wUseKalman = 0;
static u16_t g_wLightLevelAfterFilter=0;
static char_t g_ibValueLux[20] = "";
static ucg_t ucg;
/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
static void appInitCommon(void);
static void lightSensorADC_Init(void);
static void ledControlTimerOC_Init(void);
u16_t lightSensorADC_PollingRead(void);
u16_t kalmanFilter(u16_t lightLevel);
static void ledControl_TimerOCSetPwm(u32_t Compare);
static void processABL(void);
/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

/******************************************************************************/

int main(void)
{
	appInitCommon();

	while(1)
	{
		processTimerScheduler();
		processABL();
	}
}

/**
 * @func   appInitCommon
 * @brief  Application initializer
 * @param  None
 * @retval None
 */
static void appInitCommon(void)
{
	SystemCoreClockUpdate();
	TimerInit();
	lightSensorADC_Init();
	ledControlTimerOC_Init();

	KalmanFilterInit(2, 2, 0.001);

	Ucglib4WireSWSPI_begin(&ucg, UCG_FONT_MODE_SOLID);
	ucg_SetFont(&ucg, ucg_font_ncenR10_hf);
	ucg_ClearScreen(&ucg);
	ucg_SetColor(&ucg, 0, 255, 255, 255);
	ucg_SetColor(&ucg, 1, 0, 0, 0);
	ucg_SetRotate180(&ucg);

}

/**
 * @func   lightSensorADC_Init
 * @brief  Light Sensor ADC Polling initialize
 * @param  None
 * @retval None
 */
static void lightSensorADC_Init(void)
{
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable peripheral clocks ***********************************************/
	RCC_APB2PeriphClockCmd(ADCx_CLK, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = ADC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(ADC_PORT, &GPIO_InitStructure);

	/* ADC Deinit ***************************************************/
	ADC_DeInit();

	/* ADC Common Init ********************************************************/
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	/* ADC1 Init **************************************************************/
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	//Polling Configure
	ADC_EOCOnEachRegularChannelCmd(ADC1, ENABLE);
	ADC_ContinuousModeCmd(ADC1, DISABLE);
	ADC_DiscModeChannelCountConfig(ADC1, 1);
	ADC_DiscModeCmd(ADC1, ENABLE);


	/* ADC1 regular channel15 configuration ************************************/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 1, ADC_SampleTime_15Cycles);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
}

/**
 * @func   ledControlTimerOC_Init
 * @brief  Control Led with Timer Output Capture
 * @param  None
 * @retval None
 */
static void ledControlTimerOC_Init(void)
{
	GPIO_InitTypeDef 			GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef 	TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef			TIM_OC_InitStruct;


	// GPIO Configure
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_TIM1);

	//TimeBase Configure

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;

	/************************************************************************
	 * 	FPwm = Fclk / (ARR+1) * (PSC + 1)																	*
	 ************************************************************************/
	TIM_TimeBaseInitStruct.TIM_Period = TIME_PERIOD; // ARR = 8399
	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);


	//TimeIC Configure
	TIM_OC_InitStruct.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OC_InitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC_InitStruct.TIM_Pulse = 0;
	TIM_OC_InitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;

	TIM_OC4Init(TIM1, &TIM_OC_InitStruct);

	TIM_Cmd(TIM1, ENABLE);


	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

/**
 * @func   lightSensorADC_PollingRead
 * @brief  Read value from ADC
 * @param  None
 * @retval u16_t
 */
u16_t lightSensorADC_PollingRead(void)
{
	u16_t result = 0;

	ADC_SoftwareStartConv(ADCx_SENSOR);

	while (ADC_GetFlagStatus(ADCx_SENSOR, ADC_FLAG_EOC) == RESET);

	result = ADC_GetConversionValue(ADCx_SENSOR);

	return result;

}

/**
 * @func   kalmanFilter
 * @brief  Use Kanman filter for Light Sensor
 * @param  lightLevel
 * @retval u16_t
 */
u16_t kalmanFilter(u16_t lightLevel)
{
	g_wUseKalman = KalmanFilter_updateEstimate(lightLevel);

	return g_wUseKalman;
}

/**
 * @func   ledControl_TimerOCSetPwm
 * @brief  Control Led with PWM
 * @param  Compare
 * @retval None
 */
static void ledControl_TimerOCSetPwm(u32_t Compare)
{
	TIM_SetCompare4(TIM1,Compare);
}
/**
 * @func   processABL
 * @brief  AutoBrightness use Light Sensor
 * @param  None
 * @retval None
 */
static void processABL(void)
{
	u32_t dwTimeCurrent;
	static u32_t dwTimeTotal, dwTimeInit;

	dwTimeCurrent = GetMilSecTick();

	if(dwTimeCurrent >= dwTimeInit)
	{
		dwTimeTotal += dwTimeCurrent - dwTimeInit;
	}
	else
	{
		dwTimeTotal += 0xFFFFFFFFU - dwTimeCurrent + dwTimeInit;
	}

	if(dwTimeTotal >= 100)
	{
		dwTimeTotal = 0;
		g_wAdcValue  = lightSensorADC_PollingRead(); //Đọc giá trị ADC đo được từ cảm biến ánh sáng
		g_wLightLevelAfterFilter = kalmanFilter(g_wAdcValue);


		ledControl_TimerOCSetPwm(g_wLightLevelAfterFilter);
		memset(g_ibValueLux, 0, sizeof(g_ibValueLux));
		sprintf(g_ibValueLux, " Light = %d lux  ", g_wLightLevelAfterFilter);
		ucg_DrawString(&ucg, 0, 72, 0, g_ibValueLux);
	}
	dwTimeInit = dwTimeCurrent;
}
