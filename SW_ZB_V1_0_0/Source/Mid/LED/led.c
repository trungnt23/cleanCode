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
 * File name: LED.c
 *
 * Description: process led
 *
 *
 * Last Changed By:  $Author: trungnt $
 * Revision:         $Revision: $
 * Last Changed:     $Date: $April 27, 2022
 *
 ******************************************************************************/
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include "app/framework/include/af.h"
#include "em_timer.h"
#include "led.h"
/******************************************************************************/
/*                     PRIVATE TYPES and DEFINITIONS                         */
/******************************************************************************/


/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/


/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

EmberEventControl led1ToggleEventControl,led2ToggleEventControl;
EmberEventControl *ledEventControl[LED_RGB_COUNT];

LedArray_t g_ledArray[LED_RGB_COUNT][LED_RGB_ELEMENT] = {LED_RGB_1, LED_RGB_2};
LedArray_t g_ledAction[LED_RGB_COUNT];
/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
static void_t toggleLedHandle(LedNumber ledIndex);
/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

/******************************************************************************/








/**
 * @func    ledInit
 * @brief   Initialize LED
 * @param   None
 * @retval  None
 */
void_t ledInit(void_t)
{
	CMU_ClockEnable(cmuClock_GPIO, true);
	for(int i = 0;i <LED_RGB_COUNT;i++)
	{
		for(int j = 0; j< LED_RGB_ELEMENT;j++)
		{
			GPIO_PinModeSet(g_ledArray[i][j].port, g_ledArray[i][j].pin,
							gpioModePushPull,0);
		}
	}
	turnOffRBGLed(LED1);
	turnOffRBGLed(LED2);
	ledEventControl[LED1] =(EmberEventControl *) &led1ToggleEventControl;
	ledEventControl[LED2] =(EmberEventControl *) &led2ToggleEventControl;
}


/**
 * @func    turnOffRBGLed
 * @brief   Turn off number led
 * @param   LedNumber
 * @retval  None
 */
void_t turnOffRBGLed(LedNumber index)
{
	for(int j=0;j<LED_RGB_ELEMENT;j++)
	{
		GPIO_PinOutSet(g_ledArray[index][j].port, g_ledArray[index][j].pin);
	}
}

/**
 * @func    turnOnLed
 * @brief   Turn on led
 * @param   LedNumber, LedColor
 * @retval  None
 */
void_t turnOnLed(LedNumber index, LedColor color)
{
	for(int j=0;j<LED_RGB_ELEMENT;j++)
	{
		if(((color >> j) & 0x01) == LED_SET_ON)
		{
			GPIO_PinOutClear(g_ledArray[index][j].port, g_ledArray[index][j].pin);
		}
		else{
			GPIO_PinOutSet(g_ledArray[index][j].port, g_ledArray[index][j].pin);
		}
	}
}

/**
 * @func    toggleLed
 * @brief   toggled LED
 * @param   LedNumber, LedColor, toggleTime, onTimeMs, offTimeMs
 * @retval  None
 */
void_t toggleLed(LedNumber ledIndex, LedColor color, u8_t toggleTime, u32_t onTimeMs, u32_t offTimeMs)
{
	g_ledAction[ledIndex].ledBlinkMode = LED_TOGGLE;
	g_ledAction[ledIndex].color = color;
	g_ledAction[ledIndex].onTime = onTimeMs;
	g_ledAction[ledIndex].offTime = offTimeMs;
	g_ledAction[ledIndex].blinkTime = toggleTime*2;

	emberEventControlSetActive(*ledEventControl[ledIndex]);
}

/**
 * @func    toggleLedHandle
 * @brief   Event Led Handler
 * @param   LedNumber
 * @retval  None
 */
static void_t toggleLedHandle(LedNumber ledIndex)
{
	if(g_ledAction[ledIndex].blinkTime !=0)
	{
		if(g_ledAction[ledIndex].blinkTime % 2)
		{
			for( int i = 0; i<LED_RGB_ELEMENT; i++)
			{
				if(((g_ledAction[ledIndex].color >> i ) & 0x01) == 1 )
				{
					GPIO_PinOutClear(g_ledArray[ledIndex][i].port, g_ledArray[ledIndex][i].pin);
				}
				else
				{
					GPIO_PinOutSet(g_ledArray[ledIndex][i].port, g_ledArray[ledIndex][i].pin);
				}
			}
			emberEventControlSetDelayMS(*ledEventControl[ledIndex], g_ledAction[ledIndex].onTime);
		}else
		{
			for( int j = 0; j<LED_RGB_ELEMENT; j++)
			{
				GPIO_PinOutSet(g_ledArray[ledIndex][j].port, g_ledArray[ledIndex][j].pin);
			}
			emberEventControlSetDelayMS(*ledEventControl[ledIndex], g_ledAction[ledIndex].offTime);
		}
		g_ledAction[ledIndex].blinkTime--;
	}
	else
	{
		g_ledAction[ledIndex].ledBlinkMode = LED_FREE;
		for( int j = 0; j<LED_RGB_ELEMENT; j++)
		{
			GPIO_PinOutSet(g_ledArray[ledIndex][j].port, g_ledArray[ledIndex][j].pin);
		}
	}
}

/**
 * @func    led1ToggleEventHandle
 * @brief   Event Led Handler
 * @param   None
 * @retval  None
 */
void_t led1ToggleEventHandle(void_t)
{
	emberEventControlSetInactive(led1ToggleEventControl);
	switch(g_ledAction[LED1].ledBlinkMode)
	{
	case LED_TOGGLE:
		toggleLedHandle(LED1);
		break;
	default:
		break;
	}
}


/**
 * @func    led2ToggleEventHandle
 * @brief   Event Led Handler
 * @param   None
 * @retval  None
 */
void_t led2ToggleEventHandle(void_t)
{
	emberEventControlSetInactive(led2ToggleEventControl);
	switch(g_ledAction[LED2].ledBlinkMode)
	{
	case LED_TOGGLE:
		toggleLedHandle(LED2);
		break;
	default:
		break;
	}
}
