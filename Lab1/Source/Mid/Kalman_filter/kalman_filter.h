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
 * Code sample:
 ******************************************************************************/
#ifndef SOURCE_MID_KALMAN_FILTER_KALMAN_FILTER_H_
#define SOURCE_MID_KALMAN_FILTER_KALMAN_FILTER_H_
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include <typedefs.h>
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

void_t KalmanFilterInit(float_t mea_e, float_t est_e, float_t q);
float_t KalmanFilter_updateEstimate(float_t mea);
void_t KalmanFilter_setMeasurementError(float_t mea_e);
void_t KalmanFilter_setEstimateError(float_t est_e);
void_t KalmanFilter_setProcessNoise(float_t q);
float_t KalmanFilter_getKalmanGain(void_t);
float_t KalmanFilter_getEstimateError(void_t);
u32_t Kalman_sensor(u32_t sensorValue);
#endif /* SOURCE_MID_KALMAN_FILTER_KALMAN_FILTER_H_ */
