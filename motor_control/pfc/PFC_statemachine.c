
/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdint.h>
#include "fsl_device_registers.h"
#include "PFC_statemachine.h"
#include "Table.h"
#include "common_func.h"
#include "fsl_inputmux.h"
#include "mc_periph_init.h"
/******************************************************************************
* Global variables
******************************************************************************/
/* PFC object including both hardware and software content */
PFCDEF_DRIVE_T     	 gsPFC_Drive;
PFCDRV_PWMVAL            gsPFC_PwmVal;
uint16_t  Fault_Watch=0;
bool pfc_flag = false;
/****************t*************************************************************
* Static variables
******************************************************************************/
bool_t bPFC_RUN; /* PFC run/stop command */
static  bool_t bEnableCheckDCBusVunder;  /* enabling flag for checking Output voltage under*/
static  bool_t bEnableCheckInputVunder;  /* enabling flag for checking Input voltage under*/
bool_t  bPFCVarInit = 0; /* PFC variables init status flag */
volatile float g_fltPFCCurrentScale, g_fltPFCVoltageScale;
/******************************************************************************
* Local functions
******************************************************************************/

/*------------------------------------
 * User state machine functions
 * ----------------------------------*/
static void PFC_StateFaultFast(void);
static void PFC_StateInitFast(void);
static void PFC_StateStopFast(void);
static void PFC_StateRunFast(void);

static void PFC_StateFaultSlow(void);
static void PFC_StateInitSlow(void);
static void PFC_StateStopSlow(void);
static void PFC_StateRunSlow(void);
/*------------------------------------
 * User state-transition functions
 * ----------------------------------*/
static void PFC_TransFaultInit(void);
static void PFC_TransInitFault(void);
static void PFC_TransInitStop(void);
static void PFC_TransStopFault(void);
static void PFC_TransStopRun(void);
static void PFC_TransRunFault(void);
static void PFC_TransRunStop(void);

/* State machine functions field (in pmem) */
static const sm_app_state_fcn_t s_PFC_STATE_FAST = {PFC_StateFaultFast, PFC_StateInitFast, PFC_StateStopFast, PFC_StateRunFast};

/* State machine functions field (in pmem) */
static const sm_app_state_fcn_t s_PFC_STATE_SLOW = {PFC_StateFaultSlow, PFC_StateInitSlow, PFC_StateStopSlow, PFC_StateRunSlow};

/* State-transition functions field */
static const sm_app_trans_fcn_t msTRANS = {PFC_TransFaultInit, PFC_TransInitFault, PFC_TransInitStop, PFC_TransStopFault, PFC_TransStopRun, PFC_TransRunFault, PFC_TransRunStop};

/* State machine structure declaration and initialization */
sm_app_ctrl_t gsPFC_Ctrl = 
{
	/* gsPFC_Ctrl.psState, User state functions  */
    &s_PFC_STATE_FAST,
    
    /* gsPFC_Ctrl.psState, User state functions  */
    &s_PFC_STATE_SLOW,
    
 	/* gsPFC_Ctrl.psTrans, User state-transition functions */
 	&msTRANS,
 
  	/* gsPFC_Ctrl.uiCtrl, Default no control command */
  	SM_CTRL_NONE,
  	
  	/* gsPFC_Ctrl.eState, Default state after reset */
  	kSM_AppInit 	
};

/*------------------------------------
 * User sub-state machine functions
 * ----------------------------------*/
static void PFC_StateRunSoftstart(void);
static void PFC_StateRunNormal(void);
static void PFC_StateRunLightload(void);

/*------------------------------------
 * User sub-state-transition functions
 * ----------------------------------*/

/* Sub-state machine functions field  */
const pfc_app_state_fcn mPFC_STATE_RUN_TABLE[3] = 
{
		PFC_StateRunSoftstart,
		PFC_StateRunNormal,
		PFC_StateRunLightload
};

sm_app_sub_ctrl_t gsPFC_SubCtrl = 
{
    &mPFC_STATE_RUN_TABLE[0],
    SOFTSTART
};

void SM_StateSubMachineRun(sm_app_sub_ctrl_t *sAppCtrl)
{
    sAppCtrl->pSubState[sAppCtrl->eStateRunSub]();
}

void PFC_FaultDetection(void);

/***************************************************************************//*!
*
* @brief   FAULT detection function
*
* @param   void
*
* @return  none
*
******************************************************************************/
void PFC_FaultDetection(void)
{
	/* Clearing actual faults before detecting them again  */         
	PFC_FAULT_CLEAR_ALL(gsPFC_Drive.FaultIdPending.R);

	/* DC bus voltage detection */
	if(gsPFC_Drive.sUCtrl.fltUDcBusFilt > gsPFC_Drive.sFaultThresholds.fltUDcBusOver)
	{ 
		gsPFC_Drive.FaultIdPending.B.UDcBusOver = 1;
	}
	else if((gsPFC_Drive.sUCtrl.fltUDcBusFilt < gsPFC_Drive.sFaultThresholds.fltUDcBusUnder)&&(bEnableCheckDCBusVunder))
	{ 
		gsPFC_Drive.FaultIdPending.B.UDcBusUnder = 1;
	}
	
	if(gsPFC_Drive.sUInPeakDetection.bSeveralPeaksDetectedFlag)
	{
		/* AC input voltage detection */
	    if(gsPFC_Drive.sUInPeakDetection.fltUInMax > gsPFC_Drive.sFaultThresholds.fltUInOver)
	    { 
	    	gsPFC_Drive.FaultIdPending.B.UInOver = 1;
	    }
	    else if((gsPFC_Drive.sUInPeakDetection.fltUInMax < gsPFC_Drive.sFaultThresholds.fltUInUnder)&&(bEnableCheckInputVunder))
	    { 
	    	gsPFC_Drive.FaultIdPending.B.UInUnder = 1;
	    }
	    
#if PLL
	    /* AC input frequency detection */
	    if(gsPFC_Ctrl.eState == kSM_AppRun)
	    {
			if(gsPFC_Drive.sPhaseDetector.f16Freq > gsPFC_Drive.sFaultThresholds.f16FreqUInOver)
			{ 
//				gsPFC_Drive.FaultIdPending.B.UInFreqOver = 1;
			}
			else if(gsPFC_Drive.sPhaseDetector.f16Freq < gsPFC_Drive.sFaultThresholds.f16FreqUInUnder)
			{ 
//				gsPFC_Drive.FaultIdPending.B.UInFreqUnder = 1;
			}
	    }
#endif
	}
	/* Phase current detection */
	if(gsPFC_Drive.sICtrlPh1.fltIFdbck > gsPFC_Drive.sFaultThresholds.fltIInOver)
	{
		gsPFC_Drive.FaultIdPending.B.IPh1Over = 1;
	}
	
	/* HW Over current detection */
	if(PFC_OVERCURRENT_FAULT())
	{
		gsPFC_Drive.FaultIdPending.B.HW_IOver = 1;
	}
	
	/* HW DC bus over voltage detection */
	if(PFC_OVERVOLTAGE_FAULT())
	{
		gsPFC_Drive.FaultIdPending.B.HW_UDcBusOver = 1;
	}

	/* pass fault to FaultId for recording */
	gsPFC_Drive.FaultId.R |= gsPFC_Drive.FaultIdPending.R;		
}

