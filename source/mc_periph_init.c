/*
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "mc_periph_init.h"
#include "peripherals.h"
#include "fsl_common.h"

#include "fsl_gpio.h"
#include "fsl_ctimer.h"
#include "fsl_opamp.h"
#include "fsl_inputmux.h"
#include "fsl_aoi.h"
#include "fsl_lpcmp.h"
#include "fsl_spc.h"
#include "fsl_lpuart.h"
#include "freemaster.h"
#include "freemaster_serial_lpuart.h"
/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedef
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* Motor 1 */
static void InitADC0(void);
static void InitPWM0(void);
static void InitOpAmps(void);

/* Motor 2 */
static void InitADC1(void);
static void InitPWM1(void);
static void InitLpCmp0(void);

/* Common for motor 1 and motor 2 */
static void InitSlowLoop(void);
static void InitClock(void);
static void InitUART1(void);
static void InitAOI0(void);
static void InitAOI1(void);
/*******************************************************************************
 * Variables
 ******************************************************************************/

uint64_t current_time;
uint64_t g_sys_counter;

/* configuration structure for 3-phase PWM mc driver */
mcdrv_eflexpwm_t g_sM1Pwm3ph;
mcdrv_eflexpwm_t g_sM2Pwm3ph;

/* structure for current and voltage measurement*/
mcdrv_adc_t g_sM1AdcSensor;
mcdrv_adc_t g_sM2AdcSensor;
mcdrv_adc_t g_sPFCAdcSensor;

/* Clock setup structure */
clock_setup_t g_sClockSetup;
/*******************************************************************************
 * Local functions
 ******************************************************************************/
/*!
 * @brief   void InitPWM0(void)
 *           - Initialization of the eFlexPWM0 peripheral for motor M1
 *           - 3-phase center-aligned PWM
 *
 * @param   void
 *
 * @return  none
 */
