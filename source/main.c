/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "hardware_init.h"
#include "mc_periph_init.h"
#include "freemaster.h"
#include "freemaster_serial_lpuart.h"
#include "m1_sm_snsless.h"
#include "m2_sm_snsless.h"
#include "PFC_statemachine.h"
#include "fsl_lpuart.h"
#include "uart_handling.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Version info */
#define MCRSP_VER "2.0.0" /* motor control package version */

/* Example's feature set in form of bits inside ui16featureSet.
   This feature set is expected to be growing over time.
   ... | FEATURE_S_RAMP | FEATURE_FIELD_WEAKENING | FEATURE_ENC
*/
#define FEATURE_ENC (1)               /* Encoder feature flag */
#define FEATURE_FIELD_WEAKENING (0)   /* Field weakening feature flag */
#define FEATURE_S_RAMP (0)            /* S-ramp feature flag */

#define FEATURE_SET (FEATURE_ENC << (0) | \
                     FEATURE_FIELD_WEAKENING << (1) | \
                     FEATURE_S_RAMP << (2))

/* CPU load measurement SysTick START / STOP macros */
#define SYSTICK_START_COUNT() (SysTick->VAL = SysTick->LOAD)
#define SYSTICK_STOP_COUNT(par1)   \
    uint32_t val  = SysTick->VAL;  \
    uint32_t load = SysTick->LOAD; \
    par1          = load - val

/* CPU load measurement using Systick */
uint32_t g_ui32NumberOfCycles    = 0U;
uint32_t g_ui32MaxNumberOfCycles = 0U;
//uint32_t * state_change_delay = 0;
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void BOARD_InitSysTick(void);
/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Structure used in FM to get required ID's */
app_ver_t g_sAppIdFM = {
    "MCXA346", /* board id */
    "HVAC_3in1", /* example id */
    MCRSP_VER,      /* sw version */
    FEATURE_SET,    /* example's feature-set */
};
//#define PFC
//#define Fan
//#define Compressor
//#define Slow

uint32_t        M1_isr_cnt=0;
uint32_t        M2_isr_cnt=0;
uint32_t        PFC_isr_cnt=0;
uint32_t        Slow_isr_cnt=0;
/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
void main(void)
{
    /*Accessing ID structure to prevent optimization*/
    g_sAppIdFM.ui16FeatureSet = FEATURE_SET;
    
    SYSCON->NVM_CTRL &= ~SYSCON_NVM_CTRL_DIS_MBECC_ERR_DATA_MASK;
    SYSCON->LPCAC_CTRL |= SYSCON_LPCAC_CTRL_LPCAC_MEM_REQ_MASK;
    SYSCON->LPCAC_CTRL &= ~SYSCON_LPCAC_CTRL_DIS_LPCAC_MASK;
    
    uint32_t ui32PrimaskReg;
    
    /* Disable all interrupts before peripherals are initialized */
    ui32PrimaskReg = DisableGlobalIRQ();

    /* Board pin init */
    BOARD_InitHardware();
    
    /* Initialize peripheral motor control and PFC driver */
    MCDRV_Init();
    initialize_uart0_transfers();
    /* FreeMASTER driver initialization */
    FMSTR_Init();
    FMSTR_Recorder_Create();
    
    /* SysTick initialization for CPU load measurement */
    BOARD_InitSysTick();
    
    //PWM counter must run just before IRQ enable
    FLEXPWM0->MCTRL = (FLEXPWM0->MCTRL & ~PWM_MCTRL_RUN_MASK) | PWM_MCTRL_RUN(0xF);
    FLEXPWM1->MCTRL = (FLEXPWM1->MCTRL & ~PWM_MCTRL_RUN_MASK) | PWM_MCTRL_RUN(0xF);
    
    /* Enable interrupts */
    EnableGlobalIRQ(ui32PrimaskReg);

    while (1)
    {


      /* FreeMASTER Polling function */
      FMSTR_Poll();
    }
}