/***************************************************************************//*!
*
* @brief   FAULT state
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_StateFaultFast()
{
	PFC_DISABLE_PWM_OUTPUT();
		
	if(gsPFC_Drive.sUCtrl.fltUDcBusFilt < gsPFC_Drive.sUCtrl.fltUDcBusRelayOff)
	{
		RELAY_OFF();
		gsPFC_Drive.sFlag.RelayFlag = 0;
	}
	/* detect faults */
	PFC_FaultDetection();
}

static void PFC_StateFaultSlow()
{
    if(!gsPFC_Drive.FaultIdPending.R) // No fault detected for a certain time interval, recovery
	{
        if(--gsPFC_Drive.ui16CounterState == 0)
        {
            gsPFC_Ctrl.uiCtrl |= SM_CTRL_FAULT_CLEAR;  // clear fault command
        }
	}
	else 
	{
		gsPFC_Drive.ui16CounterState = FAULT_RELEASE_DURATION;
	}
}
/***************************************************************************//*!
*
* @brief   Init state
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_StateInitFast()
{
	if(!bPFCVarInit)
	{
	bPFC_RUN = 0;
	PFC_FAULT_CLEAR_ALL(gsPFC_Drive.FaultId.R);
	PFC_FAULT_CLEAR_ALL(gsPFC_Drive.FaultIdPending.R);
	PFC_CLEAR_OVERCURRENT_FAULT();
	PFC_CLEAR_OVERVOLTAGE_FAULT();
	
	/* Fault thresholds */
	gsPFC_Drive.sFaultThresholds.fltIInOver = PFC_IOVER_LIMIT;
	gsPFC_Drive.sFaultThresholds.fltUDcBusOver = PFC_OVERVOLT_LIMIT; 
        gsPFC_Drive.sFaultThresholds.fltUDcBusUnder = PFC_UNDERVOLT_LIMIT;
	gsPFC_Drive.sFaultThresholds.fltUInOver = PFC_VIN_OVERVOLT_LIMIT;
	gsPFC_Drive.sFaultThresholds.fltUInUnder = PFC_VIN_UNDERVOLT_LIMIT;
	gsPFC_Drive.sFaultThresholds.f16FreqUInOver =  FRAC16(PFC_VIN_FREQOVER_LIMIT*2/PFC_FASTLOOP_FREQ);
	gsPFC_Drive.sFaultThresholds.f16FreqUInUnder = FRAC16(PFC_VIN_FREQUNDER_LIMIT*2/PFC_FASTLOOP_FREQ);	
	
	gsPFC_Drive.sFlag.DCMFlag = 0;
	gsPFC_Drive.sFlag.PWM_enable = 0;
	gsPFC_Drive.sFlag.RelayFlag = 0;  /* flag indicate relay is open */
	gsPFC_Drive.sFlag.BrakeFlag = 0;
	/*------------------------- DC Bus voltage parameters --------------------------*/
	g_fltPFCVoltageScale = PFC_V_DCB_SCALE;
	g_fltPFCCurrentScale = PFC_I_SCALE;
	gsPFC_Drive.sUCtrl.fltUDcBusCmd = PFC_U_DCB_REF;
	gsPFC_Drive.sUCtrl.fltUDcBusFilt = 0;
	gsPFC_Drive.sUCtrl.fltUDcBusRelayOff  = PFC_U_DCB_RELAY_OFF;
	gsPFC_Drive.sUCtrl.fltUDcBusBurstOff = PFC_U_DCB_BURSTOFF;
	gsPFC_Drive.sUCtrl.fltUDcBusBurstOn = PFC_U_DCB_BURSTON;
	
	/* softstart ramp parameters */
	gsPFC_Drive.sUCtrl.sUDcBusRampParams.fltRampUp = PFC_U_SOFTSTART_STEP;

	/* DC Bus control parameters */
	gsPFC_Drive.sUCtrl.sUDcBusFilter.sFltCoeff.fltB0 = PFC_U_DCB_IIR_B0;
	gsPFC_Drive.sUCtrl.sUDcBusFilter.sFltCoeff.fltB1 = PFC_U_DCB_IIR_B1;
	gsPFC_Drive.sUCtrl.sUDcBusFilter.sFltCoeff.fltA1 = PFC_U_DCB_IIR_A1;
	GDFLIB_FilterIIR1Init_FLT(&gsPFC_Drive.sUCtrl.sUDcBusFilter);
	
	gsPFC_Drive.sUCtrl.sUDcBusPiParams.fltPGain = PFC_U_P_GAIN;
	gsPFC_Drive.sUCtrl.sUDcBusPiParams.fltIGain = PFC_START_U_I_GAIN;
	gsPFC_Drive.sUCtrl.sUDcBusPiParams.fltLowerLim = PFC_U_LOWER_LIMIT;
	gsPFC_Drive.sUCtrl.sUDcBusPiParams.fltUpperLim = PFC_U_UPPER_LIMIT;
	gsPFC_Drive.sUCtrl.sUDcBusPiParams.fltInErrK_1 = 0;
	gsPFC_Drive.sUCtrl.sUDcBusPiParams.fltIAccK_1 = 0;
	gsPFC_Drive.sUCtrl.bStopIntegFlag = 0;
	gsPFC_Drive.sUCtrl.fltIRef  = 0;
	gsPFC_Drive.sUCtrl.fltUDcBusErrLim = PFC_U_ERROR_LIMIT;

	/*------------------------- input voltage parameters --------------------------*/
	gsPFC_Drive.sUInPeakDetection.sFilter.sFltCoeff.fltB0    = PFC_U_IN_IIR_B0;
	gsPFC_Drive.sUInPeakDetection.sFilter.sFltCoeff.fltB1    = PFC_U_IN_IIR_B1;
	gsPFC_Drive.sUInPeakDetection.sFilter.sFltCoeff.fltA1    = PFC_U_IN_IIR_A1;
	GDFLIB_FilterIIR1Init_FLT(&gsPFC_Drive.sUInPeakDetection.sFilter);
	
	gsPFC_Drive.sUInPeakDetection.bSeveralPeaksDetectedFlag = 0; /* flag indicate input voltage rms calculation isn't complete*/
	gsPFC_Drive.sUInPeakDetection.bPeakFlag = 0;
	gsPFC_Drive.sUInPeakDetection.u16VoltagePeakCnt = 0;
	gsPFC_Drive.sUInPeakDetection.u16DownCnt = 0;
	gsPFC_Drive.sUInPeakDetection.u16UpCnt = 0;
	gsPFC_Drive.sUInPeakDetection.bPeakDetectEn = 0;
	gsPFC_Drive.sUInPeakDetection.fltUInMaxTemp = 0;
	gsPFC_Drive.sUInPeakDetection.fltUInMax = 0;
	gsPFC_Drive.sUInPeakDetection.f16Offset = 0;