static void InitPWM0(void)//PWM0_SM0-2 for compressor sampling and control，SM3 for PFC sampling and control interrupt
{
  PWM_Type *PWMBase = (PWM_Type *)FLEXPWM0;

  /* eFlexPWM0 init*/
  SYSCON->PWM0SUBCTL = (SYSCON_PWM0SUBCTL_CLK0_EN_MASK | SYSCON_PWM0SUBCTL_CLK1_EN_MASK | SYSCON_PWM0SUBCTL_CLK2_EN_MASK | SYSCON_PWM0SUBCTL_CLK3_EN_MASK); //Enable Sub-module0 clock
  CLOCK_EnableClock(kCLOCK_GateFLEXPWM0);

  /* value register initial values, duty cycle 50% */
  PWMBase->SM[0].INIT = PWM_INIT_INIT((uint16_t)(-(M1_PWM_MODULO / 2)));
  PWMBase->SM[1].INIT = PWM_INIT_INIT((uint16_t)(-(M1_PWM_MODULO / 2)));
  PWMBase->SM[2].INIT = PWM_INIT_INIT((uint16_t)(-(M1_PWM_MODULO / 2)));
  PWMBase->SM[3].INIT = PWM_INIT_INIT((uint16_t)(-(M2_PWM_MODULO * M2_FOC_FREQ_VS_PWM_FREQ / 2)));

  PWMBase->SM[0].VAL1 = PWM_VAL1_VAL1((uint16_t)((M1_PWM_MODULO / 2) - 1));
  PWMBase->SM[1].VAL1 = PWM_VAL1_VAL1((uint16_t)((M1_PWM_MODULO / 2) - 1));
  PWMBase->SM[2].VAL1 = PWM_VAL1_VAL1((uint16_t)((M1_PWM_MODULO / 2) - 1));
  PWMBase->SM[3].VAL1 = PWM_VAL1_VAL1((uint16_t)(M2_PWM_MODULO * M2_FOC_FREQ_VS_PWM_FREQ / 2 - 1));

  PWMBase->SM[0].VAL2 = PWM_VAL2_VAL2((uint16_t)(-(M1_PWM_MODULO / 4)));
  PWMBase->SM[1].VAL2 = PWM_VAL2_VAL2((uint16_t)(-(M1_PWM_MODULO / 4)));
  PWMBase->SM[2].VAL2 = PWM_VAL2_VAL2((uint16_t)(-(M1_PWM_MODULO / 4)));
  PWMBase->SM[3].VAL2 = PWM_VAL2_VAL2((uint16_t)(-(M2_PWM_MODULO * M2_FOC_FREQ_VS_PWM_FREQ / 4)));
    
  PWMBase->SM[0].VAL3 = PWM_VAL3_VAL3((uint16_t)((M1_PWM_MODULO / 4) - 1));
  PWMBase->SM[1].VAL3 = PWM_VAL3_VAL3((uint16_t)((M1_PWM_MODULO / 4) - 1));
  PWMBase->SM[2].VAL3 = PWM_VAL3_VAL3((uint16_t)((M1_PWM_MODULO / 4) - 1));
  PWMBase->SM[3].VAL3 = PWM_VAL3_VAL3((uint16_t)((M2_PWM_MODULO * M2_FOC_FREQ_VS_PWM_FREQ / 4) - 1));

  /* PWM0 module 0 trigger on VAL4 enabled for compressor sampling */
  PWMBase->SM[0].VAL4 = PWM_VAL4_VAL4((uint16_t)(-(M1_PWM_MODULO / 3)));
  PWMBase->SM[0].TCTRL |= PWM_TCTRL_OUT_TRIG_EN(1 << 4);
  /* PWM0 module 0 trigger on VAL5 enabled for compressor sampling */
  PWMBase->SM[0].VAL5 = PWM_VAL5_VAL5((uint16_t)(-(M1_PWM_MODULO / 6)));
  PWMBase->SM[0].TCTRL |= PWM_TCTRL_OUT_TRIG_EN(1 << 5);
  
  /* PWM0 module 1 trigger on VAL4 for compressor current offset sampling */
  PWMBase->SM[1].VAL4 = PWM_VAL4_VAL4((uint16_t)(0));
  PWMBase->SM[1].TCTRL |= PWM_TCTRL_OUT_TRIG_EN(1 << 4);
  
  /* PWM0 module 1 trigger on VAL5 enabled for Inputmux signal connection */
  PWMBase->SM[1].VAL5 = PWM_VAL5_VAL5((uint16_t)((M1_PWM_MODULO / 4) - 1));
  PWMBase->SM[1].INTEN |= PWM_INTEN_CMPIE(1 << 5);
  
  /* PWM0 module 3 trigger on VAL4 enabled for Fan force synchronization and sampling *///PWM0_SM3_OUTTRIG0
  PWMBase->SM[3].VAL4 = PWM_VAL4_VAL4((uint16_t)(-(M2_PWM_MODULO * M2_FOC_FREQ_VS_PWM_FREQ / 2)));
  PWMBase->SM[3].TCTRL |= PWM_TCTRL_OUT_TRIG_EN(1 << 4);
  
  /* set deadtime (number of Fast Peripheral Clocks)
     DTCNT0,1 = T_dead * f_fpc = 1.5us * 180MHz = 270 */
  PWMBase->SM[0].DTCNT0 = ((M1_PWM_DEADTIME * (MCU_CLOCK_FREQ / 1000000U)) / 1000U);
  PWMBase->SM[1].DTCNT0 = ((M1_PWM_DEADTIME * (MCU_CLOCK_FREQ / 1000000U)) / 1000U);
  PWMBase->SM[2].DTCNT0 = ((M1_PWM_DEADTIME * (MCU_CLOCK_FREQ / 1000000U)) / 1000U);
  PWMBase->SM[3].DTCNT0 = 0;

  PWMBase->SM[0].DTCNT1 = ((M1_PWM_DEADTIME * (MCU_CLOCK_FREQ / 1000000U)) / 1000U);
  PWMBase->SM[1].DTCNT1 = ((M1_PWM_DEADTIME * (MCU_CLOCK_FREQ / 1000000U)) / 1000U);
  PWMBase->SM[2].DTCNT1 = ((M1_PWM_DEADTIME * (MCU_CLOCK_FREQ / 1000000U)) / 1000U);
  PWMBase->SM[3].DTCNT1 = 0;

   /* Full cycle reload */
  PWMBase->SM[0].CTRL |= PWM_CTRL_FULL_MASK;
  PWMBase->SM[1].CTRL |= PWM_CTRL_FULL_MASK;
  PWMBase->SM[2].CTRL |= PWM_CTRL_FULL_MASK;
  PWMBase->SM[3].CTRL |= PWM_CTRL_FULL_MASK;
  
  //master_sync_init
  PWMBase->SM[1].CTRL2 = (PWMBase->SM[1].CTRL2 & ~PWM_CTRL2_INIT_SEL_MASK) | PWM_CTRL2_INIT_SEL(0x2);
  PWMBase->SM[2].CTRL2 = (PWMBase->SM[2].CTRL2 & ~PWM_CTRL2_INIT_SEL_MASK) | PWM_CTRL2_INIT_SEL(0x2);    
  //local_sync_init
  PWMBase->SM[3].CTRL2 = (PWMBase->SM[3].CTRL2 & (~(PWM_CTRL2_INIT_SEL_MASK))) | (PWM_CTRL2_INIT_SEL(0x0));
  
  /* Fault trigger settings */
  PWMBase->SM[0].DISMAP[0] = 0xF111U;
  PWMBase->SM[1].DISMAP[0] = 0xF111U;
  PWMBase->SM[2].DISMAP[0] = 0xF111U;
  PWMBase->SM[3].DISMAP[0] = 0xF000U;

  /* PWMs are re-enabled at PWM full cycle */
  PWMBase->FSTS = (PWMBase->FSTS & ~PWM_FSTS_FFULL_MASK) | PWM_FSTS_FFULL(0x3);

  /* PWM fault filter - 3 Fast periph. clocks sample rate, 5 agreeing
     samples to activate */
  PWMBase->FFILT = (PWMBase->FFILT & ~PWM_FFILT_FILT_PER_MASK) | PWM_FFILT_FILT_PER(2);

  /* All interrupts disabled, safe manual fault clearing, inversed logic (trigger level = high) */
  PWMBase->FCTRL &= ~(PWM_FCTRL_FLVL_MASK | PWM_FCTRL_FAUTO_MASK | PWM_FCTRL_FSAFE_MASK | PWM_FCTRL_FIE_MASK); /* clear FCTRL register prior further settings */
  PWMBase->FCTRL |= PWM_FCTRL_FIE(0U); /* FAULT 0 - Interrupt disable */
  PWMBase->FCTRL |= PWM_FCTRL_FLVL(0x0U);
  PWMBase->FCTRL |= PWM_FCTRL_FAUTO(1U);
  PWMBase->FCTRL |= PWM_FCTRL_FSAFE(0xFU);

  /* Clear all fault flags */
  PWMBase->FSTS = (PWMBase->FSTS & ~PWM_FSTS_FFLAG_MASK) | PWM_FSTS_FFLAG(0xF);

  /* Start PWMs (set load OK flags and run - we need to trigger the ADC) */
  PWMBase->MCTRL = (PWMBase->MCTRL & ~PWM_MCTRL_CLDOK_MASK) | PWM_MCTRL_CLDOK(0xF);
  PWMBase->MCTRL = (PWMBase->MCTRL & ~PWM_MCTRL_LDOK_MASK) | PWM_MCTRL_LDOK(0xF);
//  PWMBase->MCTRL = (PWMBase->MCTRL & ~PWM_MCTRL_RUN_MASK) | PWM_MCTRL_RUN(0xF);

  /* eFlexPWM base address */
  g_sM1Pwm3ph.pui32PwmBaseAddress = (PWM_Type *)PWMBase;
  
#if 0
    PWMBase->OUTEN |= PWM_OUTEN_PWMA_EN(0x7);
    PWMBase->OUTEN |= PWM_OUTEN_PWMB_EN(0x7);
#else
    PWMBase->OUTEN &= (~PWM_OUTEN_PWMA_EN(0xF));
    PWMBase->OUTEN &= (~PWM_OUTEN_PWMB_EN(0xF));
#endif
  
    NVIC_SetPriority(FLEXPWM0_SUBMODULE1_IRQn, 1U);//for inputmux signal enable
    NVIC_EnableIRQ(FLEXPWM0_SUBMODULE1_IRQn);
}

