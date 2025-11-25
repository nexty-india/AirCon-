/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CURRENT_RECONSTRUCTION_H_
#define CURRENT_RECONSTRUCTION_H_

#include "mlib_types.h"
#include "gmclib.h"
#include "mc_periph_init.h"
/******************************************************************************
* Macros
******************************************************************************/
#define PWM_CLOCK	180.0 //MHz
#define PWM_PERIOD	(1000000.0/M1_PWM_FREQ) // us
#define CRITICAL_VECTOR_LAST_TIME 4.9F//2.95 // us
#define HW_DELAY_0	2.227//us
#define HW_DELAY_1	2.227//2.45//us
#define HW_DELAY_2	2.227//us
#define CRITICAL_VECTOR_LAST_TIME_F FRAC16((2*CRITICAL_VECTOR_LAST_TIME)/PWM_PERIOD)
#define HW_DELAY_0_F	(frac16_t)(HW_DELAY_0*PWM_CLOCK)
#define HW_DELAY_1_F	(frac16_t)(HW_DELAY_1*PWM_CLOCK)
#define HW_DELAY_2_F	(frac16_t)(HW_DELAY_2*PWM_CLOCK)
#define VAL1_F		(frac16_t)(PWM_PERIOD*PWM_CLOCK*0.5)

/******************************************************************************
* Types
******************************************************************************/
typedef struct
{
	frac16_t w16TrigTimePoint1;
	frac16_t w16TrigTimePoint2;
	frac16_t w16TrigTimePoint3;
	frac16_t w16TrigTimePoint4;
	frac16_t w16TrigTimePoint5;
} ONE_SHUNT_TRG_T;

typedef struct
{
	int t_1;
	int t_2;
}SVM_T1_T2;

typedef struct
{
	frac16_t samp[3];			/* start time points of basic vectors, in the sequence of sampling, in unit of PWM clock cycles */
	frac16_t samp_scaled[3];	/* three final sampling points, in unit of PWM clock cycles */
	frac16_t critical_sector;	/* minimal vector lasting time, in unit of ratio */
	frac16_t t_delay1;		/* delay for t_1 if t_1 < minimal vector lasting time, in unit of ratio */
	frac16_t t_delay2;		/* delay for t_2 if t_2 < minimal vector lasting time, in unit of ratio */
	frac16_t adc_samples[3];	/* the original sampled values of DC current at three points */
	frac16_t pwm_delay[6];	/* edge delay for SM0~2, in unit of ratio */
	frac16_t dutyCycleChannel0; /* value of SM0 val2, in unit of PWM clock cycles  */
        frac16_t dutyCycleChannel1;
	frac16_t dutyCycleChannel2; /* value of SM1 val2, in unit of PWM clock cycles  */
        frac16_t dutyCycleChannel3;
	frac16_t dutyCycleChannel4; /* value of SM2 val2, in unit of PWM clock cycles  */
        frac16_t dutyCycleChannel5;
	frac16_t samp_t1_scaled;	  /* half of vector_t1 lasting time, in unit of PWM clock cycles */
	frac16_t samp_t2_scaled;	  /* half of vector_t2 lasting time, in unit of PWM clock cycles */
	frac16_t delay_0;			  /* compensation of hw delay on pwm channels, in unit of PWM clock cycles */
	frac16_t delay_1;			  /* compensation of hw delay on pwm channels, in unit of PWM clock cycles */
	frac16_t delay_2;			  /* compensation of hw delay on pwm channels, in unit of PWM clock cycles*/
	frac16_t shiftFlag1;		  /* indicates if there's a phase shift */
	frac16_t shiftFlag2;
}PHASE_CURRENTS;

/******************************************************************************
* Global variables
******************************************************************************/
extern PHASE_CURRENTS phaseCurrents;
extern SVM_T1_T2 t1_t2;
extern ONE_SHUNT_TRG_T mcTrigPointCompressor;
/******************************************************************************
* Global functions
******************************************************************************/

/***************************************************************************//*!
*
* @brief  Space-vector modulation technique
*
* @param  p_AlphaBeta  pointer to a structure with alpha,beta components of
*         stator voltage vector
* @param  p_abc  pointer to a structure of duty-cycles of phases a,b,c
* @param  t_1_t_2  pointer to a structure containing t1 and t2 intervals for
*         a 3-ph current reconstruction
*
* @return This function returns a sector number, where the stator voltage
*         vector resides in.
*
* @remarks The linear arithmetic on the R0 and R1 AGU registers required
* (i.e. M01 register must contain 0xFFFF before execution of this module.)
*
*  Function calculates appropriate duty cycles needed to generate
*  a given stator reference voltage using the Space Vector Modulation with
*  a duty cycle of the null switching state from states O000 and O111
*  in each sector of the hexagon. As a additional result a function returns
*  time interval t1 and t2 used in 3-phase current reconstruction.
*
****************************************************************************/
void PhaseShift_calculation(PHASE_CURRENTS *p_phaseCurrents,SVM_T1_T2 *p_t12,frac16_t svm_sector);
void Current_samplingSetup(PHASE_CURRENTS *p_phaseCurrents,SVM_T1_T2 *p_t12,frac16_t svm_sector);
void Current_reconstruction_init(PHASE_CURRENTS *p_phaseCurrents);
extern int16_t SvmU0n_t1t2(GMCLIB_2COOR_ALBE_T_F16 *pUAlBe, GMCLIB_3COOR_T_F16 *pDutyABC, SVM_T1_T2 *pT1T2,PHASE_CURRENTS *p_phaseCurrents);
#endif /* CURRENT_RECONSTRUCTION_H_ */
