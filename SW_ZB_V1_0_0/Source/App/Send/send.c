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
 * File name: send.c
 *
 * Description:
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
#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "app/framework/include/af.h"
#include "send.h"
#include "Source/App/Receive/receive.h"
#include "zigbee-framework/zigbee-device-common.h"

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

/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

/******************************************************************************/



/**
 * @func    SEND_SendCommandUnicast
 * @brief   Send uinicast command
 * @param   source, destination, address
 * @retval  None
 */
static void_t SEND_SendCommandUnicast(u8_t source,
							 u8_t destination,
							 u8_t address)
{
	emberAfSetCommandEndpoints(source, destination);
	(void_t) emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, address);
}


/**
 * @func    SEND_FillBufferGlobalCommand
 * @brief   Send fill buffer global command
 * @param   clusterID, attributeID, globalCommand, value, length, dataType
 * @retval  None
 */
static void_t SEND_FillBufferGlobalCommand(EmberAfClusterId clusterID,
								  EmberAfAttributeId attributeID,
								  u8_t globalCommand,
								  u8_t* value,
								  u8_t length,
								  u8_t dataType)
{
	u8_t data[MAX_DATA_COMMAND_SIZE];
	data[0] = (u8_t)(attributeID & 0x00FF);
	data[1] = (u8_t)((attributeID & 0xFF00)>>8);
	data[2] = EMBER_SUCCESS;
	data[3] = (u8_t)dataType;
	memcpy(&data[4], value, length);

	(void_t) emberAfFillExternalBuffer((ZCL_GLOBAL_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
									  clusterID,
									  globalCommand,
									  "b",
									  data,
									  length + 4);
}


/**
 * @func    sendReportInfoHc
 * @brief   Send Report to HC
 * @param   None
 * @retval  None
 */
void_t sendReportInfoHc(void_t)
{
	u8_t modelID[13] = {12, 'S', 'W', '2','_','L','M','1','_','T','M','P','1'};
	u8_t manufactureID[5] = {4, 'L', 'u', 'm', 'i'};
	u8_t version = 1;

	if(emberAfNetworkState() != EMBER_JOINED_NETWORK){
		return;
	}
	SEND_FillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,
								 ZCL_MODEL_IDENTIFIER_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 modelID,
								 13,
								 ZCL_CHAR_STRING_ATTRIBUTE_TYPE);
	SEND_SendCommandUnicast(SOURCE_ENDPOINT_PRIMARY,
							DESTINATTION_ENDPOINT,
							HC_NETWORK_ADDRESS);

	SEND_FillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,
								 ZCL_MANUFACTURER_NAME_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 manufactureID,
								 5,
								 ZCL_CHAR_STRING_ATTRIBUTE_TYPE);
	SEND_SendCommandUnicast(SOURCE_ENDPOINT_PRIMARY,
							DESTINATTION_ENDPOINT,
							HC_NETWORK_ADDRESS);
	SEND_FillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,
								 ZCL_APPLICATION_VERSION_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 &version,
								 1,
								 ZCL_INT8U_ATTRIBUTE_TYPE);
	SEND_SendCommandUnicast(SOURCE_ENDPOINT_PRIMARY,
							DESTINATTION_ENDPOINT,
							HC_NETWORK_ADDRESS);
}


/**
 * @func    sendOnOffStateReport
 * @brief   Send On/Off State
 * @param   Endpoint, value
 * @retval  None
 */
