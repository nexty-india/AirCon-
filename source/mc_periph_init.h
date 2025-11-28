/*
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* This is a generic configuration file of the motor control driver. You need to edit the file.
 * Remove this warning statement after this file is edited manually or
 * re-generate this file using MC_PMSM Config Tool component.
 */

#ifndef _MC_PERIPH_INIT_H_
#define _MC_PERIPH_INIT_H_

#include "fsl_device_registers.h"
#include "mcdrv_eflexpwm_mcxa346.h"
#include "mcdrv_adc_mcxa346.h"
#include "MC_Parameters.h"
#include "PFC_def.h"

/* macro used for TSA table */
#define PMSM_SNSLESS

#define USE_INTERNAL_OPAMPS     (true)

/******************************************************************************
 * Timing - common for motor 1 and motor 2
 ******************************************************************************/
#define MCU_CLOCK_FREQ          (180000000U)                    /* MCU core clock 150 MHz */

/******************************************************************************
 * Timing for motor 1
 ******************************************************************************/
#define M1_PWM_FREQ             (6250U)                        /* PWM frequency in Hz */
#define M1_PWM_MODULO           (MCU_CLOCK_FREQ / M1_PWM_FREQ)  /* PWM modulo = FTM_input_clock / M1_PWM_FREQ */
#define M1_PWM_DEADTIME         (1500)                           /* Output PWM deadtime value in nanoseconds */
#define M1_FOC_FREQ_VS_PWM_FREQ (1U)                            /* PWM vs. Fast control loop ratio */
#define M1_SLOW_LOOP_FREQ       (1000U)                         /* Slow control loop frequency in Hz */

/******************************************************************************
 * Timing for motor 2
 ******************************************************************************/
#define M2_PWM_FREQ             (16000U)                        /* PWM frequency in Hz */
#define M2_PWM_MODULO           (MCU_CLOCK_FREQ / M2_PWM_FREQ)  /* PWM modulo = FTM_input_clock / M2_PWM_FREQ */
#define M2_PWM_DEADTIME         (1500)                           /* Output PWM deadtime value in nanoseconds */
#define M2_FOC_FREQ_VS_PWM_FREQ (2U)                            /* PWM vs. Fast control loop ratio */
#define M2_SLOW_LOOP_FREQ       (1000U)                         /* Slow control loop frequency in Hz */

/* PFC frequency in Hz*/
/* PFC PWM modulo = Input_clock / PFC_PWM_FREQ */
#define PFC_PWM_MODULO           (MCU_CLOCK_FREQ / PFC_PWM_FREQ / 1000)  
 /******************************************************************************
  * Output control
  ******************************************************************************/
/* DC bus braking resistor control */
#define M1_BRAKE_SET()
#define M2_BRAKE_SET()
#define M1_BRAKE_CLEAR()
#define M2_BRAKE_CLEAR()

/******************************************************************************
 * ADC measurement definition
 ******************************************************************************/

/******************************************************************************
 * MC driver macro definition and check - do not change this part
 ******************************************************************************/
/******************************************************************************
 * Define motor 1 ADC control functions
 ******************************************************************************/
#define M1_MCDRV_ADC_GET(par)	(PhaseCurrentReconstruction(par))
#define M1_MCDRV_CURR_3PH_CHAN_ASSIGN(par)  (MCDRV_ADCChannelMapping(par))
#define M1_MCDRV_CURR_3PH_CALIB_INIT(par) (MCDRV_Curr3Ph2ShCalibInit(par))
#define M1_MCDRV_CURR_3PH_CALIB(par) (MCDRV_Curr3Ph2ShCalib(par))
#define M1_MCDRV_CURR_3PH_CALIB_SET(par) (MCDRV_Curr3Ph2ShCalibSet(par))

/******************************************************************************
 * Define motor 2 ADC control functions
 ******************************************************************************/
#define M2_MCDRV_ADC_GET(par)	(MCDRV_FanCurrentGet(par))
#define M2_MCDRV_CURR_3PH_CHAN_ASSIGN(par) (MCDRV_ADCChannelMapping(par))
#define M2_MCDRV_CURR_3PH_CALIB_INIT(par) (MCDRV_Curr3Ph2ShCalibInit(par))
#define M2_MCDRV_CURR_3PH_CALIB(par) (MCDRV_Curr3Ph2ShCalib(par))
#define M2_MCDRV_CURR_3PH_CALIB_SET(par) (MCDRV_Curr3Ph2ShCalibSet(par))
              
