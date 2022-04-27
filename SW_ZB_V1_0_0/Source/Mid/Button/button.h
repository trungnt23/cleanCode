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
 * File name: BUTTON.h
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
// Enclosing macro to prevent multiple inclusion
#ifndef BUTTON_H_
#define BUTTON_H_
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include <stddef.h>
#include "typedefs.h"
/******************************************************************************/
/*                     PRIVATE TYPES and DEFINITIONS                         */
/******************************************************************************/


/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
#define BUTTON_COUNT					2
#define GPIO_DOUT						0
#define BUTTON_DEBOUNCE					5
#define BUTTON_CHECK_HOLD_CYCLES_MS		200
#define BUTTON_CHECK_RELEASE_MS			500
#define HOLD_TIME_MIN					5

#define BUTTON_1_PORT					gpioPortD
#define BUTTON_1_PIN					(4U)

#define BUTTON_2_PORT					gpioPortD
#define BUTTON_2_PIN					(3U)

#define BUTTON_INIT						{{BUTTON_1_PORT,BUTTON_1_PIN},{BUTTON_2_PORT,BUTTON_2_PIN}}


typedef enum
{
	press_1	= 1,
	press_2 ,
	press_3 ,
	press_4 ,
	press_5,
	press_max=10,
	hold_1s	,
	hold_2s	,
	hold_3s	,
	hold_4s	,
	hold_5s ,
	hold_max,
	unknown,
}ButtonEvent;

enum
{
	SW_1,
	SW_2
};

enum
{
	BUTTON_PRESS,
	BUTTON_RELEASE
};

typedef struct {
  GPIO_Port_TypeDef		port;
  uint_t				pin;
  bool_t				state;
  u8_t					pressCount;
  u8_t					holdTime;
  bool_t				release;
  bool_t				press;
  bool_t                isHolding;
} ButtonConfig_t;

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
typedef void_t (*buttonHoldingEvent)(u8_t btIndex, ButtonEvent holdEvent );
typedef void_t (*buttonPressAndHoldEvent)(u8_t btIndex, ButtonEvent pressEvent);

void_t buttonInit(buttonHoldingEvent holdingHandler,buttonPressAndHoldEvent pressHandler);
/******************************************************************************/
#endif /* BUTTON_H_ */
