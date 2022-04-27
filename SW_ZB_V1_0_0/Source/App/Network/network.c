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
 * File name: network.c
 *
 * Description: Event network handler
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
#include "network.h"

/******************************************************************************/
/*                     PRIVATE TYPES and DEFINITIONS                         */
/******************************************************************************/


/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/


/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/
EmberEventControl joinNetworkEventControl;

u32_t g_dwTimeFindAndJoin = 0;
networkEventHandler g_pNetworkProcess = NULL;
/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
void_t joinNetworkEventHandler(void_t);
/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

/******************************************************************************/




/*
 * @function 			: network_Init
 *
 * @brief				: Handle event network.
 *
 * @parameter			: networkEventHandler
 *
 * @return value		: None
 */
void_t network_Init(networkEventHandler networkResult)
{
	g_pNetworkProcess = networkResult;
}

/*
 * @function 			: networkFindAndJoin
 *
 * @brief				: Find network
 *
 * @parameter			: None
 *
 * @return value		: None
 */
void_t networkFindAndJoin(void_t)
{
	if(emberAfNetworkState() == EMBER_NO_NETWORK)
	{
		emberEventControlSetDelayMS(joinNetworkEventControl, 2000);
	}
}


/*
 * @function 			: networkStopFindAndJoin
 *
 * @brief				: Stop find network
 *
 * @parameter			: None
 *
 * @return value		: None
 */
void_t networkStopFindAndJoin(void_t)
{
	emberAfPluginNetworkSteeringStop();
}


/*
 * @function 			: joinNetworkEventHandler
 *
 * @brief				: Handle Event Join network
 *
 * @parameter			: None
 *
 * @return value		: None
 */
void_t joinNetworkEventHandler(void_t)
{
	emberEventControlSetInactive(joinNetworkEventControl);

	if(emberAfNetworkState() == EMBER_NO_NETWORK)
	{
		emberAfPluginNetworkSteeringStart();
		g_dwTimeFindAndJoin++;
	}
}


/*
 * @function 			: emberAfStackStatusCallback
 *
 * @brief				: Stack Status
 *
 * @parameter			: EmberStatus
 *
 * @return value		: True or false
 */
bool_t emberAfStackStatusCallback(EmberStatus status)
{
	emberAfCorePrintln("emberAfStackStatusCallback\n");

	if(status == EMBER_NETWORK_UP)
	{
		if(g_dwTimeFindAndJoin>0)// vao mang thanh cong
		{
			networkStopFindAndJoin();
			if(g_pNetworkProcess != NULL)
			{
				g_pNetworkProcess(NETWORK_JOIN_SUCCESS);
				emberAfCorePrintln("NETWORK_JOIN_SUCCESS");
			}
		}else
		{
			if(g_pNetworkProcess != NULL)
			{
				g_pNetworkProcess(NETWORK_HAS_PARENT);
				emberAfCorePrintln("NETWORK_HAS_PARENT");
			}
		}

	}
	else
	{
		EmberNetworkStatus nwkStatusCurrent = emberAfNetworkState();
		if(nwkStatusCurrent == EMBER_NO_NETWORK)
		{
			if(g_pNetworkProcess != NULL)
			{
				g_pNetworkProcess(NETWORK_OUT_NETWORK);
				emberAfCorePrintln("NETWORK_OUT_NETWORK");
			}
		}
		else if(nwkStatusCurrent == EMBER_JOINED_NETWORK_NO_PARENT){
			emberAfCorePrintln("NETWORK_LOST_PARENT");
			g_pNetworkProcess(NETWORK_LOST_PARENT);
		}
	}
	if(status == EMBER_JOIN_FAILED)
	{
		emberAfCorePrintln("NETWORK_JOIN_FAIL");
		g_pNetworkProcess(NETWORK_JOIN_FAIL);
	}
	return false;
}
