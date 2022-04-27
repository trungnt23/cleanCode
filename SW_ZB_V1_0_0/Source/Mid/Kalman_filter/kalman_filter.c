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
 * File name: kalman_filer.h
 *
 * Description:
 *
 *
 * Last Changed By:  $Author: trungnt $
 * Revision:         $Revision: $
 * Last Changed:     $Date: $April 15, 2022
 *
 *
 ******************************************************************************/
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include "app/framework/include/af.h"
#include <math.h>
#include "kalman_filter.h"
/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/
float_t _err_measure;
float_t _err_estimate;
float_t _q;
float_t _current_estimate;
float_t _last_estimate;
float_t _kalman_gain;
/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/
/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */

/******************************************************************************/
/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/
/*
 */
void_t kalmanFilterInit(float_t mea_e, float_t est_e, float_t q)
{
  _err_measure=mea_e;
  _err_estimate=est_e;
  _q = q;
}

float_t kalmanFilter_updateEstimate(float_t mea)
{
  _kalman_gain = _err_estimate/(_err_estimate + _err_measure);
  _current_estimate = _last_estimate + _kalman_gain * (mea - _last_estimate);
  _err_estimate =  (1.0 - _kalman_gain)*_err_estimate + fabs(_last_estimate-_current_estimate)*_q;
  _last_estimate=_current_estimate;

  return _current_estimate;
}

void_t kalmanFilter_setMeasurementError(float_t mea_e)
{
  _err_measure=mea_e;
}

void_t kalmanFilter_setEstimateError(float_t est_e)
{
  _err_estimate=est_e;
}

void_t kalmanFilter_setProcessNoise(float_t q)
{
  _q=q;
}

float_t kalmanFilter_getkalmanGain(void_t) {
  return _kalman_gain;
}

float_t kalmanFilter_getEstimateError(void_t) {
  return _err_estimate;
}

/**
 * @func   kalman_sensor
 * @brief  Use Kanman filter for Light Sensor
 * @param  sensorValue
 * @retval u32_t
 */
u32_t kalmanMeasure(u32_t sensorValue)
{
	u32_t sensorValueAfterFilter = kalmanFilter_updateEstimate(sensorValue);

	return sensorValueAfterFilter;
}

/* END FILE */