void_t sendOnOffStateReport(u8_t Endpoint, u8_t value){
	SEND_FillBufferGlobalCommand(ZCL_ON_OFF_CLUSTER_ID,
						   ZCL_ON_OFF_ATTRIBUTE_ID,
						   ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
						   (u8_t*) &value,
						   1,
						   ZCL_BOOLEAN_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(Endpoint,
								DESTINATTION_ENDPOINT,
								HC_NETWORK_ADDRESS);

	emberAfWriteServerAttribute(Endpoint,
								ZCL_ON_OFF_CLUSTER_ID,
								ZCL_ON_OFF_ATTRIBUTE_ID,
								(u8_t*) &value,
								ZCL_BOOLEAN_ATTRIBUTE_TYPE);
}


/**
 * @func    sendPirStateReport
 * @brief   Send PIR value to App
 * @param   Endpoint, value
 * @retval  None
 */
void_t sendPirStateReport(u8_t Endpoint, u8_t value){
	SEND_FillBufferGlobalCommand(ZCL_IAS_ZONE_CLUSTER_ID,
								 ZCL_ZONE_STATUS_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 (u8_t*) &value,
								 1,
						   	   	 ZCL_BOOLEAN_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(Endpoint,
								DESTINATTION_ENDPOINT,
								HC_NETWORK_ADDRESS);

	emberAfWriteServerAttribute(Endpoint,
								ZCL_IAS_ZONE_CLUSTER_ID,
								ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								(u8_t*) &value,
								ZCL_BOOLEAN_ATTRIBUTE_TYPE);
}

/**
 * @func    sendLdrStateReport
 * @brief   Send lux value to app
 * @param   source, destination, address
 * @retval  None
 */
void_t sendLdrStateReport(u8_t Endpoint, u32_t value){
	SEND_FillBufferGlobalCommand(ZCL_ILLUM_MEASUREMENT_CLUSTER_ID,
								 ZCL_ILLUM_MEASURED_VALUE_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 (u32_t*) &value,
								 sizeof(value),
								 ZCL_INT32U_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(Endpoint,
								DESTINATTION_ENDPOINT,
								HC_NETWORK_ADDRESS);

	emberAfWriteServerAttribute(Endpoint,
								ZCL_ILLUM_MEASUREMENT_CLUSTER_ID,
								ZCL_ILLUM_MEASURED_VALUE_ATTRIBUTE_ID,
								(u32_t*) &value,
								ZCL_INT32U_ATTRIBUTE_TYPE);
}

/**
 * @func    sendTempStateReport
 * @brief   Send Temp value
 * @param   Endpoint, value
 * @retval  None
 */
void_t sendTempStateReport(u8_t Endpoint, u32_t value){
	SEND_FillBufferGlobalCommand(ZCL_TEMP_MEASUREMENT_CLUSTER_ID,
								 ZCL_TEMP_MEASURED_VALUE_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 (u32_t*) &value,
								 sizeof(value),
								 ZCL_INT32U_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(Endpoint,
								DESTINATTION_ENDPOINT,
								HC_NETWORK_ADDRESS);

	emberAfWriteServerAttribute(Endpoint,
								ZCL_TEMP_MEASUREMENT_CLUSTER_ID,
								ZCL_TEMP_MEASURED_VALUE_ATTRIBUTE_ID,
								(u32_t*) &value,
								ZCL_INT32U_ATTRIBUTE_TYPE);
}

/**
 * @func    sendZigDevRequest
 * @brief   Send ZigDevRequest
 * @param   None
 * @retval  None
 */
void_t sendZigDevRequest(void_t)
{
	u8_t contents[ZDO_MESSAGE_OVERHEAD + 1];
	contents[1] = 0x00;

	(void_t) emberSendZigDevRequest(HC_NETWORK_ADDRESS, LEAVE_RESPONSE, EMBER_AF_DEFAULT_APS_OPTIONS, contents, sizeof(contents));
}

/**
 * @func    sendBindingInitToTarget
 * @brief   Send Binding command
 * @param   remoteEndpoint, localEndpoint, value, nodeID
 * @retval  None
 */
void_t sendBindingInitToTarget(u8_t remoteEndpoint, u8_t localEndpoint, bool_t value, u16_t nodeID)
{
	EmberStatus status = EMBER_INVALID_BINDING_INDEX;

	for(u8_t i = 0; i< EMBER_BINDING_TABLE_SIZE ; i++)
		{
			EmberBindingTableEntry binding;
			status = emberGetBinding(i, &binding);
			u16_t bindingNodeID = emberGetBindingRemoteNodeId(i);

			// check status send
			if(status != EMBER_SUCCESS)
			{
				return;
			}else if((binding.local == localEndpoint) && (binding.remote == remoteEndpoint) && (bindingNodeID == nodeID))
			{
				continue;
			}
			else if((bindingNodeID != EMBER_SLEEPY_BROADCAST_ADDRESS) &&
						 (bindingNodeID != EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS) &&
						 (bindingNodeID != EMBER_BROADCAST_ADDRESS))
			{
				if(binding.local == localEndpoint && binding.clusterId == ZCL_ON_OFF_CLUSTER_ID){
					switch (value) {
						case true:
							emberAfCorePrintln("SEND ON INIT TO TARGET");
							emberAfFillCommandOnOffClusterOn();
							emberAfSetCommandEndpoints(binding.local, binding.remote);
							emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, bindingNodeID);
							sendOnOffStateReport(binding.local, LED_ON);
							break;
						case false:
							emberAfCorePrintln("SEND OFF INIT TO TARGET");
							emberAfFillCommandOnOffClusterOff();
							emberAfSetCommandEndpoints(binding.local, binding.remote);
							emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, bindingNodeID);
							sendOnOffStateReport(binding.local, LED_OFF);
							break;
					}
				}
			}
		}
}