#if PLL
	gsPFC_Drive.sPhaseDetector.bFallThrsdDetEn = 0;
	gsPFC_Drive.sPhaseDetector.bRiseThrsdDetEn = 0;
	gsPFC_Drive.sPhaseDetector.fltUInThreshold = PFC_VIN_PHASEDETECT_TH;
	gsPFC_Drive.sPhaseDetector.f16Freq = 0;
	gsPFC_Drive.sPhaseDetector.f16Phase = 0;
	gsPFC_Drive.sPhaseDetector.u16CntThrsd2Zero = 0;
	gsPFC_Drive.sPhaseDetector.sPeriodFilter.u16Sh = PHASE_DET_INPUT_VOLTAGE_PERIOD_FILTER_N;		
	gsPFC_Drive.sPhaseDetector.u16Step = (uint16_t)(4096*1024/(PFC_PWM_FREQ*10));
        gsPFC_Drive.sPhaseDetector.u16PosCnt = 0; 
        gsPFC_Drive.sPhaseDetector.u16NegCnt=0;
        gsPFC_Drive.sPhaseDetector.u8Polar = 0; 
        gsPFC_Drive.sPhaseDetector.u16Cnt = 0; 
        gsPFC_Drive.sPhaseDetector.f16Phase_refine=0;
 //	gsPFC_Drive.sPhaseDetector.sPeriodFilter.u16Step	
#endif
	
	/*------------------------- PFC current parameters --------------------------*/	
        gsPFC_Drive.sICtrlPh1.fltDuty = 0;

	gsPFC_Drive.sICtrlPh1.sIPiParams.fltPGain = PFC_I_P_GAIN1;
	gsPFC_Drive.sICtrlPh1.sIPiParams.fltIGain = PFC_I_I_GAIN1;
	gsPFC_Drive.sICtrlPh1.sIPiParams.fltLowerLim = PFC_I_LOWER_LIMIT;
	gsPFC_Drive.sICtrlPh1.sIPiParams.fltUpperLim = PFC_I_UPPER_LIMIT;
	gsPFC_Drive.sICtrlPh1.sIPiParams.fltInErrK_1 = 0;
	gsPFC_Drive.sICtrlPh1.sIPiParams.fltIAccK_1 = 0;
	gsPFC_Drive.sICtrlPh1.bStopIntegFlag = 0;
	
	gsPFC_Drive.sICtrlPh1.fltIFdbck = 0;
	gsPFC_Drive.sICtrlPh1.fltICorrect = 0;

	/* current offset parameters */
	gsPFC_Drive.sICtrlPh1.sIOffsetFilter.fltLambda = PFC_I_OFFSET_MA_WINDOW;
	gsPFC_Drive.sICtrlPh1.sIOffsetFilter.fltAcc = 0;
	
	/* flag variables */
	bEnableCheckDCBusVunder = 0;            /* disable flag for checking Output voltages under */
	bEnableCheckInputVunder = 0;            /* disable flag for checking Input voltages under */                  
	
/*	gsPFC_PwmVal.pui32PwmFrac2Val2 = (uint32_t *)(&FLEXPWM0->SM[3].FRACVAL2);
	gsPFC_PwmVal.pui32PwmFrac3Val3 = (uint32_t *)(&FLEXPWM0->SM[3].FRACVAL3);
	gsPFC_PwmVal.pui32PwmFrac4Val4 = (uint32_t *)(&FLEXPWM0->SM[3].FRACVAL4);
	gsPFC_PwmVal.pui32PwmFrac5Val5 = (uint32_t *)(&FLEXPWM0->SM[3].FRACVAL5);
*/
	/* ADC base address */
        g_sPFCAdcSensor.pToAdcBase = (ADC_Type *)ADC1;
  
	/* timing control variables */
	gsPFC_Drive.u16CounterTimeBase = TIMEBASE_1MS_FASTLOOP_CNTR;
	gsPFC_Drive.ui16CounterState = PFC_IOFFSET_CALIB_DURATION; //200ms duration for current offset detection
	
	bPFCVarInit = 1;
	}
	static uint16_t i=0;
	  
	if(i<=8)
	{
		i++;
		gsPFC_Drive.sUInPeakDetection.f16Offset += (gsPFC_Drive.sUInPeakDetection.f16Imos>>3);
	}
	PFC_FaultDetection();
	if(gsPFC_Drive.FaultIdPending.R)
	{
                Fault_Watch |= gsPFC_Drive.FaultIdPending.R;
		gsPFC_Ctrl.uiCtrl |= SM_CTRL_FAULT; 
	}
}

static void PFC_StateInitSlow()
{
    if(--gsPFC_Drive.ui16CounterState == 0)
    {
      //mark��to be confirmed how to calib adc offset
        if((gsPFC_Drive.sUInPeakDetection.f16Offset < PFC_I_HARDWARE_OFFSET-100)||(gsPFC_Drive.sUInPeakDetection.f16Offset > PFC_I_HARDWARE_OFFSET+100) )/* if offset over scale, use default */ 
        {
            gsPFC_Drive.sUInPeakDetection.f16Offset = PFC_I_HARDWARE_OFFSET;
        }      
        gsPFC_Ctrl.uiCtrl |= SM_CTRL_INIT_DONE;
    }
}
/***************************************************************************//*!
*
* @brief   Stop state
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_StateStopFast()
{
        PFC_DISABLE_PWM_OUTPUT();
	/********* Relay control ***********/
	if(gsPFC_Drive.sUCtrl.fltUDcBusFilt < gsPFC_Drive.sUCtrl.fltUDcBusRelayOff)
	{
		RELAY_OFF();
		gsPFC_Drive.sFlag.RelayFlag = 0;
	}
	/********* brake control ***********/
	if(gsPFC_Drive.sUCtrl.fltUDcBusFilt > gsPFC_Drive.sFaultThresholds.fltUDcBusOver)
	{
		BRAKE_ON();
		gsPFC_Drive.sFlag.BrakeFlag = 1;
	}
	else   
	{
		BRAKE_OFF();
		gsPFC_Drive.sFlag.BrakeFlag = 0;
	}

        PFC_FaultDetection();
	if(gsPFC_Drive.FaultIdPending.R)
	{
                Fault_Watch |= gsPFC_Drive.FaultIdPending.R;
		gsPFC_Ctrl.uiCtrl |= SM_CTRL_FAULT; 
	}
}

