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
 * File name: led.h
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
#ifndef LED_H_
#define LED_H_
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include "typedefs.h"
/******************************************************************************/
/*                     PRIVATE TYPES and DEFINITIONS                         */
/******************************************************************************/

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
#define LED_RGB_COUNT				2
#define LED_RGB_ELEMENT				3

#define LED_PORT_1					gpioPortA
#define LED_BLUE_PIN_1				(0U)
#define LED_GREEN_PIN_1				(3U)
#define LED_RED_PIN_1				(4U)

#define LED_PORT_2					gpioPortD
#define LED_BLUE_PIN_2				(2U)
#define LED_GREEN_PIN_2				(1U)
#define LED_RED_PIN_2				(0U)

#define LED_RGB_1                       { { LED_PORT_1, LED_RED_PIN_1 }, { LED_PORT_1, LED_GREEN_PIN_1 }, { LED_PORT_1, LED_BLUE_PIN_1 } }
#define LED_RGB_2                       { { LED_PORT_2, LED_RED_PIN_2 }, { LED_PORT_2, LED_GREEN_PIN_2 }, { LED_PORT_2, LED_BLUE_PIN_2 } }

typedef enum{
	LED1,
	LED2
}LedNumber;
typedef enum
{
	ledOff 				= 0x000,
	ledRed				= BIT(0),
	ledGreen			= BIT(1),
	ledBlue				= BIT(2),
	ledPink				= ledRed  | ledBlue,
	ledyellow			= ledGreen| ledRed,
	ledRGB				= ledRed  | ledGreen | ledBlue
}LedColor;

typedef enum
{
	red,
	green,
	blue,
	off
}LedState;

enum{
	LED_FREE,
	LED_TOGGLE
};

typedef struct {
  GPIO_Port_TypeDef   port;
  unsigned int        pin;
  bool 				  ledBlinkMode;
  LedColor		  color;
  u32_t 			  onTime;
  u32_t			  offTime;
  u8_t			  blinkTime;
} LedArray_t;

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
void_t ledInit(void_t);
void_t turnOffLedRBG(LedNumber index);
void_t toggleLed(LedNumber ledIndex, LedColor color, u8_t toggleTime, u32_t onTimeMs, u32_t offTimeMs);
/******************************************************************************/
#endif /* LED_H_ */




