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
 * File name: button.c
 *
 * Description: develop process button
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
#include <stddef.h>
#include "em_cmu.h"
#include "em_gpio.h"
#include "gpiointerrupt.h"
#include "button.h"
/******************************************************************************/
/*                     PRIVATE TYPES and DEFINITIONS                         */
/******************************************************************************/


/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/


/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/
buttonHoldingEvent g_pHoldingCallbackFunc = NULL;
buttonPressAndHoldEvent g_pPressAndHoldingCallbackFunc = NULL;
ButtonConfig_t g_buttonArray[BUTTON_COUNT] = BUTTON_INIT;
/******************************************************************************/
/*                              EVENT DATA                                  */
/******************************************************************************/
EmberEventControl buttonPressAndHoldEventControl;
EmberEventControl buttonReleaseEventControl;

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
void_t buttonPressAndHoldEventHandle(void_t);
void_t buttonReleaseEventHandle(void_t);
static void_t halInternalButtonIsr(u8_t byPin);
static u8_t getButtonIndex(u8_t byPin);
static void_t resetButtonParameter(u8_t byNumID);

/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

/******************************************************************************/

/*
 * @func	buttonInit
 * @brief	Initialize Button
 * @param	holdingHandle, pressAndHoldeHandler
 * @retval	None
 */
void_t buttonInit(buttonHoldingEvent holdingHandle,buttonPressAndHoldEvent pressAndHoldeHandler)
{
  GPIOINT_Init();

  CMU_ClockEnable(cmuClock_GPIO, true);
  u8_t i;
  for ( i = 0; i < BUTTON_COUNT; i++ ) {
    /* Configure pin as input */
    GPIO_PinModeSet(g_buttonArray[i].port,
                    g_buttonArray[i].pin,
					gpioModeInput,
					GPIO_DOUT);
    /* Register callbacks before setting up and enabling pin interrupt. */
    GPIOINT_CallbackRegister(g_buttonArray[i].pin,
                             halInternalButtonIsr);
    /* Set rising and falling edge interrupts */
    GPIO_ExtIntConfig(g_buttonArray[i].port,
                      g_buttonArray[i].pin,
                      g_buttonArray[i].pin,
                      true,
                      true,
                      true);
  }

  g_pHoldingCallbackFunc=holdingHandle;
  g_pPressAndHoldingCallbackFunc=pressAndHoldeHandler;

}
/*
 * @func	halInternalButtonIsr
 * @brief	IRQ button
 * @param	pin
 * @retval	None
 */
void_t halInternalButtonIsr(u8_t byPin)
{
  u8_t byButtonStateNow;
  u8_t byButtonStatePrev;
  u32_t dwDebouce;
  u8_t byButtonIndex;

  byButtonIndex = getButtonIndex(byPin);
  // check valid index
  if(byButtonIndex== INDEX_INVALID)
	  return;

  byButtonStateNow = GPIO_PinInGet(g_buttonArray[byButtonIndex].port, g_buttonArray[byButtonIndex].pin);
  for ( dwDebouce = 0;
        dwDebouce < BUTTON_DEBOUNCE;
        dwDebouce = (byButtonStateNow == byButtonStatePrev) ? dwDebouce + 1 : 0 ) {
    byButtonStatePrev = byButtonStateNow;
    byButtonStateNow = GPIO_PinInGet(g_buttonArray[byButtonIndex].port, g_buttonArray[byButtonIndex].pin);
  }

  g_buttonArray[byButtonIndex].state = byButtonStateNow;

  if(byButtonStateNow == BUTTON_PRESS)
  {
	  g_buttonArray[byButtonIndex].pressCount ++;
	  if(g_buttonArray[byButtonIndex].press != true)
	  {
		  emberEventControlSetActive(buttonPressAndHoldEventControl);
	  }

	  g_buttonArray[byButtonIndex].isHolding=false;
	  g_buttonArray[byButtonIndex].holdTime=0;
	  g_buttonArray[byButtonIndex].press = true;
	  g_buttonArray[byButtonIndex].release = false;
  }
  else
  {

	  g_buttonArray[byButtonIndex].release = true;
	  g_buttonArray[byButtonIndex].press = false;
	  emberEventControlSetInactive(buttonReleaseEventControl);
	  emberEventControlSetDelayMS(buttonReleaseEventControl,BUTTON_CHECK_RELEASE_MS);
  }
}

/*
 * @func	buttonPressAndHoldEventHandle
 * @brief	Event Button Handler
 * @param	None
 * @retval	None
 */
void_t buttonPressAndHoldEventHandle(void_t)
{
	emberEventControlSetInactive(buttonPressAndHoldEventControl);
	bool holdTrigger =false;
	for(int i=0; i<BUTTON_COUNT; i++)
	{
		if(g_buttonArray[i].press ==true)
		{
			holdTrigger = true;
			g_buttonArray[i].holdTime ++;
			if(g_buttonArray[i].holdTime>=5)
			{
				g_buttonArray[i].isHolding=true;
				g_buttonArray[i].pressCount=0;
			}

			if(g_pHoldingCallbackFunc != NULL)
			{
				if((g_buttonArray[i].holdTime %5)==0)
				{
					g_pHoldingCallbackFunc(i,g_buttonArray[i].holdTime/5 + press_max);
				}
			}
		}
	}
	if(holdTrigger == true)
		emberEventControlSetDelayMS(buttonPressAndHoldEventControl,BUTTON_CHECK_HOLD_CYCLES_MS);
}

/*
 * @func	buttonReleaseEventHandle
 * @brief	Event Button Handler
 * @param	button, holdingHandler
 * @retval	None
 */
void_t buttonReleaseEventHandle(void_t)
{
	emberEventControlSetInactive(buttonReleaseEventControl);
	for(int i=0; i<BUTTON_COUNT; i++)
	{
		if(g_buttonArray[i].release == true)
		{
			if(g_pPressAndHoldingCallbackFunc != NULL)
			{
				if(g_buttonArray[i].isHolding==false)
				{
					g_pPressAndHoldingCallbackFunc(i, g_buttonArray[i].pressCount >= press_max ? unknown:g_buttonArray[i].pressCount);
				}else
				{
					g_pPressAndHoldingCallbackFunc(i, (g_buttonArray[i].holdTime/5 + press_max) >= hold_max ? unknown :(g_buttonArray[i].holdTime/5 + press_max));
				}
			}

			resetButtonParameter(i);
		}
	}
}

/*
 * @func	resetButtonParameter
 * @brief	Reset parameter
 * @param	index
 * @retval	None
 */
static void_t resetButtonParameter(u8_t byNumID)
{
	g_buttonArray[byNumID].holdTime 	= 0;
	g_buttonArray[byNumID].pressCount	= 0;
	g_buttonArray[byNumID].press		= false;
	g_buttonArray[byNumID].release		= false;
	g_buttonArray[byNumID].state		= 0;
	g_buttonArray[byNumID].isHolding    =false;
}

/*
 * @func	getButtonIndex
 * @brief	get number button
 * @param	byPin
 * @retval	None
 */
static u8_t getButtonIndex(u8_t byPin)
{
	for(int i=0; i<BUTTON_COUNT; i++)
	{
		if(g_buttonArray[i].pin == byPin)
			return i;
	}
	return -1;
}