static void PFC_StateStopSlow()
{
    if(gsPFC_Drive.sUInPeakDetection.bSeveralPeaksDetectedFlag && gsPFC_Drive.sUInPeakDetection.fltUInMax > gsPFC_Drive.sFaultThresholds.fltUInUnder)
    {
    	gsPFC_Drive.sUCtrl.fltUDcBusRelayOn = MLIB_Mul_FLT(PFC_COEFF_U_DCB_RELAY_ON, gsPFC_Drive.sUInPeakDetection.fltUInMax); 
    	
    	if(!gsPFC_Drive.sFlag.RelayFlag)
    	{
    		if(gsPFC_Drive.sUCtrl.fltUDcBusFilt > gsPFC_Drive.sUCtrl.fltUDcBusRelayOn) // Delay some time for diode rectifier output ramping up, then turn on the relay
    		{
                    if(gsPFC_Drive.ui16CounterState >= 1)  
                    {
                        gsPFC_Drive.ui16CounterState--;   
                    } 				
                    //Zero crossing closing relay
                    if(gsPFC_Drive.ui16CounterState == 0 && gsPFC_Drive.sUInPeakDetection.fltUInFilt < PFC_VIN_THRESHOLD_RELAY_ON)
                    {
//                        RELAY_ON();
                            
                        /* HSCMP0_OUT is selected as trigger input for PWM0 FAULT channel 0 */                                                       

//                        gsPFC_Drive.sFlag.RelayFlag = 1;
//                        gsPFC_Drive.ui16CounterState = DELAY_BEFORE_CURRENT_CTRL;
                    }
    		}
    	}
    	else // Relay has been turned on
    	{
    		bEnableCheckInputVunder = 1;  //Enable input under voltage detection when PFC is enabled
    		if(gsPFC_Drive.sUCtrl.fltUDcBusFilt > gsPFC_Drive.sUCtrl.fltUDcBusRelayOn) // delay 200ms after close relay
    		{
                if(gsPFC_Drive.ui16CounterState >= 1)
                {
                    gsPFC_Drive.ui16CounterState--;
                }
    			// after the 200ms delay is over, start PFC when input voltage is low
    			if((gsPFC_Drive.ui16CounterState == 0) && (gsPFC_Drive.sUInPeakDetection.fltUInFilt < PFC_VIN_THRESHOLD_RELAY_ON) && (gsPFC_Drive.FaultIdPending.R == 0))
    			{
    			    gsPFC_Ctrl.uiCtrl |= SM_CTRL_START;
    			}  			
    		}
    	}
    }
}
/***************************************************************************//*!
*
* @brief   Run state
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_StateRunFast()
{
	float_t fltNum;
        float_t fltPFCDCMRatioNum, fltPFCDCMRatioDen;
	if(gsPFC_Drive.sFlag.RelayFlag == 0)
	{
	       gsPFC_Ctrl.uiCtrl |= SM_CTRL_STOP;
	}
	
	/* Compensate the feedback current for DCM 	
	Record the difference between the bus voltage and the input peak voltage*/
	float_t UInRestore = MLIB_Mul_FLT(gsPFC_Drive.sUInPeakDetection.fltUInMax, gsPFC_Drive.sPhaseDetector.fltSine);
	gsPFC_Drive.sUCtrl.fltVoltDiff_UDc_UIn = MLIB_Sub_FLT(PFC_U_DCB_REF, UInRestore);
	if(gsPFC_Drive.sUCtrl.fltVoltDiff_UDc_UIn < 1 )
	{
		gsPFC_Drive.sUCtrl.fltVoltDiff_UDc_UIn = 1;
	}
	
        gsPFC_Drive.fltDutyCCM = MLIB_Div_FLT(gsPFC_Drive.sUCtrl.fltVoltDiff_UDc_UIn , PFC_U_DCB_REF);
        fltPFCDCMRatioNum = MLIB_Mul_FLT(PFC_DCM_DUTY_COEFF, gsPFC_Drive.sUCtrl.fltUDcBusCtrlOut);
        fltPFCDCMRatioNum = MLIB_Mul_FLT(fltPFCDCMRatioNum, gsPFC_Drive.fltDutyCCM);
        fltPFCDCMRatioDen = gsPFC_Drive.sUInPeakDetection.fltUInMaxSquare;
        gsPFC_Drive.fltDutyDCM = GFLIB_Sqrt_FLT(MLIB_Div_FLT(fltPFCDCMRatioNum,fltPFCDCMRatioDen));
        if(gsPFC_Drive.fltDutyDCM > gsPFC_Drive.fltDutyCCM) // when calculated DCM duty is larger than CCM duty, it should be under CCM 
        {
                gsPFC_Drive.fltDutyComp = gsPFC_Drive.fltDutyCCM;
                gsPFC_Drive.sFlag.DCMFlag = 0;
        }
        else // when calculated CCM duty is larger than DCM duty, it should be under DCM 
        {
                gsPFC_Drive.fltDutyComp = gsPFC_Drive.fltDutyDCM;
                gsPFC_Drive.sFlag.DCMFlag = 1;
        }
	
        //In DCM mode, it is necessary to correct and compensate for the sampled current
	if(gsPFC_Drive.sFlag.DCMFlag)
	{
		fltNum = MLIB_Mul_FLT(PFC_U_DCB_REF, gsPFC_Drive.sICtrlPh1.fltDuty); // Duty_ph1 * Udc, mark
		gsPFC_Drive.sICtrlPh1.fltCompCoeff = MLIB_Div_FLT(fltNum, gsPFC_Drive.sUCtrl.fltVoltDiff_UDc_UIn);
		gsPFC_Drive.sICtrlPh1.fltICorrect = MLIB_Mul_FLT(gsPFC_Drive.sICtrlPh1.fltIFdbck, gsPFC_Drive.sICtrlPh1.fltCompCoeff); 
	}
	else
	{
		gsPFC_Drive.sICtrlPh1.fltICorrect = gsPFC_Drive.sICtrlPh1.fltIFdbck;
	}
	 
	if(gsPFC_Drive.sFlag.PWM_enable)
	{
    #if PLL
        gsPFC_Drive.sICtrlPh1.fltIRef = MLIB_Mul_FLT(gsPFC_Drive.sUCtrl.fltIRef, gsPFC_Drive.sPhaseDetector.fltSine);
        /* if current ref larger than 15A, then it keep as 15A */
        if(gsPFC_Drive.sICtrlPh1.fltIRef > 15.0) 
          gsPFC_Drive.sICtrlPh1.fltIRef = 15.0;
    #else
        gsPFC_Drive.sICtrlPh1.f16IRef = MLIB_Mul_FLT(gsPFC_Drive.sUCtrl.fltIRef, gsPFC_Drive.sUInPeakDetection.fltUInFilt);
    #endif
	   
        gsPFC_Drive.sICtrlPh1.sIPiParams.fltLowerLim = MLIB_Neg_FLT(gsPFC_Drive.fltDutyComp);
        gsPFC_Drive.sICtrlPh1.sIPiParams.fltUpperLim = MLIB_Sub_FLT(1.0f, gsPFC_Drive.fltDutyComp);
        gsPFC_Drive.sICtrlPh1.fltIError =  MLIB_Sub_FLT(gsPFC_Drive.sICtrlPh1.fltIRef, gsPFC_Drive.sICtrlPh1.fltICorrect);
        gsPFC_Drive.sICtrlPh1.fltDuty = GFLIB_CtrlPIpAW_FLT(gsPFC_Drive.sICtrlPh1.fltIError, &gsPFC_Drive.sICtrlPh1.bStopIntegFlag, &gsPFC_Drive.sICtrlPh1.sIPiParams);
        gsPFC_Drive.sICtrlPh1.fltDutyIout= gsPFC_Drive.sICtrlPh1.fltDuty = MLIB_Add_FLT(gsPFC_Drive.sICtrlPh1.fltDuty, gsPFC_Drive.fltDutyComp);
       
        PFC_PWM_UPDATE(gsPFC_Drive.sICtrlPh1.fltDuty);
	}
        PFC_FaultDetection();
        if(gsPFC_Drive.FaultIdPending.R)
        {
                Fault_Watch |= gsPFC_Drive.FaultIdPending.R;
                gsPFC_Ctrl.uiCtrl |= SM_CTRL_FAULT;
        }        
}