/******************************************************************************
 * Define PFC ADC control functions
 ******************************************************************************/
#define PFC_MCDRV_ADC_GET(par)	(PFC_CurrAndVoltDcBusGet(par))
#define M2_MCDRV_CURR_3PH_CALIB_INIT(par) (MCDRV_Curr3Ph2ShCalibInit(par))
#define M2_MCDRV_CURR_3PH_CALIB(par) (MCDRV_Curr3Ph2ShCalib(par))
#define M2_MCDRV_CURR_3PH_CALIB_SET(par) (MCDRV_Curr3Ph2ShCalibSet(par))
              
/******************************************************************************
 * Define motor 1 3-ph PWM control functions
 ******************************************************************************/
#define M1_MCDRV_PWM3PH_SET(par) (MCDRV_eFlexPwm3PhSet(par))
#define M1_MCDRV_PWM3PH_EN(par) (MCDRV_eFlexPwm3PhOutEn(par))
#define M1_MCDRV_PWM3PH_DIS(par) (MCDRV_eFlexPwm3PhOutDis(par))
#define M1_MCDRV_PWM3PH_FLT_GET(par) (MCDRV_eFlexPwm3PhFltGet(par))
#define M1_MCDRV_P2M3PH_FLT_TRY_CLR(par) (MCDRV_eFlexPwm3PhFltTryClr(par))

/******************************************************************************
 * Define motor 2 3-ph PWM control functions
 ******************************************************************************/
#define M2_MCDRV_PWM3PH_SET(par) (MCDRV_eFlexPwm3PhSet(par))
#define M2_MCDRV_PWM3PH_EN(par) (MCDRV_eFlexPwm3PhOutEn(par))
#define M2_MCDRV_PWM3PH_DIS(par) (MCDRV_eFlexPwm3PhOutDis(par))
#define M2_MCDRV_PWMBOTTOM_EN(par) (MCDRV_eFlexPwm3PhBottomOutEn(par))
#define M2_MCDRV_PWM3PH_FLT_GET(par) (MCDRV_eFlexPwm3PhFltGet(par))
#define M2_MCDRV_P2M3PH_FLT_TRY_CLR(par) (MCDRV_eFlexPwm3PhFltTryClr(par))

/******************************************************************************
 * Define PFC PWM control functions
 ******************************************************************************/

/******************************************************************************
 * Define FreeMASTER functions
 ******************************************************************************/
/* FreeMASTER_Recorder_0's buffer size definition */
#define FREEMASTER_REC_0_SIZE 4096*4
/******************************************************************************
 * Global typedefs
 ******************************************************************************/
/* Structure used during clocks and modulo calculations */
typedef struct _clock_setup
{
    /* Common variables for motor 1 and motor 2 */
    uint32_t ui32FastPeripheralClock;
    uint32_t ui32CpuFrequency;
    uint32_t ui32BusClock;
    uint32_t ui32SysPllClock;

    /* Variables for motor 1 */
    uint16_t ui16M1SpeedLoopFreq;
    uint16_t ui16M1SpeedLoopModulo;
    uint16_t ui16M1PwmFreq;
    uint16_t ui16M1PwmModulo;
    uint16_t ui16M1PwmDeadTime;

    /* Variables for motor 2 */
    uint16_t ui16M2SpeedLoopFreq;
    uint16_t ui16M2SpeedLoopModulo;
    uint16_t ui16M2PwmFreq;
    uint16_t ui16M2PwmModulo;
    uint16_t ui16M2PwmDeadTime;
} clock_setup_t;

/******************************************************************************
 * Global variable definitions
 ******************************************************************************/

extern uint64_t g_sys_counter;
extern mcdrv_eflexpwm_t g_sM1Pwm3ph;
extern mcdrv_eflexpwm_t g_sM2Pwm3ph;
extern mcdrv_adc_t g_sM1AdcSensor;
extern mcdrv_adc_t g_sM2AdcSensor;
extern mcdrv_adc_t g_sPFCAdcSensor;
extern clock_setup_t g_sClockSetup;
extern bool user_delay(uint32_t delay_ms, uint32_t *last_call_ms);
extern void InitInputmux(void);
extern void FMSTR_Recorder_Create(void);
/*******************************************************************************
 * API
 ******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
void MCDRV_Init(void);

#ifdef __cplusplus
}
#endif
#endif /* _MC_PERIPH_INIT_H_  */