/*!
 * @brief   uint64_t get_system_time_ms()
 *           - 1ms slow loop increments
 *
 * @param   uint64_t
 *
 * @return  none
 */

uint64_t get_system_time_ms(void)
{
	return g_sys_counter;
}

/*!
 * @brief   bool user_delay()
 *           - Non-blocking Delay Implementation
 *           - if time elapsed >= delay_ms return true
 *
 * @param   bool
 *
 * @return  none
 */

bool user_delay(uint32_t delay_ms, uint32_t *last_call_ms)
{
	// Remove static variable - use local instead
    current_time = get_system_time_ms();

	// Handle first call
	if (*last_call_ms == 0)
	{
		*last_call_ms = current_time;
		return false;
	}

	// Check for timer completion with overflow protection
	if ((current_time - *last_call_ms) >= delay_ms)
	{
		*last_call_ms = 0;
		return true;
	}

	return false;
}


/*!
 * @brief   void InitADC0(void)
 *           - Initialization of the ADC0 peripheral
 *           - Initialization of the A/D converter for current and voltage sensing
 *
 * @param   void
 *
 * @return  none
 */
static void InitADC0(void)/* for compressor sampling */
{
  lpadc_config_t lpadcConfig;
  
  /* Init the lpadcConfig */
  LPADC_GetDefaultConfig(&lpadcConfig);
  lpadcConfig.enableAnalogPreliminary = true;
  lpadcConfig.powerLevelMode = kLPADC_PowerLevelAlt4;
  lpadcConfig.referenceVoltageSource = kLPADC_ReferenceVoltageAlt3;
  lpadcConfig.conversionAverageMode = kLPADC_ConversionAverage128;

  /* Init ADC */
  lpadc_conv_trigger_config_t lpadcTriggerConfig;
  lpadc_conv_command_config_t lpadcCommandConfig;

  LPADC_Init(ADC0, &lpadcConfig);

  LPADC_DoOffsetCalibration(ADC0);
  LPADC_DoAutoCalibration(ADC0);

  /* Init commands */
  /* Set conversion CMD1 configuration. */
  LPADC_GetDefaultConvCommandConfig(&lpadcCommandConfig);
  lpadcCommandConfig.channelNumber = 2U;                                      /* Set ADC channel ADC0IN2 (Compressor_IDC) */
  lpadcCommandConfig.sampleChannelMode = kLPADC_SampleChannelSingleEndSideA;
  lpadcCommandConfig.conversionResolutionMode = kLPADC_ConversionResolutionHigh;
  lpadcCommandConfig.hardwareAverageMode = kLPADC_HardwareAverageCount1;
  lpadcCommandConfig.sampleTimeMode = kLPADC_SampleTimeADCK3;
  lpadcCommandConfig.chainedNextCommandNumber = 2U;                           /* Next execuited CMD will be CMD2 */
  LPADC_SetConvCommandConfig( ADC0, 1U, &lpadcCommandConfig );                /* Configure the CMD 1 */

  /* Set conversion CMD2 configuration. */
  LPADC_GetDefaultConvCommandConfig(&lpadcCommandConfig);
  lpadcCommandConfig.channelNumber = 2U;                                      
  lpadcCommandConfig.sampleChannelMode = kLPADC_SampleChannelSingleEndSideA;
  lpadcCommandConfig.conversionResolutionMode = kLPADC_ConversionResolutionHigh;
  lpadcCommandConfig.hardwareAverageMode = kLPADC_HardwareAverageCount1;
  lpadcCommandConfig.sampleTimeMode = kLPADC_SampleTimeADCK3;
  lpadcCommandConfig.chainedNextCommandNumber = 3U;
  lpadcCommandConfig.enableWaitTrigger = 1;
  LPADC_SetConvCommandConfig( ADC0, 2U, &lpadcCommandConfig );                /* Configure the CMD 2 */

  /* Set conversion CMD3 configuration. */
  LPADC_GetDefaultConvCommandConfig(&lpadcCommandConfig);
  lpadcCommandConfig.channelNumber = 2U;                                      
  lpadcCommandConfig.sampleChannelMode = kLPADC_SampleChannelSingleEndSideA;
  lpadcCommandConfig.conversionResolutionMode = kLPADC_ConversionResolutionHigh;
  lpadcCommandConfig.hardwareAverageMode = kLPADC_HardwareAverageCount1;
  lpadcCommandConfig.sampleTimeMode = kLPADC_SampleTimeADCK3;
  lpadcCommandConfig.chainedNextCommandNumber = 0U;                           /* Next execuited CMD will be CMD0 */
  lpadcCommandConfig.enableWaitTrigger = 1;
  LPADC_SetConvCommandConfig( ADC0, 3U, &lpadcCommandConfig );                /* Configure the CMD 3 */
  
  /* Init triggers (use trigger 0). */
  LPADC_GetDefaultConvTriggerConfig(&lpadcTriggerConfig);
  lpadcTriggerConfig.targetCommandId = 1U;
  lpadcTriggerConfig.enableHardwareTrigger = true; 
  LPADC_SetConvTriggerConfig(ADC0, 0U, &lpadcTriggerConfig);
  
  /* Enable TCOMP interrupt. */
  LPADC_EnableInterrupts(ADC0, ADC_IE_TCOMP_IE(0x1U));//Only trigger source 0 trigger the interrupt
  NVIC_SetPriority(ADC0_IRQn, 4U);
  NVIC_EnableIRQ(ADC0_IRQn);
}