static void PFC_StateRunSlow()
{
    SM_StateSubMachineRun(&gsPFC_SubCtrl);
    if(gsPFC_Drive.sFlag.PWM_enable)
    {
            if(gsPFC_SubCtrl.eStateRunSub == LIGHTLOAD) // constant current reference in light-load mode
            {
                    gsPFC_Drive.sUCtrl.fltUDcBusCtrlOut = gsPFC_Drive.sUCtrl.sUDcBusPiParams.fltLowerLim;
            }
            else
            {
                    gsPFC_Drive.sUCtrl.fltUDcBusError = MLIB_Sub_FLT(gsPFC_Drive.sUCtrl.fltUDcBusReq, gsPFC_Drive.sUCtrl.fltUDcBusFilt);
                    gsPFC_Drive.sUCtrl.fltUDcBusError = GFLIB_Limit_FLT(gsPFC_Drive.sUCtrl.fltUDcBusError,-gsPFC_Drive.sUCtrl.fltUDcBusErrLim,gsPFC_Drive.sUCtrl.fltUDcBusErrLim);
                    gsPFC_Drive.sUCtrl.fltUDcBusCtrlOut = GFLIB_CtrlPIpAW_FLT(gsPFC_Drive.sUCtrl.fltUDcBusError, &gsPFC_Drive.sUCtrl.bStopIntegFlag, &gsPFC_Drive.sUCtrl.sUDcBusPiParams);
            }
#if PLL
            gsPFC_Drive.sUCtrl.fltIRef = MLIB_Div_FLT(gsPFC_Drive.sUCtrl.fltUDcBusCtrlOut,gsPFC_Drive.sUInPeakDetection.fltUInMax);
#else
            gsPFC_Drive.sUCtrl.fltIRef = MLIB_Div_FLT(gsPFC_Drive.sUCtrl.fltUDcBusCtrlOut,gsPFC_Drive.sUInPeakDetection.fltUInMaxSquare);
#endif
    
            /* theoretical duty cycle calculation for compensation */
    }
    
    PFC_FaultDetection();
    if(gsPFC_Drive.FaultIdPending.R)
    {
            gsPFC_Ctrl.uiCtrl |= SM_CTRL_FAULT;
    } 
}
/***************************************************************************//*!
*
* @brief   FAULT to INIT transition
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_TransFaultInit()
{
	bPFCVarInit = 0;
	gsPFC_Drive.u16CounterTimeBase = TIMEBASE_1MS_FASTLOOP_CNTR;
	gsPFC_Drive.ui16CounterState = PFC_IOFFSET_CALIB_DURATION; //Time duration for current offset detection
}

/***************************************************************************//*!
*
* @brief   INIT to FAULT transition
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_TransInitFault()
{
	gsPFC_Drive.u16CounterTimeBase = TIMEBASE_1MS_FASTLOOP_CNTR;
	gsPFC_Drive.ui16CounterState = FAULT_RELEASE_DURATION; //Time duration for fault release
}

/***************************************************************************//*!
*
* @brief   INIT to STOP transition
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_TransInitStop()
{
	gsPFC_Drive.u16CounterTimeBase = TIMEBASE_1MS_FASTLOOP_CNTR;
	gsPFC_Drive.ui16CounterState = UDCBUS_RAMPUP_DURATION; // Time duration for bus voltage rise 
}
/***************************************************************************//*!
*
* @brief   STOP to RUN transition
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_TransStopRun()
{
	gsPFC_SubCtrl.eStateRunSub = SOFTSTART;
	gsPFC_Drive.sUCtrl.fltUDcBusReq = gsPFC_Drive.sUCtrl.fltUDcBusFilt; // Set current real DC bus voltage to voltage reference
	GFLIB_RampInit_FLT(gsPFC_Drive.sUCtrl.fltUDcBusFilt, &gsPFC_Drive.sUCtrl.sUDcBusRampParams);
	gsPFC_Drive.u16CounterTimeBase = TIMEBASE_1MS_SLOWLOOP_CNTR;
	gsPFC_Drive.sFlag.PWM_enable = 1;
	gsPFC_Drive.sICtrlPh1.fltDuty = 0;
	PFC_PWM_UPDATE(gsPFC_Drive.sICtrlPh1.fltDuty);
	if(pfc_flag == true)
	{
	PFC_ENABLE_PWM_OUTPUT();
	}
	gsPFC_Ctrl.uiCtrl |= SM_CTRL_RUN_ACK;
}

/***************************************************************************//*!
*
* @brief   RUN to STOP transition
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_TransRunStop()
{
	PFC_DISABLE_PWM_OUTPUT();
	gsPFC_SubCtrl.eStateRunSub = SOFTSTART;
	bEnableCheckDCBusVunder = 0; //Disable bus voltage under detection when PFC is not working
	bEnableCheckInputVunder = 0; //Disable input voltage under detection when PFC is not working
	BRAKE_OFF(); //Disconnect load
	gsPFC_Drive.sFlag.BrakeFlag = 0;
	gsPFC_Drive.sICtrlPh1.fltDuty = 0;
	gsPFC_Drive.sICtrlPh1.sIPiParams.fltIAccK_1 = 0;
	gsPFC_Drive.sUCtrl.sUDcBusPiParams.fltIAccK_1 = gsPFC_Drive.sUCtrl.sUDcBusPiParams.fltLowerLim;
	PFC_PWM_UPDATE(gsPFC_Drive.sICtrlPh1.fltDuty);
	gsPFC_Ctrl.uiCtrl |= SM_CTRL_STOP_ACK;
}

/***************************************************************************//*!
*
* @brief   STOP to FAULT transition
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_TransStopFault()
{
	bEnableCheckInputVunder = 0; 
	 if(gsPFC_Drive.sFlag.BrakeFlag)
	 {
		 BRAKE_OFF();
		 gsPFC_Drive.sFlag.BrakeFlag = 0;
	 }
	 gsPFC_Drive.u16CounterTimeBase = TIMEBASE_1MS_FASTLOOP_CNTR; 
	 gsPFC_Drive.ui16CounterState = FAULT_RELEASE_DURATION; 
}

/***************************************************************************//*!
*
* @brief   RUN to FAULT transition
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_TransRunFault()
{
	PFC_DISABLE_PWM_OUTPUT();
	bEnableCheckDCBusVunder = 0;
	bEnableCheckInputVunder = 0; 
	gsPFC_Drive.sFlag.PWM_enable = 0;
	BRAKE_OFF();
	gsPFC_Drive.sFlag.BrakeFlag = 0;
	gsPFC_Drive.ui16CounterState = FAULT_RELEASE_DURATION;
	gsPFC_Drive.sICtrlPh1.fltDuty = 0;
	gsPFC_Drive.sICtrlPh1.sIPiParams.fltInErrK_1 = 0;
	PFC_PWM_UPDATE(gsPFC_Drive.sICtrlPh1.fltDuty);
}

/***************************************************************************//*!
*
* @brief   softstart sub-state, reference ramp up
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_StateRunSoftstart(void)
{
	// Real DC bus voltage is already larger than command, go to NORMAL sub-state
	if(gsPFC_Drive.sUCtrl.fltUDcBusFilt > gsPFC_Drive.sUCtrl.fltUDcBusCmd)
	{
		gsPFC_SubCtrl.eStateRunSub = NORMAL;
		gsPFC_Drive.sUCtrl.sUDcBusPiParams.fltIGain = PFC_U_I_GAIN;
		bEnableCheckDCBusVunder = 1; //Enable bus voltage under detection when PFC is working and soft-start state has finished
		gsPFC_Drive.u16CounterTimeBase = TIMEBASE_1MS_SLOWLOOP_CNTR;
		
		if(!gsPFC_Drive.sFlag.BrakeFlag)
		{
		    BRAKE_ON(); // To add loading on DC bus
		    gsPFC_Drive.sFlag.BrakeFlag = 1;
		}
		gsPFC_Drive.sUCtrl.fltUDcBusReq = gsPFC_Drive.sUCtrl.fltUDcBusCmd; 
	}
	// DC bus voltage request is behind command, increase the request voltage
	else if(gsPFC_Drive.sUCtrl.fltUDcBusReq < gsPFC_Drive.sUCtrl.fltUDcBusCmd)
	{
		if(--gsPFC_Drive.u16CounterTimeBase == 0)
		{
			gsPFC_Drive.sUCtrl.fltUDcBusReq = GFLIB_Ramp_FLT(gsPFC_Drive.sUCtrl.fltUDcBusCmd, &gsPFC_Drive.sUCtrl.sUDcBusRampParams);
			gsPFC_Drive.u16CounterTimeBase = TIMEBASE_1MS_SLOWLOOP_CNTR;
		}
	}
	// DC bus voltage request has reached command, but the real DC bus voltage is still behind command, go to NORMAL sub-state
	else
	{
//		gsPFC_SubCtrl.eStateRunSub = NORMAL;
		bEnableCheckDCBusVunder = 1;
		gsPFC_Drive.u16CounterTimeBase = TIMEBASE_1MS_SLOWLOOP_CNTR;

		if(!gsPFC_Drive.sFlag.BrakeFlag)
		{
			BRAKE_ON();
			gsPFC_Drive.sFlag.BrakeFlag = 1;
		}
	    gsPFC_Drive.sUCtrl.fltUDcBusReq = gsPFC_Drive.sUCtrl.fltUDcBusCmd;
	}
}

/***************************************************************************//*!
*
* @brief   Normal sub-state, interleave or not control and normal to lightload judgement
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_StateRunNormal(void)
{
	//Voltage loop feedforward control
	if(gsPFC_Drive.sUCtrl.fltUDcBusFilt > MLIB_Add_FLT(gsPFC_Drive.sUCtrl.fltUDcBusCmd,  PFC_DC_BUS_FLUCTUATION))
	{
               gsPFC_Drive.sUCtrl.sUDcBusPiParams.fltPGain = 3*PFC_U_P_GAIN;
		gsPFC_Drive.sUCtrl.sUDcBusPiParams.fltIAccK_1 -= (5.0f);   // approximate output voltage feed forward to restrain the overshoot
	}
	else if(gsPFC_Drive.sUCtrl.fltUDcBusFilt < MLIB_Sub_FLT(gsPFC_Drive.sUCtrl.fltUDcBusCmd,  PFC_DC_BUS_FLUCTUATION))
	{
                gsPFC_Drive.sUCtrl.sUDcBusPiParams.fltPGain = 3*PFC_U_P_GAIN; // approximate output voltage feed forward to restrain the overshoot
	}
        else
        {
               gsPFC_Drive.sUCtrl.sUDcBusPiParams.fltPGain = PFC_U_P_GAIN;                 
        }

#if BURST_MODE_EN
	/* Enter burst mode when DC bus voltage is larger than low overshoot command and voltage controller outputs lower limit
	 * or when DC bus voltage is larger than high overshoot command
	 * */
	if((gsPFC_Drive.sUCtrl.fltUDcBusCtrlOut == gsPFC_Drive.sUCtrl.sUDcBusPiParams.fltLowerLim && \
			gsPFC_Drive.sUCtrl.fltUDcBusFilt > MLIB_Add_FLT(gsPFC_Drive.sUCtrl.fltUDcBusCmd, PFC_DC_BUS_NORMAL_OVERSHOOT_DELTA_LOW)) ||\
			gsPFC_Drive.sUCtrl.fltUDcBusFilt > MLIB_Add_FLT(gsPFC_Drive.sUCtrl.fltUDcBusCmd, PFC_DC_BUS_NORMAL_OVERSHOOT_DELTA_HIGH))
	{
		PFC_DISABLE_PWM_OUTPUT();
		gsPFC_Drive.sFlag.PWM_enable = 0;
		gsPFC_SubCtrl.eStateRunSub = LIGHTLOAD;
		gsPFC_Drive.u16CounterTimeBase = TIMEBASE_1MS_SLOWLOOP_CNTR;
		gsPFC_Drive.ui16CounterState = BURST_OFF_MIN_DURATION;
	}
