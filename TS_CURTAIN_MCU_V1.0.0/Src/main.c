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
 * Description: Assignment1
 * Project name: TS_CURTAIN_MCU_V1.0.0
 *
 *
 * Last Changed By:  $Author: trungnt $
 * Revision:         $Revision: 1.0 $
 * Last Changed:     $Date: $April 21, 2022
 *
 *
 ******************************************************************************/
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include <stdint.h>
#include <stm32f401re_gpio.h>
#include <stm32f401re_exti.h>
#include <stm32f401re_syscfg.h>
#include <stm32f401re_rcc.h>
#include <misc.h>
#include <timer.h>
#include <typedefs.h>

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
#define GPIO_PIN_SET									1
#define GPIO_PIN_RESET									0
#define GPIO_PIN_LOW									0
#define GPIO_PIN_HIGH									1

// LED GREEN1 - GREEN2
#define LEDGREEN1_GPIO_PIN								GPIO_Pin_0
#define LEDGREEN1_GPIO_PORT								GPIOA

#define LEDGREEN2_GPIO_PIN								GPIO_Pin_11
#define LEDGREEN2_GPIO_PORT								GPIOA

// LED RED1 - RED2
#define LEDRED1_GPIO_PIN								GPIO_Pin_1
#define LEDRED1_GPIO_PORT								GPIOA

#define LEDRED2_GPIO_PIN								GPIO_Pin_13
#define LEDRED2_GPIO_PORT								GPIOB

// LED BLUE1 - BLUE2
#define LEDBLUE1_GPIO_PIN								GPIO_Pin_3
#define LEDBLUE1_GPIO_PORT								GPIOA

#define LEDBLUE2_GPIO_PIN								GPIO_Pin_10
#define LEDBLUE2_GPIO_PORT								GPIOA

// BUZZER
#define BUZZER_GPIO_PIN									GPIO_Pin_9
#define BUZZER_GPIO_PORT								GPIOC

// Button B2
#define BUTTONB2_GPIO_PIN								GPIO_Pin_3
#define BUTTONB2_GPIO_PORT								GPIOB

// Button B3
#define BUTTONB3_GPIO_PIN								GPIO_Pin_4
#define BUTTONB3_GPIO_PORT								GPIOA

// Button B4
#define BUTTONB4_GPIO_PIN								GPIO_Pin_0
#define BUTTONB4_GPIO_PORT								GPIOB

#define SYSFG_Clock										RCC_APB2Periph_SYSCFG

#define PRESS_TIMEOUT									500

#define RELEASE_TIMEOUT									400
typedef enum {
	NO_CLICK = 0x00,
	BUTTON_PRESSED = 0x01,
	BUTTON_RELEASED = 0x02
} ButtonState;

typedef struct {
	ButtonState State;
	u32_t timePress;
	u32_t timeReleased;
	u32_t Count;
} ButtonName_t;

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

u8_t g_byStatus = 0;
u32_t g_dwStartTimerB3 = 0;
ButtonName_t buttonB2;
ButtonName_t buttonB4;

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
static void_t ledBuzzer_Init(void_t);
static void_t interruptPA4_Init(void_t);
static void_t interruptPA0_Init(void_t);
static void_t interruptPB3_Init(void_t);

static void_t ledControlStatus(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN_ID, u8_t byStatus);
static void_t toggleLed_5Times(void_t);
static void_t buzzerControlSetBeep(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN, u32_t num);
static void_t blinkLedStatusPower(GPIO_TypeDef *GPIOx1, uint16_t GPIO_PIN_ID1,
								 GPIO_TypeDef *GPIOx2, uint16_t GPIO_PIN_ID2, u32_t num);
static u32_t calculatorTime(u32_t dwTimeInit, u32_t dwTimeCurrent);
static void_t LedControl_TimPressRealease(void_t);
static void_t delay_ms(u32_t dwMillisecond);
/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

/******************************************************************************/

