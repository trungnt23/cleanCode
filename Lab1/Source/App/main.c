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
 * File name: main.c
 *
 * Description:	Lab 1
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
#include "Source/Mid/Temphumi/si7020.h"
#include "app/framework/include/af.h"
#include "Source/Mid/Button/button.h"
#include "Source/Mid/Kalman_filter/kalman_filter.h"
#include "Source/Mid/Led/led.h"
#include "Source/Mid/LDR/ldr.h"
#include "typedefs.h"

/******************************************************************************/
/*                     PRIVATE TYPES and DEFINITIONS                         */
/******************************************************************************/


/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
#define PERIOD_SCAN_SENSORLIGHT									5000 	//	ms
#define PERIOD_SCAN_SENSORTEMHUMI								10000	//	ms
/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/
u32_t g_dwKalmanHumi=0;
u32_t g_dwKalmanTemp=0;

/******************************************************************************/
/*                              EVENT DATA                                  */
/******************************************************************************/
EmberEventControl readValueSensorLightControl;
EmberEventControl readValueTempHumiControl;
/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
void_t userButtonPressAndHoldEventHandler(u8_t byButton, ButtonEvent pressAndHoldEvent);
void_t userButtonPressAndHoldingEventHandler(u8_t byButton, ButtonEvent holdingHandler);
/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

/******************************************************************************/
/** @brief Main Init
 *
 * This function is called from the application's main function. It gives the
 * application a chance to do any initialization required at system startup.
 * Any code that you would normally put into the top of the application's
 * main() routine should be put into this function.
        Note: No callback
 * in the Application Framework is associated with resource cleanup. If you
 * are implementing your application on a Unix host where resource cleanup is
 * a consideration, we expect that you will use the standard Posix system
 * calls, including the use of atexit() and handlers for signals such as
 * SIGTERM, SIGINT, SIGCHLD, SIGPIPE and so on. If you use the signal()
 * function to register your signal handler, please mind the returned value
 * which may be an Application Framework function. If the return value is
 * non-null, please make sure that you call the returned function from your
 * handler to avoid_t negating the resource cleanup of the Application Framework
 * itself.
 *
 */
void_t emberAfMainInitCallback(void_t)
{
	emberAfCorePrintln("Main Init");
	buttonInit(userButtonPressAndHoldingEventHandler, userButtonPressAndHoldEventHandler);
	ledInit();
	ldr_Init();
	initI2C();
	si7020_Init();
	KalmanFilterInit(2, 2, 0.001); // Initialize Kalman filter
	emberEventControlSetDelayMS(readValueSensorLightControl, 1000);
	emberEventControlSetDelayMS(readValueTempHumiControl,1000);
}

/*
 * @func	userButtonPressAndHoldEventHandler
 * @brief	Event Button Handler
 * @param	button, pressAndHoldEvent
 * @retval	None
 */
void_t userButtonPressAndHoldEventHandler(u8_t byButton, u8_t pressAndHoldEvent)
{
	switch(pressAndHoldEvent)
	{
	case press_1:
		if(byButton == SW_1)
		{
			emberAfCorePrintln("SW1: 1 time");
			GPIO_PinOutSet(LED_PORT_1,LED_BLUE_PIN_1);
			GPIO_PinOutSet(LED_PORT_1,LED_GREEN_PIN_1);
			GPIO_PinOutClear(LED_PORT_1,LED_RED_PIN_1);

		}
		else
		{
			emberAfCorePrintln("SW2: 1 time");
			toggleLed(LED2,ledPink,3,300,300);
		}
	break;
	case press_2:
		if(byButton == SW_1)
		{
			emberAfCorePrintln("SW1: 2 times");
			GPIO_PinOutSet(LED_PORT_1,LED_BLUE_PIN_1);
			GPIO_PinOutSet(LED_PORT_1,LED_RED_PIN_1);
			GPIO_PinOutClear(LED_PORT_1,LED_GREEN_PIN_1);
		}
		else
		{
			emberAfCorePrintln("SW2: 2 time");
			toggleLed(LED2,ledyellow,4,500,500);
		}
		break;
	case press_3:
		if(byButton == SW_1)
		{
			emberAfCorePrintln("SW1: 3 times");
			GPIO_PinOutSet(LED_PORT_1,LED_RED_PIN_1);
			GPIO_PinOutSet(LED_PORT_1,LED_GREEN_PIN_1);
			GPIO_PinOutClear(LED_PORT_1,LED_BLUE_PIN_1);

		}
		break;
	default:
		break;
	}
}

/*
 * @func	userButtonPressAndHoldingEventHandler
 * @brief	Event Button Handler
 * @param	button, holdingHandler
 * @retval	None
 */
void_t userButtonPressAndHoldingEventHandler(u8_t byButton, ButtonEvent holdingHandler)
{
	switch(holdingHandler)
	{
	case hold_1s:
		emberAfCorePrintln("SW1: 1 s");
		break;
	case hold_2s:
		emberAfCorePrintln("SW1: 2 s");
		break;
	case hold_3s:
		emberAfCorePrintln("SW1: 3 s");
		break;

	default:

		break;
	}
}



/**
 * @func   lightSensorPollingReadHandler
 * @brief  Read value from ADC
 * @param  None
 * @retval None
 */
void_t lightSensorPollingReadHandler(void_t)
{
	emberEventControlSetInactive(readValueSensorLightControl);
	u32_t dwLux = lightSensorPollingRead();
	emberAfCorePrintln("Light:   %d lux         ",dwLux);
	emberEventControlSetDelayMS(readValueSensorLightControl,PERIOD_SCAN_SENSORLIGHT);
}

/**
 * @func    readValueTempHumiHandler
 * @brief   Event Sensor Handler
 * @param   None
 * @retval  None
 */
void_t readValueTempHumiHandler(void_t)
{
	emberEventControlSetInactive(readValueTempHumiControl);
	g_dwKalmanHumi = si7020_MeasureHumi();
	g_dwKalmanTemp = si7020_MeasureTemp();
	emberAfCorePrintln("Humi:    %d RH       Temp:     %d oC        ", g_dwKalmanHumi,g_dwKalmanTemp);
	emberEventControlSetDelayMS(readValueTempHumiControl,PERIOD_SCAN_SENSORTEMHUMI);
}

/**
 * @func    emberIncomingManyToOneRouteRequestHandler
 * @brief   Handler infor many to one route request
 * @param   source, longId, cost
 * @retval  None
 */
void_t emberIncomingManyToOneRouteRequestHandler(EmberNodeId source,
                                               EmberEUI64 longId,
                                               u8_t cost)
{
}