#endif
}

/***************************************************************************//*!
*
* @brief   lightload sub-state, lightload to normal judgement
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_StateRunLightload(void)
{
#if BURST_MODE_EN
	//Timing every 30ms. In light load mode, if the voltage drops within 8V after 30ms, it is acceptable. If this limit is exceeded or reduced by 15V, enter normal mode directly
	if(!gsPFC_Drive.sFlag.PWM_enable)
	{
		if(--gsPFC_Drive.u16CounterTimeBase == 0)
		{
			gsPFC_Drive.u16CounterTimeBase = TIMEBASE_1MS_SLOWLOOP_CNTR;
			if(gsPFC_Drive.ui16CounterState >= 1) 
			{
				gsPFC_Drive.ui16CounterState--;
			}
		}     		
	}
	else
	{
		gsPFC_Drive.u16CounterTimeBase = TIMEBASE_1MS_SLOWLOOP_CNTR;
		gsPFC_Drive.ui16CounterState = BURST_OFF_MIN_DURATION; //minimum burst off time duration
	}
	
	// When DC bus reaches a higher threshold, turn off the switch (burst off)
	if(gsPFC_Drive.sUCtrl.fltUDcBusFilt > gsPFC_Drive.sUCtrl.fltUDcBusBurstOff)
	{
		gsPFC_Drive.sFlag.PWM_enable = 0;
		PFC_DISABLE_PWM_OUTPUT();
		gsPFC_Drive.sICtrlPh1.sIPiParams.fltIAccK_1 = 0;
	}
	else if(gsPFC_Drive.sUCtrl.fltUDcBusFilt < gsPFC_Drive.sUCtrl.fltUDcBusBurstOn && !gsPFC_Drive.sFlag.PWM_enable)
	{
		if(gsPFC_Drive.ui16CounterState != 0)//burst off time is less than the minimum duration, 
			                                 //voltage drop too fast, directly return to normal mode
		{
			gsPFC_Drive.sUCtrl.sUDcBusPiParams.fltInErrK_1 = 0;
			gsPFC_Drive.sUCtrl.sUDcBusPiParams.fltIAccK_1 = gsPFC_Drive.sUCtrl.sUDcBusPiParams.fltLowerLim;	
			gsPFC_SubCtrl.eStateRunSub = NORMAL;	
		}
		// if burst off time is longer than the minimum duration, stay in light load mode and burst on
		gsPFC_Drive.sFlag.PWM_enable = 1;
		if(pfc_flag == true)
		{
		PFC_ENABLE_PWM_OUTPUT();
		}
	}
	
	// output voltage drop too low , directly return to normal mode
	if(gsPFC_Drive.sUCtrl.fltUDcBusFilt < MLIB_Sub_FLT(gsPFC_Drive.sUCtrl.fltUDcBusCmd, PFC_DC_BUS_BURST_UNDER_DELTA))
	{
          gsPFC_Drive.sUCtrl.sUDcBusPiParams.fltInErrK_1 = 0;
          gsPFC_Drive.sUCtrl.sUDcBusPiParams.fltIAccK_1 = gsPFC_Drive.sUCtrl.sUDcBusPiParams.fltLowerLim;
		gsPFC_SubCtrl.eStateRunSub = NORMAL;
          gsPFC_Drive.sFlag.PWM_enable = 1;
      	if(pfc_flag == true)
      	{
      	PFC_ENABLE_PWM_OUTPUT();
      	}
	}	
#endif
}

/******************************************************************************
* Inline functions
******************************************************************************/
void PFC_PWM_UPDATE(float_t Duty)//mark, need to confirm the calculation
{
    int16_t w16ModuloHalf;
    int16_t w16PwmValPhA;

    GFLIB_Limit_FLT(Duty, 0.0f, 0.95f);
    /* Get PWM_modulo/2 from PWM register, always correct regardless of PWM runtime setting, variable used for update of duty cycles of all 3 phases */
    w16ModuloHalf = (FLEXPWM1->SM[3].VAL1+1);
    
    /* Phase A - duty cycle calculation */
    w16PwmValPhA = (w16ModuloHalf * MLIB_Conv_F16f(Duty)) >> 15;
    
    FLEXPWM1->SM[3].VAL2 = (uint16_t)(-w16PwmValPhA); // rising edge value register update
    FLEXPWM1->SM[3].VAL3 = (uint16_t)w16PwmValPhA;  // falling edge value register update, no need to calculate it

    FLEXPWM1->MCTRL |= PWM_MCTRL_CLDOK(8);
    FLEXPWM1->MCTRL |= PWM_MCTRL_LDOK(8);
    FLEXPWM1->MCTRL |= PWM_MCTRL_RUN(8);
}

