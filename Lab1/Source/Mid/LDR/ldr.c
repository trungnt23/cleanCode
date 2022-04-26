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
 * Last Changed:     $Date: $April 26, 2022
 *
 * Code sample:
 ******************************************************************************/
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include <math.h>
#include "app/framework/include/af.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_adc.h"
#include "em_iadc.h"
#include "em_gpio.h"
#include "kalman_filter.h"
#include "ldr.h"

/******************************************************************************/
/*                     PRIVATE TYPES and DEFINITIONS                         */
/******************************************************************************/


/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/


/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/
EmberEventControl readValueSensorLightControl;
volatile u32_t g_dwRegistor;
volatile u32_t g_dwLux = 0;
u32_t g_dwValueADC=0;
u32_t g_dwKalmanLight=0;
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
 * @func    ldr_Init
 * @brief   LDR initialize
 * @param   None
 * @retval  None
 */
void_t ldr_Init(void_t)
{
	// Declare init structs
	  IADC_Init_t init = IADC_INIT_DEFAULT;
	  IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
	  IADC_InitSingle_t initSingle = IADC_INITSINGLE_DEFAULT;
	  IADC_SingleInput_t initSingleInput = IADC_SINGLEINPUT_DEFAULT;

	  // Enable IADC clock
	  CMU_ClockEnable(cmuClock_IADC0, true);
	  // enable pin

	  // Reset IADC to reset configuration in case it has been modified
	  IADC_reset(IADC0);

	  // Configure IADC clock source for use while in EM2
	  CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);

	  // Modify init structs and initialize
	  init.warmup = iadcWarmupKeepWarm;

	  // Set the HFSCLK prescale value here
	  init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);

	  // Configuration 0 is used by both scan and single conversions by default
	  // Use unbuffered AVDD as reference
	  initAllConfigs.configs[0].reference = iadcCfgReferenceVddx;

	  // Divides CLK_SRC_ADC to set the CLK_ADC frequency for desired sample rate
	  initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,
	                                                                    CLK_ADC_FREQ,
	                                                                    0,
	                                                                    iadcCfgModeNormal,
	                                                                    init.srcClkPrescale);

	  // Set oversampling rate to 32x
	  // resolution formula res = 12 + log2(oversampling * digital averaging)
	  // in this case res = 12 + log2(32*1) = 16
	  initAllConfigs.configs[0].osrHighSpeed = iadcCfgOsrHighSpeed32x;


	  // Single initialization
	  initSingle.dataValidLevel = _IADC_SINGLEFIFOCFG_DVL_VALID1;

	  // Set conversions to run continuously
	//  initSingle.triggerAction = iadcTriggerActionContinuous;

	  // Set alignment to right justified with 16 bits for data field
	//  initSingle.alignment = iadcAlignRight16;

	  // Configure Input sources for single ended conversion
	  initSingleInput.posInput = iadcPosInputPortCPin5;
	  initSingleInput.negInput = iadcNegInputGnd;

	  // Initialize IADC
	  // Note oversampling and digital averaging will affect the offset correction
	  // This is taken care of in the IADC_init() function in the emlib
	  IADC_init(IADC0, &init, &initAllConfigs);

	  // Initialize Scan
	  IADC_initSingle(IADC0, &initSingle, &initSingleInput);

	  // Allocate the analog bus for ADC0 inputs
	  GPIO->IADC_INPUT_0_BUS |= GPIO_CDBUSALLOC_CDODD0_ADC0;  //IADC_INPUT_BUSALLOC
}

/**
 * @func   LightSensor_AdcPollingReadHandler
 * @brief  Read value from ADC
 * @param  None
 * @retval None
 */
u32_t lightSensorPollingRead(void_t)
{
	emberEventControlSetInactive(readValueSensorLightControl);
	IADC_Result_t iadcResult;
	// Start IADC conversion
	IADC_command(IADC0, iadcCmdStartSingle);

	// Wait for conversion to be complete
	while((IADC0->STATUS & (_IADC_STATUS_CONVERTING_MASK
				| _IADC_STATUS_SINGLEFIFODV_MASK)) != IADC_STATUS_SINGLEFIFODV); //while combined status bits 8 & 6 don't equal 1 and 0 respectively

	// Get ADC result
	iadcResult = IADC_pullSingleFifoResult(IADC0);
	g_dwValueADC = iadcResult.data;
	g_dwKalmanLight = Kalman_sensor(g_dwValueADC);
	// Calculate input voltage:
	//  For differential inputs, the resultant range is from -Vref to +Vref, i.e.,
	//  for Vref = AVDD = 3.30V, 12 bits represents 6.60V full scale IADC range.
	g_dwRegistor= 10000*(3300 - g_dwKalmanLight)/(g_dwKalmanLight);    // registor  = 10K*ADC / (4095 -ADC)
	g_dwLux = abs(316*pow(10,5)*pow(g_dwRegistor,-1.4));
	return g_dwLux;
}

