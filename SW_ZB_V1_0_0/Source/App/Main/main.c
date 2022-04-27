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
 * Description:
 *
 *
 * Last Changed By:  $Author: trungnt $
 * Revision:         $Revision: $
 * Last Changed:     $Date: $April 27, 2022
 *
 * Code sample:
 ******************************************************************************/
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include "app/framework/include/af.h"
#include "protocol/zigbee/stack/include/binding-table.h"
#include <math.h>
#include "Source/Mid/LED/led.h"
#include "Source/Mid/Button/button.h"
#include "Source/Mid/Kalman_filter/kalman_filter.h"
#include "Source/Mid/LDR/ldr.h"
#include "Source/Mid/Temphumi/Si7020.h"
#include "Source/App/Network/network.h"
#include "Source/App/Send/send.h"
#include "Source/App/Receive/receive.h"
#include "main.h"
/******************************************************************************/
/*                     PRIVATE TYPES and DEFINITIONS                         */
/******************************************************************************/


/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/


/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/
EmberEventControl lightSensorReadValueControl;
EmberEventControl readValueTempHumiControl;
EmberEventControl mainStateEventControl;
EmberEventControl MTORRsEventControl;
EmberEventControl findNetworkControl;


SystemState g_SystemState = POWER_ON_STATE;
bool_t g_boNetworkReady = false;
u32_t g_dwLux_1time = 0;
u32_t g_dwLux_2times = 0;

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
void_t mainButtonPressCallbackHandler(u8_t button, ButtonEvent pressHandler);
void_t mainButtonHoldCallbackHandler(u8_t button, ButtonEvent holdingHandler);
void_t mainNetworkEventHandler(u8_t networkResult);
void_t emberIncomingManyToOneRouteRequestHandler(EmberNodeId source,
                                               EmberEUI64 longId,
                                               u8_t cost);
/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

/******************************************************************************/
/** @brief Main Init
 *
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
	ledInit();
	buttonInit(mainButtonHoldCallbackHandler, mainButtonPressCallbackHandler);
	network_Init(mainNetworkEventHandler);
	emberEventControlSetActive(mainStateEventControl);
	ldr_Init();
	initI2C();
	si7020_Init();
	kalmanFilterInit(2, 2, 0.001); // Initialize Kalman filter
	emberEventControlSetActive(lightSensorReadValueControl);
	emberEventControlSetActive(lightSensorReadValueControl);
}

/*
 * @func	mainButtonPressCallbackHandler
 * @brief	Event Button Handler
 * @param	button, pressHandler
 * @retval	None
 */
void_t mainButtonPressCallbackHandler(u8_t button, ButtonEvent pressHandler)
{
	switch(pressHandler)
		{
		case press_1:
			if(button == SW_1)
			{
				emberAfCorePrintln("SW1: 1 time");
				turnOnLed(LED1,ledBlue);
				sendOnOffStateReport(1, LED_ON);
				sendBindingInitToTarget(SOURCE_ENDPOINT_PRIMARY,
										 DESTINATTION_ENDPOINT,
										 true,
										 HC_NETWORK_ADDRESS);
			}
			else
			{
				emberAfCorePrintln("SW2: 1 time");
				turnOnLed(LED2,ledBlue);
				sendOnOffStateReport(2, LED_ON);

			}
		break;
		case press_2:
			if(button == SW_1)
			{
				emberAfCorePrintln("SW1: 2 times");
				sendOnOffStateReport(1, LED_OFF);
				turnOffRBGLed(LED1);
				sendBindingInitToTarget(SOURCE_ENDPOINT_PRIMARY,
										DESTINATTION_ENDPOINT,
										false,
										HC_NETWORK_ADDRESS);
			}
			else
			{
				emberAfCorePrintln("SW2: 2 time");
				turnOffRBGLed(LED2);
				sendOnOffStateReport(2, LED_OFF);

			}
			break;
		case press_3:
			if(button == SW_1)
			{
				emberAfCorePrintln("SW1: 3 time");
				emberAfPluginFindAndBindTargetStart(1);
				toggleLed(LED2,ledyellow,3,200,200);


			}
			else
			{
				emberAfCorePrintln("SW2: 3 time");
				emberAfPluginFindAndBindInitiatorStart(1);
				toggleLed(LED2,ledRGB,3,200,200);
			}
			break;
		case press_4:
			if(button == SW_1)
			{
				emberAfCorePrintln("SW1: 4 time");


			}
			else
			{
				emberAfCorePrintln("SW2: 4 time");
			}
			break;
		case press_5:
			if(button == SW_1)
			{
				emberAfCorePrintln("SW1: 5 time");

			}
			else
			{
				emberAfCorePrintln("SW2: 5 time");
				toggleLed(LED1,ledRed, 2, 200, 200);
				g_SystemState = REBOOT_STATE;
				emberEventControlSetDelayMS(mainStateEventControl,3000);
			}
			break;
		default:
			break;
		}
}

