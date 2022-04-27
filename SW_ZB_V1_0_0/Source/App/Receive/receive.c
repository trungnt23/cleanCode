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
 * File name: receive.c
 *
 * Description: handle news received
 *
 *
 * Last Changed By:  $Author: trungnt $
 * Revision:         $Revision: $
 * Last Changed:     $Date: $April 27, 2022
 *
 *
 ******************************************************************************/
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include "app/framework/include/af.h"
#include "Source/Mid/LED/led.h"
#include "Source/App/Send/send.h"
#include "receive.h"
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

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
bool_t emberAfPreCommandReceivedCallback(EmberAfClusterCommand* cmd);
static bool_t receiveHandleLevelControlCluster(EmberAfClusterCommand* cmd);
static bool_t receiveHandleOnOffCluster(EmberAfClusterCommand* cmd);
/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

/******************************************************************************/


/**
 * @func    emberAfPreCommandReceivedCallback
 * @brief   Process Command Received
 * @param   EmberAfClusterCommand
 * @retval  bool
 */
bool_t emberAfPreCommandReceivedCallback(EmberAfClusterCommand* cmd)
{
	if(cmd->clusterSpecific)
	{
		switch(cmd->apsFrame->clusterId)
		{
			case ZCL_ON_OFF_CLUSTER_ID:
				receiveHandleOnOffCluster(cmd);
				break;
			case ZCL_LEVEL_CONTROL_CLUSTER_ID:
				receiveHandleLevelControlCluster(cmd);
				break;
			default:
				break;
		}
	}
	return false;
}


/**
 * @func    emberAfPreMessageReceivedCallback
 * @brief   Process Pre message received
 * @param   EmberAfIncomingMessage
 * @retval  None
 */
bool_t emberAfPreMessageReceivedCallback(EmberAfIncomingMessage* incommingMessage)
{
	if(incommingMessage->apsFrame->clusterId == ACTIVE_ENDPOINTS_RESPONSE)
	{
		return true;
	}
 return false;
}


/**
 * @func    receiveHandleLevelControlCluster
 * @brief   Handler Level led
 * @param   EmberAfClusterCommand
 * @retval  None
 */
static bool_t receiveHandleLevelControlCluster(EmberAfClusterCommand* cmd)
{
	u8_t commandID = cmd->commandId;
	u8_t endPoint  = cmd->apsFrame -> destinationEndpoint;
	u8_t payloadOffset = cmd->payloadStartIndex;		// Gan offset = startindex
	u8_t level;
	u16_t transitionTime;
	emberAfCorePrintln("ClusterID: 0x%2X", cmd->apsFrame->clusterId);
/******************************************LEVEL CONTROL LED***************************************************************************/
		switch(commandID)
			{

				case ZCL_MOVE_TO_LEVEL_COMMAND_ID:
					if(cmd->bufLen < payloadOffset + 1u)
					{
						return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
					}
					level = emberAfGetInt8u(cmd->buffer, payloadOffset, cmd->bufLen);		// Lay 1 byte gia tri cua level control
					transitionTime = emberAfGetInt16u(cmd->buffer, payloadOffset+1, cmd->bufLen);
					emberAfCorePrintln(" receiveHandleLevelControlCluster LEVEL = %d, time: 0x%2X\n", level, transitionTime);

					if(endPoint == 1)
					{
						if(level >=80)
						{
							emberAfCorePrintln("LED GREEN");
							turnOnLed(LED1, ledGreen);
						}else if(level>=40)
						{
							emberAfCorePrintln("LED RED");
							turnOnLed(LED1, ledRed);
						}else if(level>0)
						{
							emberAfCorePrintln("LED BLUE");
							turnOnLed(LED1, ledBlue);
						}else
						{
							emberAfCorePrintln("turn 0ff");
							turnOffRBGLed(LED1);
						}

					}
					break;
				default:
					break;
				}
		return false;
}


/**
 * @func    receiveHandleOnOffCluster
 * @brief   Handler On/Off command
 * @param   EmberAfClusterCommand
 * @retval  bool_t
 */
static bool_t receiveHandleOnOffCluster(EmberAfClusterCommand* cmd)
{
	u8_t commandID = cmd->commandId;
	u8_t localEndpoint = cmd ->apsFrame -> destinationEndpoint;
	u8_t remoteEndpoint = cmd->apsFrame -> sourceEndpoint;
	u16_t IgnoreNodeID = cmd->source;
/************************************ON-OFF LED******************************************************************************************/
	emberAfCorePrintln("RECEIVE_HandleOnOffCluster SourceEndpoint = %d, RemoteEndpoint = %d, commandID = %d, nodeID %2X\n",remoteEndpoint,localEndpoint,commandID,IgnoreNodeID);
	switch(commandID)
	{
	case ZCL_OFF_COMMAND_ID:
		emberAfCorePrintln("Turn off LED");

		switch (cmd->type) {
			case EMBER_INCOMING_UNICAST:
				if(localEndpoint == 1)
				{
				turnOffRBGLed(LED1);
				sendOnOffStateReport(localEndpoint, LED_OFF);
				emberAfCorePrintln("check: %d",checkBindingTable(localEndpoint));
					if(checkBindingTable(localEndpoint) >= 1)
					{

						sendBindingInitToTarget(remoteEndpoint, localEndpoint, false, IgnoreNodeID);
					}
				}
				if(localEndpoint == 2)
				{
					turnOffRBGLed(LED2);
					sendOnOffStateReport(localEndpoint, LED_OFF);
				}
				break;
			case EMBER_INCOMING_MULTICAST:
				emberAfCorePrintln("Multicast");
				turnOnLed(LED1, ledOff);
				turnOnLed(LED2, ledOff);
				break;
			default:
				break;
		}

		break;
	case ZCL_ON_COMMAND_ID:

		emberAfCorePrintln("Turn on LED");
		switch (cmd->type) {
			case EMBER_INCOMING_UNICAST:
				if(localEndpoint == 1)
				{
					turnOnLed(LED1, ledBlue);
					sendOnOffStateReport(localEndpoint, LED_ON);
					if(checkBindingTable(localEndpoint) >= 1)
					{
						sendBindingInitToTarget(remoteEndpoint, localEndpoint, true, IgnoreNodeID);
					}
				}
				if(localEndpoint == 2)
				{
					turnOnLed(LED2, ledBlue);
					sendOnOffStateReport(localEndpoint, LED_ON);
				}
				break;
			case EMBER_INCOMING_MULTICAST:
				emberAfCorePrintln("Multicast");
				turnOnLed(LED2, ledGreen);
				break;
			default:
				break;
		}
		break;
	default:
		break;
	}
	return false;
}

/*
 * @function 			: checkBindingTable
 *
 * @brief				: API support to check information on binding table.
 *
 * @parameter			: localEndpoint
 *
 * @return value		: True or false
 */


u8_t checkBindingTable(u8_t localEndpoint)
{
	u8_t index = 0;
	for(u8_t i=0; i< EMBER_BINDING_TABLE_SIZE; i++)
	{
		EmberBindingTableEntry binding;
		if(emberGetBindingRemoteNodeId(i) != EMBER_SLEEPY_BROADCAST_ADDRESS){
			emberGetBinding(i, &binding);
			if(binding.local == localEndpoint && (binding.type == EMBER_UNICAST_BINDING))
			{
				index++;
			}
		}
	}
	return index;
}