//ADC0 interrupt for compressor sampling, priority 4
void ADC0_IRQHandler(void)
{
    /* Clear the TCOMP INT flag */
    ADC0->STAT |= (uint32_t)(1U << 9);
    
    M2_isr_cnt++;
    
#ifdef Compressor
    uint32_t ui32PrimaskReg;
    GPIO_PinWrite(GPIO3, 6, 1);
    /* Disable all interrupts before peripherals are initialized */
    ui32PrimaskReg = DisableGlobalIRQ();

    /* Start CPU tick number couting */
    SYSTICK_START_COUNT();
#endif
    /* Transfer the UDCB voltage for Compressor control */
    g_sM1Drive.sFocPMSM.f16UDcBus = gsPFC_Drive.sUInPeakDetection.f16Udcb;
    /* StateMachine call */
    SM_StateMachineFast(&g_sM1Ctrl);

#ifdef Compressor
    /* Stop CPU tick number couting and store actual and maximum ticks */
    SYSTICK_STOP_COUNT(g_ui32NumberOfCycles);
    g_ui32MaxNumberOfCycles =
        g_ui32NumberOfCycles > g_ui32MaxNumberOfCycles ? g_ui32NumberOfCycles : g_ui32MaxNumberOfCycles;
    GPIO_PinWrite(GPIO3, 6, 0);
    /* Enable interrupts  */
    EnableGlobalIRQ(ui32PrimaskReg);
#endif
    /* Call FreeMASTER recorder */
    FMSTR_Recorder(0);
}

//ADC1 interrupt for fan sampling, priority 3
void ADC1_IRQHandler(void)
{
    /* Clear the TCOMP INT flag */
    ADC1->STAT |= (uint32_t)(1U << 9);
    
    M1_isr_cnt++;
    
#ifdef Fan
    GPIO_PinWrite(GPIO3, 6, 1);
    uint32_t ui32PrimaskReg;
    /* Disable all interrupts before peripherals are initialized */
    ui32PrimaskReg = DisableGlobalIRQ();

    /* Start CPU tick number couting */
    SYSTICK_START_COUNT();
#endif
    /* Transfer the UDCB voltage for Fan control */
    g_sM2Drive.sFocPMSM.f16UDcBus = gsPFC_Drive.sUInPeakDetection.f16Udcb;
    /* StateMachine call */
    SM_StateMachineFast(&g_sM2Ctrl);
    
#ifdef Fan
    /* Stop CPU tick number couting and store actual and maximum ticks */
    SYSTICK_STOP_COUNT(g_ui32NumberOfCycles);
    g_ui32MaxNumberOfCycles =
        g_ui32NumberOfCycles > g_ui32MaxNumberOfCycles ? g_ui32NumberOfCycles : g_ui32MaxNumberOfCycles;

    GPIO_PinWrite(GPIO3, 6, 0);
    /* Enable interrupts  */
    EnableGlobalIRQ(ui32PrimaskReg);
#endif  
}

frac16_t f16SlowLoopCnt=1;
//CTIMER0 interrupt for Slow loop control, priority 5
void CTIMER0_IRQHandler(void)
{

    /* Clear the match interrupt flag. */
    CTIMER0->IR |= CTIMER_IR_MR0INT(1U);
    //state_change_delay++;


#ifdef Slow
    GPIO_PinWrite(GPIO3, 6, 1);
    uint32_t ui32PrimaskReg;
    /* Disable all interrupts before peripherals are initialized */
    ui32PrimaskReg = DisableGlobalIRQ();

    /* Start CPU tick number couting */
    SYSTICK_START_COUNT();
#endif
    if(f16SlowLoopCnt++>=PFC_SLOW_LOOP_FREQ*1000/M1_SLOW_LOOP_FREQ)
    {
      f16SlowLoopCnt=1;
      /* M1 Slow StateMachine call */
      SM_StateMachineSlow(&g_sM1Ctrl);
      /* M2 Slow StateMachine call */
      SM_StateMachineSlow(&g_sM2Ctrl);
    }


    gsPFC_Drive.sUCtrl.fltUDcBus = gsPFC_Drive.sUInPeakDetection.fltUdcb;
    gsPFC_Drive.sUCtrl.fltUDcBusFilt = GDFLIB_FilterIIR1_FLT(gsPFC_Drive.sUCtrl.fltUDcBus, &gsPFC_Drive.sUCtrl.sUDcBusFilter);

    PFC_UInPeak_detect(&gsPFC_Drive);    
    //PFC control
    SM_StateMachineSlow(&gsPFC_Ctrl);    
    uart0_sm();
#ifdef Slow
    /* Stop CPU tick number couting and store actual and maximum ticks */
    SYSTICK_STOP_COUNT(g_ui32NumberOfCycles);
    g_ui32MaxNumberOfCycles =
        g_ui32NumberOfCycles > g_ui32MaxNumberOfCycles ? g_ui32NumberOfCycles : g_ui32MaxNumberOfCycles;

    GPIO_PinWrite(GPIO3, 6, 0);
    /* Enable interrupts  */
    EnableGlobalIRQ(ui32PrimaskReg);
#endif 
}