/*
 * @func	mainButtonHoldCallbackHandler
 * @brief	Event Button Handler
 * @param	button, holdingHandler
 * @retval	None
 */
void_t mainButtonHoldCallbackHandler(u8_t button, ButtonEvent holdingHandler)
{
//	emberAfCorePrintln("SW %d HOLDING %d s\n",button+1,holdingHandler-press_max);
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
	case hold_4s:
		emberAfCorePrintln("SW1: 4 s");
		break;
	case hold_5s:
		emberAfCorePrintln("SW1: 5 s");
		break;

	default:
		break;
	}
}


/*
 * @func	mainStateEventHandler
 * @brief	Handle Event State Network
 * @param	None
 * @retval	None
 */
void_t mainStateEventHandler(void_t)
{
	emberEventControlSetInactive(mainStateEventControl);
	EmberNetworkStatus nwkStatusCurrent;
	switch (g_SystemState) {
		case POWER_ON_STATE:
			nwkStatusCurrent = emberAfNetworkState();
			if(nwkStatusCurrent == EMBER_NO_NETWORK)
			{
				toggleLed(LED1,ledRed,3,200,200);
				emberEventControlSetInactive(findNetworkControl);
				emberEventControlSetActive(findNetworkControl);
			}
			g_SystemState = IDLE_STATE;
			break;
		case REPORT_STATE:
			g_SystemState = IDLE_STATE;
			sendReportInfoHc();
			break;
		case IDLE_STATE:
				emberAfCorePrintln("IDLE_STATE");
			break;
		case REBOOT_STATE:
			g_SystemState = IDLE_STATE;
			EmberNetworkStatus networkStatus = emberAfNetworkState();
			if (networkStatus != EMBER_NO_NETWORK) {
				sendZigDevRequest();
				emberClearBindingTable();
				emberLeaveNetwork();
			} else {
				halReboot();
			}
			break;
		default:
			break;
	}

}

/*
 * @func	mainNetworkEventHandler
 * @brief	Handler Event Result Network
 * @param	networkResult
 * @retval	None
 */
void_t mainNetworkEventHandler(u8_t networkResult)
{
	emberAfCorePrintln("Network Event Handle");
	switch (networkResult) {
		case NETWORK_HAS_PARENT:
			emberAfCorePrintln("Network has parent");
			toggleLed(LED1,ledPink,3,300,300);
			g_boNetworkReady = true;
			g_SystemState = REPORT_STATE;
			emberEventControlSetDelayMS(mainStateEventControl, 1000);
			break;
		case NETWORK_JOIN_FAIL:
			g_SystemState = IDLE_STATE;
			toggleLed(LED1,ledBlue,3,300,300);
			emberAfCorePrintln("Network Join Fail");
			emberEventControlSetDelayMS(mainStateEventControl, 1000);
			break;
		case NETWORK_JOIN_SUCCESS:
			emberEventControlSetInactive(findNetworkControl);
			emberAfCorePrintln("Network Join Success");
			toggleLed(LED1,ledPink,3,300,300);
			g_boNetworkReady =true;
			g_SystemState = REPORT_STATE;
			emberEventControlSetDelayMS(mainStateEventControl, 1000);
			break;
		case NETWORK_LOST_PARENT:
			emberAfCorePrintln("Network lost parent");
			toggleLed(LED1,ledPink,3,300,300);
			g_SystemState = IDLE_STATE;
			emberEventControlSetDelayMS(mainStateEventControl, 1000);
			break;
		case NETWORK_OUT_NETWORK:
			if(g_boNetworkReady)
			{
				toggleLed(LED1,ledRed,3,300,300);
				g_SystemState = REBOOT_STATE;
				emberEventControlSetDelayMS(mainStateEventControl, 3000);
			}
			break;
		default:
			break;
	}
}

