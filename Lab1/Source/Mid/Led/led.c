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
 * File name: Example.h
 *
 * Description:
 *
 *
 * Last Changed By:  $Author: trungnt $
 * Revision:         $Revision: $
 * Last Changed:     $Date: $April 15, 2022
 *
 * Code sample:
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
EmberEventControl led1ToggleEventControl,led2ToggleEventControl;
EmberEventControl *pLedEventControl[LED_RGB_COUNT];

ledArray_t g_led_Array[LED_RGB_COUNT][LED_RGB_ELEMENT] = {LED_RGB_1, LED_RGB_2};
ledArray_t g_ledAction[LED_RGB_COUNT];
/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
static void toggleLedHandle(ledNumber ledIndex);
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
void ledInit(void)
{
	CMU_ClockEnable(cmuClock_GPIO, true);
	for(int i = 0;i <LED_RGB_COUNT;i++)
	{
		for(int j = 0; j< LED_RGB_ELEMENT;j++)
		{
			GPIO_PinModeSet(g_led_Array[i][j].port, g_led_Array[i][j].pin,
							gpioModePushPull,0);
		}
	}
	turnOffLedRBG(LED1);
	turnOffLedRBG(LED2);
	pLedEventControl[LED1] =(EmberEventControl *) &led1ToggleEventControl;
	pLedEventControl[LED2] =(EmberEventControl *) &led2ToggleEventControl;
}

/**
 * @func    GetHumi_Sensor
 * @brief   Get value humidity
 * @param   None
 * @retval  Humidity
 */
void turnOffLedRBG(ledNumber index)
{
	for(int j=0;j<LED_RGB_ELEMENT;j++)
	{
		GPIO_PinOutSet(g_led_Array[index][j].port, g_led_Array[index][j].pin);
	}
}

/**
 * @func    toggleLed
 * @brief   toggled LED
 * @param   None
 * @retval  None
 */
void toggleLed(ledNumber ledIndex, ledColor_e color, uint8_t toggleTime, uint32_t onTimeMs, uint32_t offTimeMs)
{
	g_ledAction[ledIndex].ledBlinkMode = LED_TOGGLE;
	g_ledAction[ledIndex].color = color;
	g_ledAction[ledIndex].onTime = onTimeMs;
	g_ledAction[ledIndex].offTime = offTimeMs;
	g_ledAction[ledIndex].blinkTime = toggleTime*2;

	emberEventControlSetActive(*pLedEventControl[ledIndex]);
}

/**
 * @func    toggleLedHandle
 * @brief   Event Led Handler
 * @param   None
 * @retval  None
 */
static void toggleLedHandle(ledNumber ledIndex)
{
	if(g_ledAction[ledIndex].blinkTime !=0)
	{
		if(g_ledAction[ledIndex].blinkTime % 2)
		{
			for( int i = 0; i<LED_RGB_ELEMENT; i++)
			{
				if(((g_ledAction[ledIndex].color >> i ) & 0x01) == 1 )
				{
					GPIO_PinOutClear(g_led_Array[ledIndex][i].port, g_led_Array[ledIndex][i].pin);
				}
				else
				{
					GPIO_PinOutSet(g_led_Array[ledIndex][i].port, g_led_Array[ledIndex][i].pin);
				}
			}
			emberEventControlSetDelayMS(*pLedEventControl[ledIndex], g_ledAction[ledIndex].onTime);
		}else
		{
			for( int j = 0; j<LED_RGB_ELEMENT; j++)
			{
				GPIO_PinOutSet(g_led_Array[ledIndex][j].port, g_led_Array[ledIndex][j].pin);
			}
			emberEventControlSetDelayMS(*pLedEventControl[ledIndex], g_ledAction[ledIndex].offTime);
		}
		g_ledAction[ledIndex].blinkTime--;
	}
	else
	{
		g_ledAction[ledIndex].ledBlinkMode = LED_FREE;
		for( int j = 0; j<LED_RGB_ELEMENT; j++)
		{
			GPIO_PinOutSet(g_led_Array[ledIndex][j].port, g_led_Array[ledIndex][j].pin);
		}
	}
}

/**
 * @func    led1ToggleEventHandle
 * @brief   Event Led Handler
 * @param   None
 * @retval  None
 */
void led1ToggleEventHandle(void)
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
void led2ToggleEventHandle(void)
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