/* PFC_Phase_detect is called in current control loop of PFC when compilation condition "PLL" is not zero*/
//mark, need to calculation the float/frac unit 
void PFC_Phase_detect(PFCDEF_DRIVE_T *ptr)
{	
	ptr->sPhaseDetector.u16TimeCnt++;
	
	// Voltage zero-crossing detection when input AC voltage is indeed falling down
	if(ptr->sPhaseDetector.bFallThrsdDetEn)
	{
		//The input voltage is decreasing and the amplitude is less than 65V
		if(ptr->sUInPeakDetection.fltUInFilt < ptr->sPhaseDetector.fltUInThreshold)
		{
			ptr->sPhaseDetector.u16FallOkCnt++;
		}
		else
		{
			ptr->sPhaseDetector.u16FallOkCnt = 0;	
		}
		if(ptr->sPhaseDetector.u16FallOkCnt >= PHASE_DET_INPUT_VOLTAGE_FALL_CNT_THRSD)
		{
			//Confirmed input voltage drop edge has been detected, with 3 consecutive drops
			ptr->sPhaseDetector.bFallThrsdDetEn = 0; // Falling voltage threshold has been detected, disable the detection
			ptr->sPhaseDetector.u16FallOkCnt = 0;
			//If two or more sinusoidal descent periods have been detected, calculate the input voltage frequency
			if(ptr->sUInPeakDetection.u16VoltagePeakCnt > 2)
			{
				ptr->sPhaseDetector.u16Period = ptr->sPhaseDetector.u16TimeCnt;
				ptr->sPhaseDetector.u16PeriodFilt = GDFLIB_FilterMA_F16(ptr->sPhaseDetector.u16Period, &ptr->sPhaseDetector.sPeriodFilter);
				ptr->sPhaseDetector.f16Freq = MLIB_Div_F16(1, ptr->sPhaseDetector.u16PeriodFilt);					
			}
			else if(ptr->sUInPeakDetection.u16VoltagePeakCnt == 2) //Consider different starting voltage point, discard the first period value
			{
				ptr->sPhaseDetector.sPeriodFilter.a32Acc = ptr->sPhaseDetector.u16TimeCnt * (PHASE_DET_INPUT_VOLTAGE_PERIOD_FILTER_POINTS-1);
			}
			//At this moment, it is located 3 fast loop cycles after the peak of the input voltage
			ptr->sPhaseDetector.u16TimeCnt = 0;
		}
	}
	// Voltage zero-crossing detection when input AC voltage is rising up
	else if(ptr->sPhaseDetector.bRiseThrsdDetEn)
	{
		if(ptr->sUInPeakDetection.fltUInFilt > ptr->sPhaseDetector.fltUInThreshold)
		{
			ptr->sPhaseDetector.u16RiseOkCnt ++;
		}
		else 
		{
			ptr->sPhaseDetector.u16RiseOkCnt = 0;
		}
		if(ptr->sPhaseDetector.u16RiseOkCnt >= PHASE_DET_INPUT_VOLTAGE_RISE_CNT_THRSD)
		{
			//The input voltage waveform rises continuously three times
			ptr->sPhaseDetector.bRiseThrsdDetEn = 0; // Rising voltage threshold has been detected, disable the detection
			ptr->sPhaseDetector.u16RiseOkCnt = 0;
			//Calculate the duration of delaying from the maximum input voltage by 3 fast loop cycles to zero
			ptr->sPhaseDetector.u16CntThrsd2Zero = (ptr->sPhaseDetector.u16TimeCnt >> 1) - PHASE_DET_INPUT_VOLTAGE_FALL_CNT_THRSD;
		}
	}
	
	if(ptr->sPhaseDetector.u16TimeCnt == ptr->sPhaseDetector.u16CntThrsd2Zero)
	{
		ptr->sPhaseDetector.f16Phase = 0;
	}
	else
	{
		ptr->sPhaseDetector.f16Phase = MLIB_AddSat_F16(ptr->sPhaseDetector.f16Phase, ptr->sPhaseDetector.f16Freq);
	}
        ptr->sPhaseDetector.f16Phase_refine= (ptr->sPhaseDetector.f16Phase+1000)%32767;
	ptr->sPhaseDetector.fltSine = GFLIB_Sin_FLTa((acc32_t)(ptr->sPhaseDetector.f16Phase_refine));
}