void FLEXPWM0_SUBMODULE1_IRQHandler()
{
//  GPIO_PinWrite(GPIO3, 6, 1);
  InitInputmux();
  NVIC_EnableIRQ(FLEXPWM1_SUBMODULE0_IRQn);
  NVIC_EnableIRQ(FLEXPWM1_SUBMODULE1_IRQn);
  if(FLEXPWM0->SM[1].STS & PWM_STS_CMPF(1<<5))
  {
    FLEXPWM0->SM[1].STS |= PWM_STS_CMPF(1<<5);
  }
  NVIC_DisableIRQ(FLEXPWM0_SUBMODULE1_IRQn);
//  GPIO_PinWrite(GPIO3, 6, 0);
}

#define TEST_PFC_OUTPUT 0 //Can not enable in high voltage!!!!!!!  

#if TEST_PFC_OUTPUT
float_t test_duty = 0.5f;
#endif

//FLEXPWM0_COMPARE1 interrupt for PFC sampling, priority 2
void FLEXPWM1_SUBMODULE1_IRQHandler()
{
//  GPIO_PinWrite(GPIO3, 6, 1);

    if(FLEXPWM1->SM[1].STS & PWM_STS_CMPF(1<<4))
    {
      FLEXPWM1->SM[1].STS |= PWM_STS_CMPF(1<<4);
    }
    else if(FLEXPWM1->SM[1].STS & PWM_STS_CMPF(1<<5))
    {
      FLEXPWM1->SM[1].STS |= PWM_STS_CMPF(1<<5);
    }
    PFC_isr_cnt++;  
   
#ifdef PFC
    uint32_t ui32PrimaskReg;
    /* Disable all interrupts before peripherals are initialized */
    ui32PrimaskReg = DisableGlobalIRQ();
    /* Start CPU tick number couting */
    SYSTICK_START_COUNT();
#endif
    //read adc sample for PFC control
    PFC_MCDRV_ADC_GET(&g_sPFCAdcSensor);
    
 //   gsPFC_Drive.sUInPeakDetection.f16UIn = 
    
 //   gsPFC_Drive.sUInPeakDetection.fltUIn = MLIB_ConvSc_FLTsf(gsPFC_Drive.sUInPeakDetection.f16UIn, g_fltPFCVoltageScale);
    gsPFC_Drive.sICtrlPh1.fltIFdbck = MLIB_ConvSc_FLTsf((gsPFC_Drive.sUInPeakDetection.f16Imos- PFC_I_HARDWARE_OFFSET), g_fltPFCCurrentScale);
    
    gsPFC_Drive.sUInPeakDetection.fltUInFiltRaw = GDFLIB_FilterIIR1_FLT(gsPFC_Drive.sUInPeakDetection.fltUIn, &gsPFC_Drive.sUInPeakDetection.sFilter);

    gsPFC_Drive.sUInPeakDetection.fltUInFilt = MLIB_Abs_FLT(gsPFC_Drive.sUInPeakDetection.fltUIn);
    
#if PLL
    PFC_Phase_detect(&gsPFC_Drive);
#endif
      
#if TEST_PFC_OUTPUT   
    FLEXPWM1->OUTEN |= PWM_OUTEN_PWMA_EN(0x8);
    FLEXPWM1->OUTEN |= PWM_OUTEN_PWMB_EN(0x8);
    PFC_PWM_UPDATE(test_duty);
#else
    SM_StateMachineFast(&gsPFC_Ctrl);
#endif
#ifdef PFC
    /* Stop CPU tick number couting and store actual and maximum ticks */
    SYSTICK_STOP_COUNT(g_ui32NumberOfCycles);
    g_ui32MaxNumberOfCycles =
        g_ui32NumberOfCycles > g_ui32MaxNumberOfCycles ? g_ui32NumberOfCycles : g_ui32MaxNumberOfCycles;
    /* Enable interrupts  */
    EnableGlobalIRQ(ui32PrimaskReg);
#endif

//  GPIO_PinWrite(GPIO3, 6, 0);
}

static void BOARD_InitSysTick(void)
{
    /* Initialize SysTick core timer to run free */
    /* Set period to maximum value 2^24*/
    SysTick->LOAD = 0xFFFFFF;

    /*Clock source - System Clock*/
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;

    /*Start Sys Timer*/
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}