/*!
@brief   void InitHsCmp0(void)
          - Initialization of the comparator 0 module for dc-bus over current
            detection to generate eFlexPWM0 fault

@param   void

@return  none
*/
static void InitLpCmp0(void)
{
    SPC_EnableActiveModeAnalogModules(SPC0, (kSPC_controlCmp0 | kSPC_controlCmp0Dac));
    lpcmp_config_t mLpcmpConfigStruct;
    lpcmp_dac_config_t mLpcmpDacConfigStruct;
    /*
     *   k_LpcmpConfigStruct->enableStopMode      = false;
     *   k_LpcmpConfigStruct->enableOutputPin     = false;
     *   k_LpcmpConfigStruct->useUnfilteredOutput = false;
     *   k_LpcmpConfigStruct->enableInvertOutput  = false;
     *   k_LpcmpConfigStruct->hysteresisMode      = kLPCMP_HysteresisLevel0;
     *   k_LpcmpConfigStruct->powerMode           = kLPCMP_LowSpeedPowerMode;
     *   k_LpcmpConfigStruct->functionalSourceClock = kLPCMP_FunctionalClockSource0;
     */
    
    LPCMP_GetDefaultConfig(&mLpcmpConfigStruct);
    /* Init the LPCMP module. */
    LPCMP_Init(CMP0, &mLpcmpConfigStruct);
    
    /* Configure the internal DAC to output half of reference voltage. */
    mLpcmpDacConfigStruct.enableLowPowerMode = false;
    mLpcmpDacConfigStruct.referenceVoltageSource = kLPCMP_VrefSourceVin1;//use VDD as DAC reference
    mLpcmpDacConfigStruct.DACValue = 128+FRAC8(2*PFC_IOVER_LIMIT/PFC_I_SCALE);  //20A protect
    LPCMP_SetDACConfig(CMP0, &mLpcmpDacConfigStruct);

    /* Configure LPCMP input channels: ch2 and DAC ch7. */
    LPCMP_SetInputChannels(CMP0, 2, 7);
}

/*!
 * @brief   void InitOpAmps(void)
 *           - Initialization of the Operational Amplifier 0-2 peripheral
 *
 * @param   void
 *
 * @return  none
 */