void PFC_UInPeak_detect(PFCDEF_DRIVE_T *ptr)
{
	//After enabling peak detection, if the input voltage collected is greater than the recorded value of max, it will be retained
	if((ptr->sUInPeakDetection.fltUInFilt > ptr->sUInPeakDetection.fltUInMaxTemp) && ptr->sUInPeakDetection.bPeakDetectEn)
	{
		gsPFC_Drive.sUInPeakDetection.fltUInMaxTemp = gsPFC_Drive.sUInPeakDetection.fltUInFilt;
	}
	
	// Voltage rising detection, enable peak detection when voltage is continuously rising 
	if(ptr->sUInPeakDetection.fltUInFilt > ptr->sUInPeakDetection.fltLastUInFilt)
	{
		ptr->sUInPeakDetection.u16UpCnt++;
		ptr->sUInPeakDetection.u16DownCnt = 0;
		if((ptr->sUInPeakDetection.u16UpCnt >= PEAK_DET_INPUT_VOLTAGE_RISE_NUM) && !ptr->sUInPeakDetection.bPeakDetectEn)
		{
			ptr->sUInPeakDetection.bPeakDetectEn = 1; //start peak detection of this cycle when confirmed that the voltage is rising
#if PLL
			ptr->sPhaseDetector.bRiseThrsdDetEn = 1; /*for phase detect, start rising edge threshold value
			                                         detection when confirmed that the voltage is rising*/
#endif
		}
	}
	// Voltage falling detection, disable peak detection when voltage is continuously falling
	// Peak voltage is detected in this procedure
	else if(ptr->sUInPeakDetection.fltUInFilt < ptr->sUInPeakDetection.fltLastUInFilt)
	{
		ptr->sUInPeakDetection.u16UpCnt = 0;
		ptr->sUInPeakDetection.u16DownCnt++;
		
		/*disable peak detection when confirmed that the voltage is falling, the stored peak value f16UInMaxTemp is the peak voltage of this cycle. */
		if((ptr->sUInPeakDetection.u16DownCnt >= PEAK_DET_INPUT_VOLTAGE_FALL_NUM) && ptr->sUInPeakDetection.bPeakDetectEn)
		{
			ptr->sUInPeakDetection.bPeakDetectEn = 0; 
#if PLL		
			ptr->sPhaseDetector.bFallThrsdDetEn = 1; /*for phase detect, start falling edge threshold value
			                                         detection when confirmed that the voltage is falling*/
#endif
			ptr->sUInPeakDetection.bPeakFlag = 1; 
			ptr->sUInPeakDetection.fltUInMax = ptr->sUInPeakDetection.fltUInMaxTemp;
			ptr->sUInPeakDetection.fltUInMaxSquare = MLIB_Mul_FLT(ptr->sUInPeakDetection.fltUInMax, ptr->sUInPeakDetection.fltUInMax);

			// adjust bus voltage controller output according to the input voltage to keep the same minimum and maximum allowable current 
			if(gsPFC_SubCtrl.eStateRunSub == SOFTSTART) 
			{
				ptr->sUCtrl.sUDcBusPiParams.fltLowerLim = 0.0;	
			}
			else
			{
				ptr->sUCtrl.sUDcBusPiParams.fltLowerLim = MLIB_Mul_FLT(LOW_CURRENT, ptr->sUInPeakDetection.fltUInMax);
			}
			ptr->sUCtrl.sUDcBusPiParams.fltUpperLim = MLIB_Mul_FLT(HIGH_CURRENT, ptr->sUInPeakDetection.fltUInMax);
			ptr->sUInPeakDetection.fltUInMaxTemp = 0;		
		}
	}	
	ptr->sUInPeakDetection.fltLastUInFilt = ptr->sUInPeakDetection.fltUInFilt;
	
	/* PFC starts to work at least PEAK_DET_INPUT_VOLTAGE_PEAK_NUM number of peaks have been detected */
	if(!ptr->sUInPeakDetection.bSeveralPeaksDetectedFlag && ptr->sUInPeakDetection.bPeakFlag)
	{
		ptr->sUInPeakDetection.bPeakFlag = 0;
		ptr->sUInPeakDetection.u16VoltagePeakCnt ++;
		if(ptr->sUInPeakDetection.u16VoltagePeakCnt > PEAK_DET_INPUT_VOLTAGE_PEAK_NUM)
		{
			ptr->sUInPeakDetection.bSeveralPeaksDetectedFlag = 1;
		}
	}
}
