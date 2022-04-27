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
 * Description: process button
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
EmberEventControl buttonPressAndHoldEventControl;
EmberEventControl buttonReleaseEventControl;

buttonHoldingEvent g_pHoldingCallbackFunc = NULL;
buttonPressAndHoldEvent g_pPressAndg_pHoldingCallbackFunc = NULL;
ButtonConfig_t g_buttonArray[BUTTON_COUNT] = BUTTON_INIT;
/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
static void_t halInternalButtonIsr(u8_t pin);
static u8_t getButtonIndex(u8_t pin);
static void_t resetButtonParameter(u8_t index);
void_t buttonPressAndHoldEventHandle(void_t);
void_t buttonReleaseEventHandle(void_t);
/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/


/******************************************************************************/

/*
 * @func	buttonInit
 * @brief	Initialize Button
 * @param	holdingHandle, pressHandler
 * @retval	None
 */
void_t buttonInit(buttonHoldingEvent holdingHandle,buttonPressAndHoldEvent pressHandler)
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
  g_pPressAndg_pHoldingCallbackFunc=pressHandler;

}
/*
 * @func	halInternalButtonIsr
 * @brief	IRQ button
 * @param	pin
 * @retval	None
 */
static void_t halInternalButtonIsr(u8_t pin)
{
  u8_t buttonStateNow;
  u8_t buttonStatePrev;
  u32_t debounce;
  u8_t buttonIndex;

  buttonIndex = getButtonIndex(pin);
  // check valid index
  if(buttonIndex==-1)
	  return;

  buttonStateNow = GPIO_PinInGet(g_buttonArray[buttonIndex].port, g_buttonArray[buttonIndex].pin);
  for ( debounce = 0;
        debounce < BUTTON_DEBOUNCE;
        debounce = (buttonStateNow == buttonStatePrev) ? debounce + 1 : 0 ) {
    buttonStatePrev = buttonStateNow;
    buttonStateNow = GPIO_PinInGet(g_buttonArray[buttonIndex].port, g_buttonArray[buttonIndex].pin);
  }

  g_buttonArray[buttonIndex].state = buttonStateNow;

  if(buttonStateNow == BUTTON_PRESS)
  {
	  g_buttonArray[buttonIndex].pressCount++;
	  if(g_buttonArray[buttonIndex].press != true)
	  {
		  emberEventControlSetActive(buttonPressAndHoldEventControl);
	  }

	  g_buttonArray[buttonIndex].isHolding=false;
	  g_buttonArray[buttonIndex].holdTime=0;
	  g_buttonArray[buttonIndex].press = true;
	  g_buttonArray[buttonIndex].release = false;

  }
  else
  {

	  g_buttonArray[buttonIndex].release = true;
	  g_buttonArray[buttonIndex].press = false;
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
			g_buttonArray[i].holdTime++;
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
			if(g_pPressAndg_pHoldingCallbackFunc != NULL)
			{
				if(g_buttonArray[i].isHolding==false)
				{
					g_pPressAndg_pHoldingCallbackFunc(i, g_buttonArray[i].pressCount >= press_max ? unknown:g_buttonArray[i].pressCount);
				}else
				{
					g_pHoldingCallbackFunc(i, (g_buttonArray[i].holdTime/5 + press_max) >= hold_max ? unknown :(g_buttonArray[i].holdTime/5 + press_max));
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
static void_t resetButtonParameter(u8_t index)
{
	g_buttonArray[index].holdTime 	= 0;
	g_buttonArray[index].pressCount	= 0;
	g_buttonArray[index].press		= false;
	g_buttonArray[index].release	= false;
	g_buttonArray[index].state		= 0;
	g_buttonArray[index].isHolding  =false;
}

/*
 * @func	getButtonIndex
 * @brief	get number button
 * @param	pin
 * @retval	None
 */
static u8_t getButtonIndex(u8_t pin)
{
	for(int i=0; i<BUTTON_COUNT; i++)
	{
		if(g_buttonArray[i].pin == pin)
			return i;
	}
	return -1;
}