/*
 * @func	findNetworkHandler
 * @brief	Find network if device doesn't have network
 * @param	networkResult
 * @retval	None
 */
void_t findNetworkHandler(void_t)
{
	emberEventControlSetInactive(findNetworkControl);
	networkFindAndJoin();
	emberEventControlSetDelayMS(findNetworkControl, 10000);

}

/**
 * @func   lightSensorReadValueHandler
 * @brief  Read value from ADC
 * @param  None
 * @retval None
 */
void_t lightSensorReadValueHandler(void_t)
{
	emberEventControlSetInactive(lightSensorReadValueControl);
	g_dwLux_1time = lightSensorPollingRead();
	if(abs(g_dwLux_2times - g_dwLux_1time) > 30)
		{
		g_dwLux_2times = g_dwLux_1time;
		sendLdrStateReport(3,g_dwLux_2times);
		emberAfCorePrintln("Light:   %d lux         ",g_dwLux_2times);
			if(g_dwLux_2times > 500)
			{
				turnOnLed(LED2,ledGreen);
			}
			else
			{
				turnOffRBGLed(LED2);
			}
		}
	emberEventControlSetDelayMS(lightSensorReadValueControl,PERIOD_SCAN_SENSORLIGHT);
}

/**
 * @func    readValueTempHumiHandler
 * @brief   Event Sensor Handler
 * @param   None
 * @retval  None
 */
void_t readValueTempHumiHandler(void_t)
{
	emberEventControlSetInactive(lightSensorReadValueControl);
	u32_t Humi = si7020_MeasureHumi();
	u32_t Temp = si7020_MeasureTemp();
	sendTempStateReport(TEMP_ENDPOINT, Temp);
	emberAfCorePrintln("Humi:    %d RH       Temp:     %d oC        ", Humi, Temp);
	emberEventControlSetDelayMS(lightSensorReadValueControl,PERIOD_SCAN_SENSORTEMHUMI);
}



/*
 * @func	emberIncomingManyToOneRouteRequestHandler
 * @brief	Update state send to HC
 * @param	EmberNodeId, EmberEUI64, cost
 * @retval	None
 */
void_t emberIncomingManyToOneRouteRequestHandler(EmberNodeId source,
                                               EmberEUI64 longId,
                                               u8_t cost)
{
	// handle for MTORRs
	emberAfCorePrintln("Received MTORRs");
	emberEventControlSetInactive(MTORRsEventControl);
	emberEventControlSetDelayMS(MTORRsEventControl,2* ((u8_t)halCommonGetRandom()));			// 1-2p
}

/*
 * @func	MTORRsEventHandler
 * @brief	Read Status
 * @param	None
 * @retval	None
 */
void_t MTORRsEventHandler(void_t)
{
	emberEventControlSetInactive(MTORRsEventControl);
	u8_t data;
	EmberAfStatus status = emberAfReadServerAttribute(RGB1_ENDPOINT,
											   ZCL_ON_OFF_CLUSTER_ID,
											   ZCL_ON_OFF_ATTRIBUTE_ID,
											   &data,
											   RGB1_ENDPOINT);
	if(status == EMBER_ZCL_STATUS_SUCCESS){
		sendOnOffStateReport(RGB1_ENDPOINT,data);
	}

	EmberAfStatus status1 = emberAfReadServerAttribute(RGB2_ENDPOINT,
												   ZCL_ON_OFF_CLUSTER_ID,
												   ZCL_ON_OFF_ATTRIBUTE_ID,
												   &data,
												   RGB1_ENDPOINT);
		if(status1 == EMBER_ZCL_STATUS_SUCCESS){
			sendOnOffStateReport(RGB2_ENDPOINT,data);
		}
}
