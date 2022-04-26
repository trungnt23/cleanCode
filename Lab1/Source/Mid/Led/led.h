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
// Enclosing macro to prevent multiple inclusion
#ifndef LED_H_
#define LED_H_
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

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
}ledNumber;
typedef enum
{
	ledOff 				= 0x000,
	ledRed				= BIT(0),
	ledGreen			= BIT(1),
	ledBlue				= BIT(2),
	ledPink				= ledRed  | ledBlue,
	ledyellow			= ledGreen| ledRed,
	ledRGB				= ledRed  | ledGreen | ledBlue
}ledColor_e;

typedef enum
{
	red,
	green,
	blue,
	off
}ledState_e;

enum{
	LED_FREE,
	LED_TOGGLE
};

typedef struct {
  GPIO_Port_TypeDef   port;
  unsigned int        pin;
  bool 				  ledBlinkMode;
  ledColor_e		  color;
  uint32_t 			  onTime;
  uint32_t			  offTime;
  uint8_t			  blinkTime;
} ledArray_t;

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
void ledInit(void);
void turnOffLedRBG(ledNumber index);
void toggleLed(ledNumber ledIndex, ledColor_e color, uint8_t toggleTime, uint32_t onTimeMs, uint32_t offTimeMs);
/******************************************************************************/
#endif /* LED_H_ */




