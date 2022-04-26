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
 * File name: si7020.h
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
// Enclosing macro to prevent multiple inclusion
#ifndef SI7020_H_
#define SI7020_H_
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include <typedefs.h>
/******************************************************************************/
/*                     PRIVATE TYPES and DEFINITIONS                         */
/******************************************************************************/


/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
#define I2C_GPIO							 gpioPortB
#define I2C0_PIN_SDA			    		 	 1
#define I2C0_PIN_SCL			    			 0
#define SCAN_TEMP_AND_HUMI						10000

/* I2C device address for Si7020 */
#define SI7020_ADDR            0x40

/* Device ID value for Si7020 */
#define SI7020_DEVICE_ID       0x14

/* Si7020 Read Temperature Command */
#define SI7020_READ_TEMP       0xE0

/* Si7020 Read RH Command */
#define SI7020_READ_RH         0xE5

/* SI7020 ID */
#define SI7020_READ_ID_1       0xFC
#define SI7020_READ_ID_2       0xC9

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
void_t initI2C(void_t);
void_t si7020_Init (void_t);
bool_t si7020_Measure (u32_t *buffer, u8_t command, u8_t Length_Data);
u32_t si7020_MeasureHumi (void_t);
u32_t si7020_MeasureTemp (void_t);
/******************************************************************************/
#endif /* SI7020_H_ */