int main(void_t) {
	buttonB2.Count = 0;
	SystemCoreClockUpdate();
	ledBuzzer_Init();
	TimerInit();
	interruptPA4_Init();
	interruptPB3_Init();
	interruptPA0_Init();
	blinkLedStatusPower(LEDGREEN1_GPIO_PORT, LEDGREEN1_GPIO_PIN,
						LEDGREEN2_GPIO_PORT, LEDGREEN2_GPIO_PIN, 4);
	while (1) {
		toggleLed_5Times();
		LedControl_TimPressRealease();

	}
}

/**
 * @func   delay_ms
 * @brief  delay millisecond
 * @param  milisecond
 * @retval None
 */
static void_t delay_ms(u32_t dwMillisecond) {

	u32_t dwStartTime = GetMilSecTick(); //Lưu lại thời điểm hiện tại.
	while (calculatorTime(dwStartTime, GetMilSecTick()) <= dwMillisecond);

//	Đợi cho đến khi hết khoảng time cài đặt

}

/**
 * @func   ledBuzzer_Init
 * @brief  Init Buuzzer and Led
 * @param  None
 * @retval None
 */
static void_t ledBuzzer_Init(void_t) {
	//Declare type variable GPIO Struct------------------------------------------------
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(
			RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC,
			ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

//	Port A
	GPIO_InitStructure.GPIO_Pin = LEDGREEN1_GPIO_PIN | LEDGREEN2_GPIO_PIN
			| LEDRED1_GPIO_PIN | LEDBLUE1_GPIO_PIN | LEDBLUE2_GPIO_PIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

//	Port B
	GPIO_InitStructure.GPIO_Pin = LEDRED2_GPIO_PIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

//  Port C
	GPIO_InitStructure.GPIO_Pin = BUZZER_GPIO_PIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
 * @func   interruptPA4_Init
 * @brief  Init Interrupt
 * @param  None
 * @retval None
 */
static void_t interruptPA4_Init(void_t) {
	GPIO_InitTypeDef GPIO_InitStructure;	
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

// Enable Clock Port A;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = BUTTONB3_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_Init(BUTTONB3_GPIO_PORT, &GPIO_InitStructure);

//  Enable Clock Syscfg, Connect EXTI Line 4 to PA4 pin

	RCC_APB2PeriphClockCmd(SYSFG_Clock, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource4);

//	Configuration Interrupt

	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

// Configuration NVIC

	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
}

/**
 * @func   interruptPA0_Init
 * @brief  Init Interrupt
 * @param  None
 * @retval None
 */
static void_t interruptPA0_Init(void_t) {
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

// Enable Clock Port B;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = BUTTONB4_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_Init(BUTTONB4_GPIO_PORT, &GPIO_InitStructure);

//  Enable Clock Syscfg, Connect EXTI Line 0 to PB0 pin

	RCC_APB2PeriphClockCmd(SYSFG_Clock, ENABLE);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);

//	Configuration Interrupt

	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

// Configuration NVIC

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
}

/**
 * @func   interruptPB3_Init
 * @brief  Init Interrupt
 * @param  None
 * @retval None
 */
static void_t interruptPB3_Init(void_t) {
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

// Enable Clock Port C;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = BUTTONB2_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_Init(BUTTONB2_GPIO_PORT, &GPIO_InitStructure);

//  Enable Clock Syscfg, Connect EXTI Line 3 to PB3 pin

	RCC_APB2PeriphClockCmd(SYSFG_Clock, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource3);

//	Configuration Interrupt

	EXTI_InitStructure.EXTI_Line = EXTI_Line3;

	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

// Configuration NVIC

	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
}

/**
 * @func   EXTI4_IRQHandler
 * @brief  Interrupt line 4
 * @param  None
 * @retval None
 */
void_t EXTI4_IRQHandler(void_t) {
	if (EXTI_GetFlagStatus(EXTI_Line4) == SET) {
		if (GPIO_ReadInputDataBit(BUTTONB3_GPIO_PORT,BUTTONB3_GPIO_PIN)== GPIO_PIN_RESET) {
			g_dwStartTimerB3 = GetMilSecTick();	// Lưu thời gian nhấn nút
		}
		else
		{
			g_byStatus++;						// Khi nhả nút thì biến tăng
		}
	}
	//xóa cờ ngắt sau khi thực hiện xong chương trình ngắt.
	EXTI_ClearITPendingBit(EXTI_Line4);
}

/**
 * @func   EXTI3_IRQHandler
 * @brief  Interrupt line 3
 * @param  None
 * @retval None
 */
void_t EXTI3_IRQHandler(void_t) {
	if (EXTI_GetFlagStatus(EXTI_Line3) == SET) {

		if (GPIO_ReadInputDataBit(BUTTONB2_GPIO_PORT,BUTTONB2_GPIO_PIN)== GPIO_PIN_RESET) {
			buttonB2.State = BUTTON_PRESSED;
			buttonB2.timePress = GetMilSecTick();
			buttonB2.Count++;
		} else {

			buttonB2.timeReleased = GetMilSecTick();
			buttonB2.State = BUTTON_RELEASED;
		}
	}
	//xóa cờ ngắt sau khi thực hiện xong chương trình ngắt.
	EXTI_ClearITPendingBit(EXTI_Line3);
}

/**
 * @func   EXTI0_IRQHandler
 * @brief  Interrupt line 0
 * @param  None
 * @retval None
 */
void_t EXTI0_IRQHandler(void_t) {
	if (EXTI_GetFlagStatus(EXTI_Line0) == SET) {
		if (GPIO_ReadInputDataBit(BUTTONB4_GPIO_PORT,BUTTONB4_GPIO_PIN)== GPIO_PIN_RESET) {
			buttonB4.State = BUTTON_PRESSED;
			buttonB4.timePress = GetMilSecTick();
			buttonB4.Count++;
		} else {
			buttonB4.timeReleased = GetMilSecTick();
			buttonB4.State = BUTTON_RELEASED;
		}
	}
	//xóa cờ ngắt sau khi thực hiện xong chương trình ngắt.
	EXTI_ClearITPendingBit(EXTI_Line0);
}

/**
 * @func   ledControlStatus
 * @brief  set status LED
 * @param  PORT, PIN
 * @retval None
 */
static void_t ledControlStatus(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN,
		u8_t byStatus) {
	// SET bit in BSRR Registers

	if (byStatus == GPIO_PIN_SET) {
		GPIOx->BSRRL = GPIO_PIN;
	}
	if (byStatus == GPIO_PIN_RESET) {
		GPIOx->BSRRH = GPIO_PIN;
	}
}

/**
 * @func   buzzerControlSetBeep
 * @brief  set status buzzer
 * @param  PORT, PIN
 * @retval None
 */
static void_t buzzerControlSetBeep(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN,
		u32_t num) {
	for (u32_t i = 0; i < num; i++) {
		GPIO_SetBits(GPIOx, GPIO_PIN);
		delay_ms(200);
		GPIO_ResetBits(GPIOx, GPIO_PIN);
		delay_ms(200);
	}
}

/**
 * @func   calculatorTime
 * @brief  Caculator time button
 * @param  time
 * @retval u32_t
 */
static u32_t calculatorTime(u32_t dwTimeInit, u32_t dwTimeCurrent) {
	u32_t dwTimeTotal;
	if (dwTimeCurrent >= dwTimeInit) {
		dwTimeTotal = dwTimeCurrent - dwTimeInit;
	} else {
		dwTimeTotal = 0xFFFFFFFFU + dwTimeCurrent - dwTimeInit;
	}
	return dwTimeTotal;

}

/**
 * @func   toggleLed_5Times
 * @brief  Toggle led number times
 * @param  None
 * @retval None
 */
static void_t toggleLed_5Times(void_t) {
	if (g_byStatus == 5) {
		delay_ms(200);
		blinkLedStatusPower(LEDGREEN1_GPIO_PORT, LEDGREEN1_GPIO_PIN,
		LEDGREEN2_GPIO_PORT, LEDGREEN2_GPIO_PIN, 5);
		buzzerControlSetBeep(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN, 2);
		g_byStatus = 0;
	}
	else
	{
		// check timeout between button presses
		if(calculatorTime(g_dwStartTimerB3, GetMilSecTick()) > PRESS_TIMEOUT) //
		{
			g_byStatus = 0;
		}
	}
}

/**
 * @func   blinkLedStatusPower
 * @brief  Led blink
 * @param  PORT, PIN
 * @retval None
 */
static void_t blinkLedStatusPower(GPIO_TypeDef *GPIOx1, uint16_t GPIO_PIN_ID1,
		GPIO_TypeDef *GPIOx2, uint16_t GPIO_PIN_ID2, u32_t num) {
	for (u32_t i = 0; i < num; i++) {
		ledControlStatus(GPIOx1, GPIO_PIN_ID1, GPIO_PIN_HIGH);
		ledControlStatus(GPIOx2, GPIO_PIN_ID2, GPIO_PIN_HIGH);
		delay_ms(100);
		ledControlStatus(GPIOx1, GPIO_PIN_ID1, GPIO_PIN_LOW);
		ledControlStatus(GPIOx2, GPIO_PIN_ID2, GPIO_PIN_LOW);
		delay_ms(100);
	}
}

/**
 * @func   LedControl_TimPressRealease
 * @brief  Control led with state button
 * @param  None
 * @retval None
 */

static void_t LedControl_TimPressRealease(void_t) {
//	BUTTON B2****************************************************************************************************
	// check holding button
	if (buttonB2.State == BUTTON_PRESSED) {
		if (calculatorTime(buttonB2.timePress, GetMilSecTick()) > PRESS_TIMEOUT) {
			buttonB2.Count = 0;
			ledControlStatus(LEDBLUE2_GPIO_PORT, LEDBLUE2_GPIO_PIN,GPIO_PIN_HIGH);
		}
	}

	// check release button
	if (buttonB2.State == BUTTON_RELEASED) {

		ledControlStatus(LEDBLUE2_GPIO_PORT, LEDBLUE2_GPIO_PIN, GPIO_PIN_LOW);
		if (buttonB2.Count == 1) {
				if (calculatorTime(buttonB2.timeReleased, GetMilSecTick()) > RELEASE_TIMEOUT)
					buttonB2.Count = 0;

		}
		if (buttonB2.Count == 2) {
			ledControlStatus(LEDBLUE2_GPIO_PORT, LEDBLUE2_GPIO_PIN, GPIO_PIN_HIGH);
		}
		if (buttonB2.Count >= 3) {
			ledControlStatus(LEDBLUE2_GPIO_PORT, LEDBLUE2_GPIO_PIN, GPIO_PIN_LOW);
			buttonB2.Count = 0;
		}
	}
	if (buttonB2.State == NO_CLICK) {

	}

//BUTTON B4****************************************************************************************************
	if (buttonB4.State == BUTTON_PRESSED) {
		if (calculatorTime(buttonB4.timePress, GetMilSecTick()) > PRESS_TIMEOUT) {			// Kiểm tra xem nút có đang được giữ không
			buttonB4.Count = 0;
			ledControlStatus(LEDRED2_GPIO_PORT, LEDRED2_GPIO_PIN,GPIO_PIN_HIGH);
		}
	}
	if (buttonB4.State == BUTTON_RELEASED) {

		ledControlStatus(LEDRED2_GPIO_PORT, LEDRED2_GPIO_PIN, GPIO_PIN_LOW);
		if (buttonB4.Count == 1) {
				if (calculatorTime(buttonB4.timeReleased, GetMilSecTick()) > RELEASE_TIMEOUT)  // Kiểm tra khoảng thời gian giữa 2 lần nhấn nút
					buttonB4.Count = 0;
		}
		if (buttonB4.Count == 2) {
			ledControlStatus(LEDRED2_GPIO_PORT, LEDRED2_GPIO_PIN, GPIO_PIN_HIGH);
		}
		if (buttonB4.Count >= 3) {
			ledControlStatus(LEDRED2_GPIO_PORT, LEDRED2_GPIO_PIN, GPIO_PIN_LOW);
			buttonB4.Count = 0;
		}
	}
	if (buttonB4.State == NO_CLICK) {

	}
}