static void InitOpAmps(void)
{
    SPC_EnableActiveModeAnalogModules(SPC0, (kSPC_controlOpamp0 | kSPC_controlOpamp1 | kSPC_controlOpamp2));
    opamp_config_t opampConfig;
    // IDCB for compressor
    /* OPAMP0 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kOPAMP0_RST_SHIFT_RSTn);
    OPAMP_GetDefaultConfig(&opampConfig);
    opampConfig.compCap     = kOPAMP_FitGain8x;
    opampConfig.biasCurrent = kOPAMP_NoChange;
    OPAMP_Init(OPAMP0, &opampConfig);
    OPAMP_Enable(OPAMP0, true);

    // IW for Fan
    /* OPAMP1 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kOPAMP1_RST_SHIFT_RSTn);
    OPAMP_GetDefaultConfig(&opampConfig);
    opampConfig.compCap     = kOPAMP_FitGain2x;
    opampConfig.biasCurrent = kOPAMP_NoChange;
    OPAMP_Init(OPAMP1, &opampConfig);
    OPAMP_Enable(OPAMP1, true);

    // IV for Fan
    /* OPAMP2 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kOPAMP2_RST_SHIFT_RSTn);
    OPAMP_GetDefaultConfig(&opampConfig);
    opampConfig.compCap     = kOPAMP_FitGain2x;
    opampConfig.biasCurrent = kOPAMP_NoChange;
    OPAMP_Init(OPAMP2, &opampConfig);
    OPAMP_Enable(OPAMP2, true);
}

/*!
 * @brief   void InitPWM1(void)
 *           - Initialization of the eFlexPWM1 peripheral for motor M1
 *           - 3-phase center-aligned PWM
 *
 * @param   void
 *
 * @return  none
 */
static void InitPWM1(void)/* PWM1_SM0-2 for fan control, SM3 for PFC PWM output */
{
  PWM_Type *PWMBase = (PWM_Type *)FLEXPWM1;

  /*eFlexPWM1 init*/
  SYSCON->PWM1SUBCTL = (SYSCON_PWM1SUBCTL_CLK0_EN_MASK | SYSCON_PWM1SUBCTL_CLK1_EN_MASK | SYSCON_PWM1SUBCTL_CLK2_EN_MASK | SYSCON_PWM1SUBCTL_CLK3_EN_MASK); //Enable Sub-module0 clock
  CLOCK_EnableClock(kCLOCK_GateFLEXPWM1);
  
  /* value register initial values, duty cycle 50% */
  PWMBase->SM[0].INIT = PWM_INIT_INIT((uint16_t)(-(M2_PWM_MODULO / 2)));
  PWMBase->SM[1].INIT = PWM_INIT_INIT((uint16_t)(-(M2_PWM_MODULO / 2)));
  PWMBase->SM[2].INIT = PWM_INIT_INIT((uint16_t)(-(M2_PWM_MODULO / 2)));
  PWMBase->SM[3].INIT = PWM_INIT_INIT((uint16_t)(-(PFC_PWM_MODULO / 2)));  

  PWMBase->SM[0].VAL1 = PWM_VAL1_VAL1((uint16_t)((M2_PWM_MODULO / 2) - 1));
  PWMBase->SM[1].VAL1 = PWM_VAL1_VAL1((uint16_t)((M2_PWM_MODULO / 2) - 1));
  PWMBase->SM[2].VAL1 = PWM_VAL1_VAL1((uint16_t)((M2_PWM_MODULO / 2) - 1));
  PWMBase->SM[3].VAL1 = PWM_VAL1_VAL1((uint16_t)((PFC_PWM_MODULO / 2)) - 1);  

  PWMBase->SM[0].VAL2 = PWM_VAL2_VAL2((uint16_t)(-(M2_PWM_MODULO / 4)));
  PWMBase->SM[1].VAL2 = PWM_VAL2_VAL2((uint16_t)(-(M2_PWM_MODULO / 4)));
  PWMBase->SM[2].VAL2 = PWM_VAL2_VAL2((uint16_t)(-(M2_PWM_MODULO / 4)));
  PWMBase->SM[3].VAL2 = PWM_VAL2_VAL2((uint16_t)(-(PFC_PWM_MODULO / 4)));

  PWMBase->SM[0].VAL3 = PWM_VAL3_VAL3((uint16_t)((M2_PWM_MODULO / 4) - 1));
  PWMBase->SM[1].VAL3 = PWM_VAL3_VAL3((uint16_t)((M2_PWM_MODULO / 4) - 1));
  PWMBase->SM[2].VAL3 = PWM_VAL3_VAL3((uint16_t)((M2_PWM_MODULO / 4) - 1));
  PWMBase->SM[3].VAL3 = PWM_VAL3_VAL3((uint16_t)((PFC_PWM_MODULO / 4) - 1));
  
  //for PFC sampling
  /* PWM1 module 0 trigger on VAL4 for PFC ADC sample */
  PWMBase->SM[0].VAL4 = PWM_VAL4_VAL4((uint16_t)(-(M2_PWM_MODULO *3 / 8)));
  PWMBase->SM[0].TCTRL |= PWM_TCTRL_OUT_TRIG_EN(1 << 4);
  
  //for PFC sampling
  /* PWM0 module 0 trigger on VAL5 for PFC ADC sample */
  PWMBase->SM[0].VAL5 = PWM_VAL5_VAL5((uint16_t)(M2_PWM_MODULO / 8));
  PWMBase->SM[0].TCTRL |= PWM_TCTRL_OUT_TRIG_EN(1 << 5);
  
  //for PFC control
  /* PWM0 module 1 trigger on VAL4 for PFC control */
  PWMBase->SM[1].VAL4 = PWM_VAL4_VAL4((uint16_t)(180*3-(M2_PWM_MODULO *3 / 8)));

  /* Enable compare interrupt for PWM1_SM1_VAL4 for PFC control */
  PWMBase->SM[1].INTEN |= PWM_INTEN_CMPIE(1 << 4);
  
  //for PFC control
  /* PWM0 module 1 trigger on VAL5 for PFC control */
  PWMBase->SM[1].VAL5 = PWM_VAL5_VAL5((uint16_t)(180*3+(M2_PWM_MODULO / 8)));
  /* Enable compare interrupt for PWM1_SM0_VAL5 for PFC control */
  PWMBase->SM[1].INTEN |= PWM_INTEN_CMPIE(1 << 5);
  
  /* set deadtime (number of Fast Peripheral Clocks)
     DTCNT0,1 = T_dead * f_fpc = 1.5us * 180MHz = 270 */
  PWMBase->SM[0].DTCNT0 = ((M2_PWM_DEADTIME * (MCU_CLOCK_FREQ / 1000000U)) / 1000U);
  PWMBase->SM[1].DTCNT0 = ((M2_PWM_DEADTIME * (MCU_CLOCK_FREQ / 1000000U)) / 1000U);
  PWMBase->SM[2].DTCNT0 = ((M2_PWM_DEADTIME * (MCU_CLOCK_FREQ / 1000000U)) / 1000U);
  PWMBase->SM[3].DTCNT0 = 0;
  PWMBase->SM[0].DTCNT1 = ((M2_PWM_DEADTIME * (MCU_CLOCK_FREQ / 1000000U)) / 1000U);
  PWMBase->SM[1].DTCNT1 = ((M2_PWM_DEADTIME * (MCU_CLOCK_FREQ / 1000000U)) / 1000U);
  PWMBase->SM[2].DTCNT1 = ((M2_PWM_DEADTIME * (MCU_CLOCK_FREQ / 1000000U)) / 1000U);
  PWMBase->SM[3].DTCNT1 = 0;

   /* Full cycle reload */
  PWMBase->SM[0].CTRL |= PWM_CTRL_FULL_MASK;
  PWMBase->SM[1].CTRL |= PWM_CTRL_FULL_MASK;
  PWMBase->SM[2].CTRL |= PWM_CTRL_FULL_MASK;
  PWMBase->SM[3].CTRL |= PWM_CTRL_FULL_MASK;  
  
  //ext_force init for fan
  PWMBase->SM[0].CTRL2 = (PWMBase->SM[0].CTRL2 & ~PWM_CTRL2_INIT_SEL_MASK)
    | (PWM_CTRL2_INIT_SEL(0x0)|PWM_CTRL2_FORCE_SEL(6U)|PWM_CTRL2_FRCEN_MASK); 
  PWMBase->SM[1].CTRL2 = (PWMBase->SM[1].CTRL2 & ~PWM_CTRL2_INIT_SEL_MASK)
    | (PWM_CTRL2_INIT_SEL(0x0)|PWM_CTRL2_FORCE_SEL(6U)|PWM_CTRL2_FRCEN_MASK); 
  PWMBase->SM[2].CTRL2 = (PWMBase->SM[2].CTRL2 & ~PWM_CTRL2_INIT_SEL_MASK)
    | (PWM_CTRL2_INIT_SEL(0x0)|PWM_CTRL2_FORCE_SEL(6U)|PWM_CTRL2_FRCEN_MASK); 
  //ext_force to force for PFC
  PWMBase->SM[3].CTRL2 = (PWMBase->SM[3].CTRL2 & ~PWM_CTRL2_INIT_SEL_MASK) 
    | (PWM_CTRL2_INIT_SEL(0x0)|PWM_CTRL2_FORCE_SEL(6U)|PWM_CTRL2_FRCEN_MASK|PWM_CTRL2_INDEP_MASK); 

  /* Fault trigger settings */
  PWMBase->SM[0].DISMAP[0] = 0xF111U;//Fault 0 will disable the PWM1SM0 PWM output
  PWMBase->SM[1].DISMAP[0] = 0xF111U;//Fault 0 will disable the PWM1SM1 PWM output
  PWMBase->SM[2].DISMAP[0] = 0xF111U;//Fault 0 will disable the PWM1SM2 PWM output
  PWMBase->SM[3].DISMAP[0] = 0xF222U;//Fault 1 will disable the PWM1SM3 PWM output

  /* PWMs are re-enabled at PWM full cycle */
  PWMBase->FSTS = (PWMBase->FSTS & ~PWM_FSTS_FFULL_MASK) | PWM_FSTS_FFULL(0x3);

  /* PWM fault filter - 3 Fast periph. clocks sample rate, 5 agreeing
     samples to activate */
  PWMBase->FFILT = (PWMBase->FFILT & ~PWM_FFILT_FILT_PER_MASK) | PWM_FFILT_FILT_PER(2);

  /* All interrupts disabled, safe manual fault clearing, inversed logic (trigger level = high) */
  PWMBase->FCTRL &= ~(PWM_FCTRL_FLVL_MASK | PWM_FCTRL_FAUTO_MASK | PWM_FCTRL_FSAFE_MASK | PWM_FCTRL_FIE_MASK); /* clear FCTRL register prior further settings */
  PWMBase->FCTRL |= PWM_FCTRL_FIE(0U); /* FAULT 0 & FAULT 1 - Interrupt disable */
  PWMBase->FCTRL |= PWM_FCTRL_FLVL((0<<0)|(1<<1));/* Active low for FAULT 0 & Active high for FAULT 1 */
  PWMBase->FCTRL |= PWM_FCTRL_FAUTO(0x3U);
  PWMBase->FCTRL |= PWM_FCTRL_FSAFE(0xFU);

  /* Clear all fault flags */
  PWMBase->FSTS = (PWMBase->FSTS & ~PWM_FSTS_FFLAG_MASK) | PWM_FSTS_FFLAG(0xF);
  
  /* Start PWMs (set load OK flags and run - we need to trigger the ADC) */
  PWMBase->MCTRL = (PWMBase->MCTRL & ~PWM_MCTRL_CLDOK_MASK) | PWM_MCTRL_CLDOK(0xF);
  PWMBase->MCTRL = (PWMBase->MCTRL & ~PWM_MCTRL_LDOK_MASK) | PWM_MCTRL_LDOK(0xF);
//  PWMBase->MCTRL = (PWMBase->MCTRL & ~PWM_MCTRL_RUN_MASK) | PWM_MCTRL_RUN(0xF);

  /* eFlexPWM base address */
  g_sM2Pwm3ph.pui32PwmBaseAddress = (PWM_Type *)PWMBase;
  
#if 0
  PWMBase->OUTEN |= PWM_OUTEN_PWMA_EN(0x8);
  PWMBase->OUTEN |= PWM_OUTEN_PWMB_EN(0x8);
#else
    PWMBase->OUTEN &= (~PWM_OUTEN_PWMA_EN(0xF));
    PWMBase->OUTEN &= (~PWM_OUTEN_PWMB_EN(0xF));
#endif
  
  NVIC_SetPriority(FLEXPWM1_SUBMODULE1_IRQn, 2U);//for PFC control
}

/*!
 * @brief   void InitADC1(void)
 *           - Initialization of the ADC1 peripheral
 *           - Initialization of the A/D converter for current and voltage sensing
 *
 * @param   void
 *
 * @return  none
 */
static void InitADC1(void)
{
  lpadc_config_t lpadcConfig;

  /* Init the lpadcConfig */
  LPADC_GetDefaultConfig(&lpadcConfig);
  lpadcConfig.enableAnalogPreliminary = true;
  lpadcConfig.powerLevelMode = kLPADC_PowerLevelAlt4;
  lpadcConfig.referenceVoltageSource = kLPADC_ReferenceVoltageAlt3;
  lpadcConfig.conversionAverageMode = kLPADC_ConversionAverage128;

  /* Init ADC */
  lpadc_conv_trigger_config_t lpadcTriggerConfig;
  lpadc_conv_command_config_t lpadcCommandConfig;

  LPADC_Init(ADC1, &lpadcConfig);

  LPADC_DoOffsetCalibration(ADC1);
  LPADC_DoAutoCalibration(ADC1);

  /* Init commands */
  /* Set conversion CMD1 configuration. *///IW sampling for fan
  LPADC_GetDefaultConvCommandConfig(&lpadcCommandConfig);
  lpadcCommandConfig.channelNumber = 2U;                                      /* Set ADC channel ADC1IN2 (FAN_IW) */
  lpadcCommandConfig.sampleChannelMode = kLPADC_SampleChannelSingleEndSideA;
  lpadcCommandConfig.conversionResolutionMode = kLPADC_ConversionResolutionHigh;
  lpadcCommandConfig.chainedNextCommandNumber = 2U;                           /* Next execuited CMD will be CMD2 */
  LPADC_SetConvCommandConfig( ADC1, 1U, &lpadcCommandConfig );                /* Configure the CMD1 */

  /* Set conversion CMD2 configuration. *///IV sampling for fan
  LPADC_GetDefaultConvCommandConfig(&lpadcCommandConfig);
  lpadcCommandConfig.channelNumber = 3U;                                      /* Set ADC channel ADC1IN3 (FAN_IV) */
  lpadcCommandConfig.sampleChannelMode = kLPADC_SampleChannelSingleEndSideA;
  lpadcCommandConfig.conversionResolutionMode = kLPADC_ConversionResolutionHigh;
  lpadcCommandConfig.chainedNextCommandNumber = 0U;                           /* Next execuited CMD will be CMD3 */
  LPADC_SetConvCommandConfig( ADC1, 2U, &lpadcCommandConfig );                /* Configure the CMD 2 */

  /* Init triggers (use trigger 0). */
  LPADC_GetDefaultConvTriggerConfig(&lpadcTriggerConfig);
  lpadcTriggerConfig.targetCommandId = 1U;
  lpadcTriggerConfig.enableHardwareTrigger = true;
  LPADC_SetConvTriggerConfig(ADC1, 0U, &lpadcTriggerConfig);
  
  //PFC sampling
  /* Set conversion CMD3 configuration. *///UDCB sampling
  LPADC_GetDefaultConvCommandConfig(&lpadcCommandConfig);
  lpadcCommandConfig.channelNumber = 8U;                                      /* Set ADC channel ADC1IN8 (UDCB) */
  lpadcCommandConfig.sampleChannelMode = kLPADC_SampleChannelSingleEndSideA;
  lpadcCommandConfig.conversionResolutionMode = kLPADC_ConversionResolutionHigh;
  lpadcCommandConfig.chainedNextCommandNumber = 4U;                           
  LPADC_SetConvCommandConfig( ADC1, 3U, &lpadcCommandConfig );                /* Configure the CMD3 */

  /* Set conversion CMD4 configuration. *///PFC_I sampling
  LPADC_GetDefaultConvCommandConfig(&lpadcCommandConfig);
  lpadcCommandConfig.channelNumber = 9U;                                      /* Set ADC channel ADC1IN9 (PFC_I) */
  lpadcCommandConfig.sampleChannelMode = kLPADC_SampleChannelSingleEndSideA;
  lpadcCommandConfig.conversionResolutionMode = kLPADC_ConversionResolutionHigh;
  lpadcCommandConfig.chainedNextCommandNumber = 5U;                           
  LPADC_SetConvCommandConfig( ADC1, 4U, &lpadcCommandConfig );                /* Configure the CMD4 */

  /* Set conversion CMD5 configuration. *///VAC sampling
  LPADC_GetDefaultConvCommandConfig(&lpadcCommandConfig);
  lpadcCommandConfig.channelNumber = 10U;                                      /* Set ADC channel ADC1IN10 (VAC) */
  lpadcCommandConfig.sampleChannelMode = kLPADC_SampleChannelSingleEndSideA;
  lpadcCommandConfig.conversionResolutionMode = kLPADC_ConversionResolutionHigh;
  lpadcCommandConfig.chainedNextCommandNumber = 0U;                           
  LPADC_SetConvCommandConfig( ADC1, 5U, &lpadcCommandConfig );                /* Configure the CMD5 */
  
  /* Init triggers (use trigger 1). */
  LPADC_GetDefaultConvTriggerConfig(&lpadcTriggerConfig);
  lpadcTriggerConfig.targetCommandId = 3U;
  lpadcTriggerConfig.enableHardwareTrigger = true;
  LPADC_SetConvTriggerConfig(ADC1, 1U, &lpadcTriggerConfig);/* Configurate the trigger1. */

  /* Enable TCOMP interrupt. */
  LPADC_EnableInterrupts(ADC1, ADC_IE_TCOMP_IE(0x1U));/* For fan sampling and control */
  NVIC_SetPriority(ADC1_IRQn, 3U);
  NVIC_EnableIRQ(ADC1_IRQn);
}

/*!
 * @brief   void InitSlowLoop(void)
 *           - Initialization of the CTIMER0 peripheral
 *           - performs slow control loop counter
 *
 * @param   void
 *
 * @return  none
 */
static void InitSlowLoop(void)
{
    ctimer_config_t cTimerConfig;
    ctimer_match_config_t cTimerMatchConfig;
    uint32_t ui32CTimerFreq;

    /* Get defaut configuration */
    CTIMER_GetDefaultConfig(&cTimerConfig);
    /* Init timer */
    CTIMER_Init(CTIMER0, &cTimerConfig);

    /* Get CTimer0 frequency for correct set Match register value */
    ui32CTimerFreq = CLOCK_GetFreq(kCLOCK_Fro12M);

    cTimerMatchConfig.enableCounterReset = true;
    cTimerMatchConfig.enableCounterStop  = false;
    cTimerMatchConfig.matchValue         = (uint32_t)(ui32CTimerFreq / PFC_SLOW_LOOP_FREQ / 1000);
    cTimerMatchConfig.outControl         = kCTIMER_Output_NoAction;
    cTimerMatchConfig.outPinInitState    = false;
    cTimerMatchConfig.enableInterrupt    = true;

    CTIMER_SetupMatch(CTIMER0, kCTIMER_Match_0, &cTimerMatchConfig);
    NVIC_SetPriority(CTIMER0_IRQn, 5U);
    NVIC_EnableIRQ(CTIMER0_IRQn);
    CTIMER_StartTimer(CTIMER0);
}

/*!
* @brief   void InitClock(void)
*          - Core, bus, flash clock setup
*
* @param   void
*
* @return  none
*/
static void InitClock(void)
{
    uint32_t ui32CyclesNumber = 0U;

    /* Calculate clock dependant variables for PMSM control algorithm */
    g_sClockSetup.ui32FastPeripheralClock = CLOCK_GetFreq(kCLOCK_CoreSysClk);
    g_sClockSetup.ui32CpuFrequency = CLOCK_GetFreq(kCLOCK_CoreSysClk);

    /* Parameters for motor 1 */
    g_sClockSetup.ui16M1PwmFreq   = M1_PWM_FREQ; /* 6.25 kHz */
    g_sClockSetup.ui16M1PwmModulo = (g_sClockSetup.ui32FastPeripheralClock) / g_sClockSetup.ui16M1PwmFreq;
    ui32CyclesNumber = ((M1_PWM_DEADTIME * (g_sClockSetup.ui32FastPeripheralClock / 1000000U)) / 1000U);
    g_sClockSetup.ui16M1PwmDeadTime   = ui32CyclesNumber;
    g_sClockSetup.ui16M1SpeedLoopFreq = M1_SLOW_LOOP_FREQ; /* 1kHz */

    /* Parameters for motor 2 */
    g_sClockSetup.ui16M2PwmFreq   = M2_PWM_FREQ; /* 16 kHz */
    g_sClockSetup.ui16M2PwmModulo = (g_sClockSetup.ui32FastPeripheralClock) / g_sClockSetup.ui16M2PwmFreq;
    ui32CyclesNumber = ((M2_PWM_DEADTIME * (g_sClockSetup.ui32FastPeripheralClock / 1000000U)) / 1000U);
    g_sClockSetup.ui16M2PwmDeadTime   = ui32CyclesNumber;
    g_sClockSetup.ui16M2SpeedLoopFreq = M2_SLOW_LOOP_FREQ; /* 1kHz */
}

static void InitAOI0(void)
{
    aoi_event_config_t aoiEventLogicStruct;
    
    /* Configure the AOI event */
    aoiEventLogicStruct.PT0AC = kAOI_InputSignal;
    aoiEventLogicStruct.PT0BC = kAOI_LogicOne;
    aoiEventLogicStruct.PT0CC = kAOI_LogicOne;
    aoiEventLogicStruct.PT0DC = kAOI_LogicOne;

    aoiEventLogicStruct.PT1AC = kAOI_LogicOne;
    aoiEventLogicStruct.PT1BC = kAOI_InputSignal;
    aoiEventLogicStruct.PT1CC = kAOI_LogicOne;
    aoiEventLogicStruct.PT1DC = kAOI_LogicOne;

    aoiEventLogicStruct.PT2AC = kAOI_LogicOne;
    aoiEventLogicStruct.PT2BC = kAOI_LogicOne;
    aoiEventLogicStruct.PT2CC = kAOI_InputSignal;
    aoiEventLogicStruct.PT2DC = kAOI_LogicOne;

    aoiEventLogicStruct.PT3AC = kAOI_LogicZero;
    aoiEventLogicStruct.PT3BC = kAOI_LogicZero;
    aoiEventLogicStruct.PT3CC = kAOI_LogicZero;
    aoiEventLogicStruct.PT3DC = kAOI_LogicZero;

    /* Init AOI module. */
    AOI_Init(AOI0);
    AOI_SetEventLogicConfig(AOI0, kAOI_Event0, &aoiEventLogicStruct);
}

static void InitAOI1(void)
{
    aoi_event_config_t aoiEventLogicStruct;
    
    /* Configure the AOI event */
    aoiEventLogicStruct.PT0AC = kAOI_InputSignal;
    aoiEventLogicStruct.PT0BC = kAOI_LogicOne;
    aoiEventLogicStruct.PT0CC = kAOI_LogicOne;
    aoiEventLogicStruct.PT0DC = kAOI_LogicOne;

    aoiEventLogicStruct.PT1AC = kAOI_LogicOne;
    aoiEventLogicStruct.PT1BC = kAOI_InputSignal;
    aoiEventLogicStruct.PT1CC = kAOI_LogicOne;
    aoiEventLogicStruct.PT1DC = kAOI_LogicOne;

    aoiEventLogicStruct.PT2AC = kAOI_LogicZero;
    aoiEventLogicStruct.PT2BC = kAOI_LogicZero;
    aoiEventLogicStruct.PT2CC = kAOI_LogicZero;
    aoiEventLogicStruct.PT2DC = kAOI_LogicZero;

    aoiEventLogicStruct.PT3AC = kAOI_LogicZero;
    aoiEventLogicStruct.PT3BC = kAOI_LogicZero;
    aoiEventLogicStruct.PT3CC = kAOI_LogicZero;
    aoiEventLogicStruct.PT3DC = kAOI_LogicZero;

    /* Init AOI module. */
    AOI_Init(AOI1);
    AOI_SetEventLogicConfig(AOI1, kAOI_Event0, &aoiEventLogicStruct);
    INPUTMUX_AttachSignal(INPUTMUX0, 1U, kINPUTMUX_Aoi1Out0ToExtTrigger);
}

static void InitUART1(void)
{
    lpuart_config_t config;
    /*
     * config.baudRate_Bps = 115200U;
     * config.parityMode = kLPUART_ParityDisabled;
     * config.stopBitCount = kLPUART_OneStopBit;
     * config.txFifoWatermark = 0;
     * config.rxFifoWatermark = 0;
     * config.enableTx = false;
     * config.enableRx = false;
     */
    LPUART_GetDefaultConfig(&config);
    config.baudRate_Bps = 19200U; //57600
    config.enableTx     = false;
    config.enableRx     = false;

    LPUART_Init((LPUART_Type *)LPUART1, &config, 12000000U);
    
    /* Register communication module used by FreeMASTER driver. */
    FMSTR_SerialSetBaseAddress((LPUART_Type *)LPUART1);
}

void InitInputmux(void)
{
    /* INPUTMUX0: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GateINPUTMUX0);
    /* INPUTMUX0 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kINPUTMUX0_RST_SHIFT_RSTn);
    
    //for compressor sampling
    INPUTMUX_AttachSignal(INPUTMUX0, 0U, kINPUTMUX_Pwm0Sm0OutTrig0ToAoi0Mux);
    INPUTMUX_AttachSignal(INPUTMUX0, 1U, kINPUTMUX_Pwm0Sm0OutTrig1ToAoi0Mux);
    INPUTMUX_AttachSignal(INPUTMUX0, 2U, kINPUTMUX_Pwm0Sm1OutTrig0ToAoi0Mux);
    
    /*  Aoi0Out0 connect to Adc0Trigger 0 */
    INPUTMUX_AttachSignal(INPUTMUX0, 0U, kINPUTMUX_Aoi0Out0ToAdc0Trigger);

    /*  for fan sampling */
    INPUTMUX_AttachSignal(INPUTMUX0, 0U, kINPUTMUX_Pwm0Sm3OutTrig0ToAdc1Trigger);
}

void InitInputmuxPFC(void)
{
   //for PFC sampling
    INPUTMUX_AttachSignal(INPUTMUX0, 0U, kINPUTMUX_Pwm1Sm0OutTrig0ToAoi1Mux);
    INPUTMUX_AttachSignal(INPUTMUX0, 1U, kINPUTMUX_Pwm1Sm0OutTrig1ToAoi1Mux);
    INPUTMUX_AttachSignal(INPUTMUX0, 1U, kINPUTMUX_Aoi1Out0ToAdc1Trigger);
   
    /* LPCMP0_OUT is selected as trigger input for PWM1 FAULT channel 1 */
    INPUTMUX_AttachSignal(INPUTMUX0, 1U, kINPUTMUX_Cmp0OutToFlexPwm1Fault);
}

/* Recorder0 configuration */
FMSTR_U8 FreeMASTER_RecBuffer0[FREEMASTER_REC_0_SIZE];
FMSTR_REC_BUFF FreeMASTER_Recorder_0 = {
  .name = "Description of recorder 0",
  .addr = (FMSTR_ADDR)FreeMASTER_RecBuffer0,
  .size = (FMSTR_SIZE) sizeof(FreeMASTER_RecBuffer0),
  .basePeriod_ns = 160000UL
};
/*******************************************************************************
 * Public functions
 ******************************************************************************/
/*!
 * @brief   void MCDRV_Init(void)
 *           - Motor control driver main initialization
 *           - Calls initialization functions of peripherals required for motor
 *             control functionality
 *
 * @param   void
 *
 * @return  none
 */
void MCDRV_Init(void)
{
    InitClock();                /* Init application clock dependent variables */
    InitSlowLoop();             /* Init slow loop counter */

    InitADC1();                 /* Init ADC1 */
    InitOpAmps();               /* Init OPAMPS */
    InitLpCmp0();               /* Init Low Power Comparator 0 */
    InitInputmuxPFC();          /* Init InputMux for PFC*/
    InitPWM0();                 /* 6-channel PWM0 peripheral init */
    InitADC0();                 /* Init ADC0 */
    InitPWM1();                 /* 8-channel PWM1 peripheral init */
    
    InitUART1();                /* Init UART1 for FreeMASTER */
    
    InitAOI0();                  /* Init AOI for compressor sampling */
    InitAOI1();                  /* Init AOI for PFC sampling */ 
}

void FMSTR_Recorder_Create()
{
    /* FreeMASTER recorder 0 configuration initialization  */
    FMSTR_RecorderCreate(0, &FreeMASTER_Recorder_0);
}
