/*
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#define Nonlinear_observer
#include "pmsm_control.h"
#include "current_reconstruction.h"
#include "Flux_observer.h"
#include "M2_observer.h"
#include "mc_periph_init.h"
#include "MC_Parameters.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
extern Flux_Obs_t Flux_Obs;
extern FLUX_OBSRV_T sFluxObsrv;
extern GDFLIB_FILTER_IIR1_T_F32 sFluxSpdFilter;
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
void MCS_FocFieldWeakeningCtrl(mcs_pmsm_foc_t *psFocPMSM, mcs_pmsm_fw_t *psFocFw, mcs_speed_t *psSpeed)
{
      if (MLIB_Abs_FLT(psSpeed->fltSpeed) > psFocFw->fltSpeedFwOn)
      {
          /* Begin - field weakening controller */    
          psFocFw->fltUFwError = 0.01*MLIB_Sub_FLT(psFocPMSM->sIqPiParams.fltUpperLim, MLIB_Abs_FLT(psFocPMSM->sUDQReq.fltQ));
          psFocFw->fltIFwError = 2*MLIB_Abs_FLT(MLIB_Sub_FLT(psFocPMSM->sIDQReq.fltQ, psFocPMSM->sIDQ.fltQ));
          psFocFw->fltFwError = MLIB_Sub_FLT(psFocFw->fltUFwError, psFocFw->fltIFwError);
          psFocFw->fltFwErrorFilt = GDFLIB_FilterIIR1_FLT(psFocFw->fltFwError, &psFocFw->sFwErrorFilter);

          psFocFw->bFwPiSatFlag = psFocFw->sFwPiParams.bLimFlag | psFocPMSM->bIdPiStopInteg;
          psFocPMSM->sIDQReq.fltD = GFLIB_CtrlPIpAW_FLT(psFocFw->fltFwErrorFilt, &psFocFw->bFwPiSatFlag, &psFocFw->sFwPiParams);
          /* End - field weakening controller */                        
      }
      else
      {
              psFocFw->bFwPiSatFlag = 0;
              psFocPMSM->sIDQReq.fltD  = 0;
              psFocFw->sFwPiParams.fltIAccK_1 = 0;
      }
}

bool_t bDeadTimeComp=1;
float_t Temp,TempCnt, fltDTComp;
float_t fltUCompCoeff=0.35;
float_t fltUComp=3.0;
float_t fltUAlphaComp,fltUBetaComp;
float_t fltIAbs;
extern GFLIB_LUT1D_T_FLT sParam;
static float_t Voltage_Comp_Table[22]={0.0F,
1.20014F,
1.53214F,
2.01713F,
2.64546F,
3.42211F,
4.43388F,
5.60944F,
6.81397F,
7.93848F,
8.57528F,
8.27676F,
7.95494F,
8.77263F,
9.62929F,
10.29340F,
10.83213F,
11.28219F,
11.66459F,
12.01182F,
12.29362F,
12.56260F
};
void M2_PMSMFocCtrl(mcs_pmsm_foc_t *psFocPMSM)
{
    /* pass electrical position from outside function if enabled else estimated
     * electrical position is selected */
    if (psFocPMSM->bPosExtOn)
    {
        psFocPMSM->f16PosEl = psFocPMSM->f16PosElExt;
    }
    else
    {
        psFocPMSM->f16PosEl = psFocPMSM->f16PosElEst;
    }

    /* Position angle of the last PWM update */
    psFocPMSM->sAnglePosEl.fltSin = GFLIB_Sin_FLTa((acc32_t)psFocPMSM->f16PosElEst);
    psFocPMSM->sAnglePosEl.fltCos = GFLIB_Cos_FLTa((acc32_t)psFocPMSM->f16PosElEst);

    /* 3-phase to 2-phase transformation to stationary ref. frame */
    GMCLIB_Clark_FLT(&psFocPMSM->sIABC, &psFocPMSM->sIAlBe);

    if(bDeadTimeComp)
    {
      TempCnt=0.0F;
      Temp=0.01F;
      fltIAbs = GFLIB_Sqrt_FLT(psFocPMSM->sIAlBe.fltAlpha*psFocPMSM->sIAlBe.fltAlpha+psFocPMSM->sIAlBe.fltBeta*psFocPMSM->sIAlBe.fltBeta);
      while((Temp<fltIAbs)&&(TempCnt<21))
      {
        TempCnt++;
        Temp*=1.2160418F;
      }
      if(TempCnt!=0)
      {
        Temp /= 1.2160418F;
        TempCnt+=(fltIAbs-Temp)/(0.2160418F*Temp);
      }else
      {
        TempCnt+=fltIAbs/0.01F;
      }
      fltDTComp = GFLIB_Lut1D_FLT(TempCnt,Voltage_Comp_Table,&sParam);
      fltUComp = fltDTComp*fltUCompCoeff;
      
      if(psFocPMSM->sIDQReq.fltQ>0)
      {
        fltUAlphaComp = MLIB_Mul_FLT(psFocPMSM->sAnglePosEl.fltSin, fltUComp);
        fltUBetaComp = MLIB_Mul_FLT(-psFocPMSM->sAnglePosEl.fltCos, fltUComp);
      }
      else
      {
        fltUAlphaComp = -MLIB_Mul_FLT(psFocPMSM->sAnglePosEl.fltSin, fltUComp);
        fltUBetaComp = -MLIB_Mul_FLT(-psFocPMSM->sAnglePosEl.fltCos, fltUComp);
      }
      psFocPMSM->sUAlBeReq.fltAlpha+=fltUAlphaComp;
      psFocPMSM->sUAlBeReq.fltBeta+=fltUBetaComp;
    }

    Flux_Obs.Id = psFocPMSM->sIDQ.fltD;
    Flux_Obs.sIAlBe.fltAlpha = psFocPMSM->sIAlBe.fltAlpha;
    Flux_Obs.sIAlBe.fltBeta = psFocPMSM->sIAlBe.fltBeta;
    Flux_Obs.sUAlBe.fltAlpha = psFocPMSM->sUAlBeReq.fltAlpha+fltUAlphaComp;
    Flux_Obs.sUAlBe.fltBeta = psFocPMSM->sUAlBeReq.fltBeta+fltUBetaComp;
    Flux_update(&Flux_Obs);   
    
    /* 2-phase to 2-phase transformation to rotary ref. frame */
    GMCLIB_Park_FLT(&psFocPMSM->sIAlBe, &psFocPMSM->sAnglePosEl, &psFocPMSM->sIDQ);
    GMCLIB_Park_FLT(&psFocPMSM->sUAlBeReq, &psFocPMSM->sAnglePosEl, &psFocPMSM->sUDQEst);

#ifndef M1_DEADTIME_TEST
#ifndef M2_DEADTIME_TEST
    psFocPMSM->f16PosElEst = (frac16_t)AMCLIB_TrackObsrv_A32af(psFocPMSM->acc32BemfErr, &psFocPMSM->sTo);
#endif
#endif

    psFocPMSM->sTo.fltSpeed = MLIB_ConvSc_FLTsf(Flux_Obs.f16SpdEstimOTF, M2_N_MAX);
    psFocPMSM->f16PosElEst = Flux_Obs.f16PositionEst;

    /* Speed estimation filter  */
    psFocPMSM->fltSpeedElEst = GDFLIB_FilterIIR1_FLT(psFocPMSM->sTo.fltSpeed, &psFocPMSM->sSpeedElEstFilt);

    /* for open loop control enabled parallel running of observer and FOC
     * open loop electrical position passed to rest of FOC */
    if (psFocPMSM->bOpenLoop || psFocPMSM->bPosExtOn)
    {
        psFocPMSM->sAnglePosEl.fltSin = GFLIB_Sin_FLTa((acc32_t)psFocPMSM->f16PosEl);
        psFocPMSM->sAnglePosEl.fltCos = GFLIB_Cos_FLTa((acc32_t)psFocPMSM->f16PosEl);
        GMCLIB_Park_FLT(&psFocPMSM->sIAlBe, &psFocPMSM->sAnglePosEl, &psFocPMSM->sIDQ);
    }

    /* perform current control loop if enabled */
    if (psFocPMSM->bCurrentLoopOn)
    {
        /* D current error calculation */
        psFocPMSM->sIDQError.fltD = MLIB_Sub_FLT(psFocPMSM->sIDQReq.fltD, psFocPMSM->sIDQ.fltD);

        /* Q current error calculation */
        psFocPMSM->sIDQError.fltQ = MLIB_Sub_FLT(psFocPMSM->sIDQReq.fltQ, psFocPMSM->sIDQ.fltQ);

        /*** D - controller limitation calculation ***/
        psFocPMSM->sIdPiParams.fltLowerLim = MLIB_MulNeg_FLT(psFocPMSM->fltDutyCycleLimit, psFocPMSM->fltUDcBusFilt);
        psFocPMSM->sIdPiParams.fltUpperLim = MLIB_Mul_FLT(psFocPMSM->fltDutyCycleLimit, psFocPMSM->fltUDcBusFilt);

        /* D current PI controller */
        psFocPMSM->sUDQReq.fltD =
            GFLIB_CtrlPIpAW_FLT(psFocPMSM->sIDQError.fltD, &psFocPMSM->bIdPiStopInteg, &psFocPMSM->sIdPiParams);

        /*** Q - controller limitation calculation ***/
        psFocPMSM->sIqPiParams.fltUpperLim =
            GFLIB_Sqrt_FLT(psFocPMSM->sIdPiParams.fltUpperLim * psFocPMSM->sIdPiParams.fltUpperLim -
                           psFocPMSM->sUDQReq.fltD * psFocPMSM->sUDQReq.fltD);
        psFocPMSM->sIqPiParams.fltLowerLim = MLIB_Neg_FLT(psFocPMSM->sIqPiParams.fltUpperLim);

        /* Q current PI controller */
        psFocPMSM->sUDQReq.fltQ =
            GFLIB_CtrlPIpAW_FLT(psFocPMSM->sIDQError.fltQ, &psFocPMSM->bIqPiStopInteg, &psFocPMSM->sIqPiParams);
    }

    /* 2-phase to 2-phase transformation to stationary ref. frame */
    GMCLIB_ParkInv_FLT(&psFocPMSM->sUDQReq, &psFocPMSM->sAnglePosEl, &psFocPMSM->sUAlBeReq);

    /* DCBus ripple elimination */
    GMCLIB_ElimDcBusRipFOC_F16ff(psFocPMSM->fltUDcBusFilt, &psFocPMSM->sUAlBeReq, &psFocPMSM->sUAlBeCompFrac);

    /* space vector modulation */
    psFocPMSM->ui16SectorSVM = GMCLIB_SvmStd_F16(&psFocPMSM->sUAlBeCompFrac, &psFocPMSM->sDutyABC);
}

void M1_PMSMFocPositionObsDQ(mcs_pmsm_foc_t *psFocPMSM)
{
    frac16_t f16FluxObsrvSpdFilt;
    /* pass electrical position from outside function if enabled else estimated
     * electrical position is selected */
    if (psFocPMSM->bPosExtOn)
    {
        psFocPMSM->f16PosEl = psFocPMSM->f16PosElExt;
    }
    else
    {
        psFocPMSM->f16PosEl = psFocPMSM->f16PosElEst;
    }
    
    //flux observer
    GMCLIB_Clark_F16(&psFocPMSM->sIABCFrac, &psFocPMSM->sIAlBeFrac);
    sFluxObsrv.f16UAlpha = MLIB_ConvSc_F16ff(psFocPMSM->sUAlBeReq.fltAlpha, M1_U_MAX);
    sFluxObsrv.f16UBeta = MLIB_ConvSc_F16ff(psFocPMSM->sUAlBeReq.fltBeta, M1_U_MAX);
    sFluxObsrv.f16IAlpha = psFocPMSM->sIAlBeFrac.f16Alpha;
    sFluxObsrv.f16IBeta = psFocPMSM->sIAlBeFrac.f16Beta;
    sFluxObsrv.Update(&sFluxObsrv);
    f16FluxObsrvSpdFilt = GDFLIB_FilterIIR1_F16(sFluxObsrv.f16SpeedEstim, &sFluxSpdFilter);
    
    psFocPMSM->fltSpeedElEst = MLIB_ConvSc_FLTsf(f16FluxObsrvSpdFilt, M1_N_MAX);
    psFocPMSM->f16PosElEst = sFluxObsrv.f16PosEstim;
    
    /* Position angle of the last PWM update */
    psFocPMSM->sAnglePosEl.fltSin = GFLIB_Sin_FLTa((acc32_t)psFocPMSM->f16PosElEst);
    psFocPMSM->sAnglePosEl.fltCos = GFLIB_Cos_FLTa((acc32_t)psFocPMSM->f16PosElEst);
    
    /* 3-phase to 2-phase transformation to stationary ref. frame */
    GMCLIB_Clark_FLT(&psFocPMSM->sIABC, &psFocPMSM->sIAlBe);

    /* 2-phase to 2-phase transformation to rotary ref. frame */
    GMCLIB_Park_FLT(&psFocPMSM->sIAlBe, &psFocPMSM->sAnglePosEl, &psFocPMSM->sIDQ);
}

#define HALF_PWM_CNT_MOD FRAC16(0.95)//PWM output duty limit
float_t fltDeadValue=1.12/1000*6.25;//[duty]
float_t zeroCurLim=0.05;//[A]
void MCS_PMSMDeadTimeComp(mcs_pmsm_foc_t *psFocPMSM)
{
    if(psFocPMSM->sIABC.fltA >  zeroCurLim) 
    { 
      psFocPMSM->sDutyABC.f16A = MLIB_AddSat_F16(psFocPMSM->sDutyABC.f16A, FRAC16(fltDeadValue));
    if(psFocPMSM->sDutyABC.f16A > HALF_PWM_CNT_MOD) 
      psFocPMSM->sDutyABC.f16A = HALF_PWM_CNT_MOD;
    }
    else if(psFocPMSM->sIABC.fltA < -zeroCurLim) 
    { 
      psFocPMSM->sDutyABC.f16A = MLIB_Sub_F16(psFocPMSM->sDutyABC.f16A, FRAC16(fltDeadValue)); 
      if(psFocPMSM->sDutyABC.f16A < 0) 
        psFocPMSM->sDutyABC.f16A = 0; 
    }
    
    if(psFocPMSM->sIABC.fltB >  zeroCurLim) 
    { 
        psFocPMSM->sDutyABC.f16B = MLIB_AddSat_F16(psFocPMSM->sDutyABC.f16B, FRAC16(fltDeadValue));
      if(psFocPMSM->sDutyABC.f16B > HALF_PWM_CNT_MOD) 
        psFocPMSM->sDutyABC.f16B = HALF_PWM_CNT_MOD;
    }
    else if(psFocPMSM->sIABC.fltB < -zeroCurLim) 
    { 
      psFocPMSM->sDutyABC.f16B = MLIB_Sub_F16(psFocPMSM->sDutyABC.f16B, FRAC16(fltDeadValue)); 
      if(psFocPMSM->sDutyABC.f16B < 0) 
          psFocPMSM->sDutyABC.f16B = 0; 
    }
    
    if(psFocPMSM->sIABC.fltC >  zeroCurLim) 
    { 
      psFocPMSM->sDutyABC.f16C = MLIB_AddSat_F16(psFocPMSM->sDutyABC.f16C, FRAC16(fltDeadValue));
      if(psFocPMSM->sDutyABC.f16C > HALF_PWM_CNT_MOD) 
        psFocPMSM->sDutyABC.f16C = HALF_PWM_CNT_MOD;
    }
    else if(psFocPMSM->sIABC.fltC < -zeroCurLim) 
    { 
      psFocPMSM->sDutyABC.f16C = MLIB_Sub_F16(psFocPMSM->sDutyABC.f16C, FRAC16(fltDeadValue)); 
      if(psFocPMSM->sDutyABC.f16C < 0) 
          psFocPMSM->sDutyABC.f16C = 0; 
    }
}

float_t fltIdComp=0;
void MCS_PMSMFocCurrentCtrl(mcs_pmsm_foc_t *psFocPMSM)
{
    /* for open loop control enabled parallel running of observer and FOC
     * open loop electrical position passed to rest of FOC */
    if (psFocPMSM->bOpenLoop || psFocPMSM->bPosExtOn)
    {
        psFocPMSM->sAnglePosEl.fltSin = GFLIB_Sin_FLTa((acc32_t)psFocPMSM->f16PosEl);
        psFocPMSM->sAnglePosEl.fltCos = GFLIB_Cos_FLTa((acc32_t)psFocPMSM->f16PosEl);
        GMCLIB_Park_FLT(&psFocPMSM->sIAlBe, &psFocPMSM->sAnglePosEl, &psFocPMSM->sIDQ);
    }

    /* perform current control loop if enabled */
    if (psFocPMSM->bCurrentLoopOn)
    {
        /* D current error calculation */
        psFocPMSM->sIDQError.fltD = MLIB_Sub_FLT(psFocPMSM->sIDQReq.fltD, psFocPMSM->sIDQ.fltD) + fltIdComp;
        
        /* Q current error calculation */
        psFocPMSM->sIDQError.fltQ = MLIB_Sub_FLT(psFocPMSM->sIDQReq.fltQ, psFocPMSM->sIDQ.fltQ);
        
        /*** D - controller limitation calculation ***/
        psFocPMSM->sIdPiParams.fltLowerLim = MLIB_MulNeg_FLT(psFocPMSM->fltDutyCycleLimit, psFocPMSM->fltUDcBusFilt);
        psFocPMSM->sIdPiParams.fltUpperLim = MLIB_Mul_FLT(psFocPMSM->fltDutyCycleLimit, psFocPMSM->fltUDcBusFilt);

        /* D current PI controller */
        psFocPMSM->sUDQReq.fltD = GFLIB_CtrlPIpAW_FLT(psFocPMSM->sIDQError.fltD, &psFocPMSM->bIdPiStopInteg, &psFocPMSM->sIdPiParams);

        /*** Q - controller limitation calculation ***/
        psFocPMSM->sIqPiParams.fltUpperLim =
            GFLIB_Sqrt_FLT(psFocPMSM->sIdPiParams.fltUpperLim * psFocPMSM->sIdPiParams.fltUpperLim -
                           psFocPMSM->sUDQReq.fltD * psFocPMSM->sUDQReq.fltD);
        psFocPMSM->sIqPiParams.fltLowerLim = MLIB_Neg_FLT(psFocPMSM->sIqPiParams.fltUpperLim);

        /* Q current PI controller */
        psFocPMSM->sUDQReq.fltQ = GFLIB_CtrlPIpAW_FLT(psFocPMSM->sIDQError.fltQ, &psFocPMSM->bIqPiStopInteg, &psFocPMSM->sIqPiParams);
    }

    /* 2-phase to 2-phase transformation to stationary ref. frame */
    GMCLIB_ParkInv_FLT(&psFocPMSM->sUDQReq, &psFocPMSM->sAnglePosEl, &psFocPMSM->sUAlBeReq);

    /* DCBus ripple elimination */
    GMCLIB_ElimDcBusRipFOC_F16ff(psFocPMSM->fltUDcBusFilt, &psFocPMSM->sUAlBeReq, &psFocPMSM->sUAlBeCompFrac);

    /* space vector modulation */
    psFocPMSM->ui16SectorSVM = SvmU0n_t1t2(&psFocPMSM->sUAlBeCompFrac, &psFocPMSM->sDutyABC, &t1_t2, &phaseCurrents);
}

void M1_PMSMFocPhaseShiftCtrl(mcs_pmsm_foc_t *psFocPMSM)
{
    int16_t w16DutyAl,w16DutyBl,w16DutyCl; // duty of left part
    int16_t w16DutyAr,w16DutyBr,w16DutyCr; // duty of right part
    PhaseShift_calculation(&phaseCurrents, &t1_t2, psFocPMSM->ui16SectorSVM);
    w16DutyAl = MLIB_Mul_F16(MLIB_AddSat_F16(psFocPMSM->sDutyABC.f16A,phaseCurrents.pwm_delay[0]), VAL1_F);
    w16DutyAr = MLIB_Mul_F16(MLIB_AddSat_F16(psFocPMSM->sDutyABC.f16A,phaseCurrents.pwm_delay[1]), VAL1_F);
    w16DutyBl = MLIB_Mul_F16(MLIB_AddSat_F16(psFocPMSM->sDutyABC.f16B,phaseCurrents.pwm_delay[2]), VAL1_F);
    w16DutyBr = MLIB_Mul_F16(MLIB_AddSat_F16(psFocPMSM->sDutyABC.f16B,phaseCurrents.pwm_delay[3]), VAL1_F);
    w16DutyCl = MLIB_Mul_F16(MLIB_AddSat_F16(psFocPMSM->sDutyABC.f16C,phaseCurrents.pwm_delay[4]), VAL1_F);
    w16DutyCr = MLIB_Mul_F16(MLIB_AddSat_F16(psFocPMSM->sDutyABC.f16C,phaseCurrents.pwm_delay[5]), VAL1_F);

    phaseCurrents.dutyCycleChannel0 = MLIB_Neg_F16(w16DutyAl);
    phaseCurrents.dutyCycleChannel1 = w16DutyAr;
    phaseCurrents.dutyCycleChannel2 = MLIB_Neg_F16(w16DutyBl);
    phaseCurrents.dutyCycleChannel3 = w16DutyBr;
    phaseCurrents.dutyCycleChannel4 = MLIB_Neg_F16(w16DutyCl);
    phaseCurrents.dutyCycleChannel5 = w16DutyCr;
    
    FLEXPWM0->SM[0].VAL2 = phaseCurrents.dutyCycleChannel0;
    FLEXPWM0->SM[0].VAL3 = w16DutyAr;
    FLEXPWM0->SM[1].VAL2 = phaseCurrents.dutyCycleChannel2;
    FLEXPWM0->SM[1].VAL3 = w16DutyBr;
    FLEXPWM0->SM[2].VAL2 = phaseCurrents.dutyCycleChannel4;
    FLEXPWM0->SM[2].VAL3 = w16DutyCr;
    
    // setup sampling time points
    Current_samplingSetup(&phaseCurrents, &t1_t2,  psFocPMSM->ui16SectorSVM);// samp_scaled[0~2] is updated
    mcTrigPointCompressor.w16TrigTimePoint1 = phaseCurrents.samp_scaled[0];
    mcTrigPointCompressor.w16TrigTimePoint2 = phaseCurrents.samp_scaled[1];
    mcTrigPointCompressor.w16TrigTimePoint3 = phaseCurrents.samp_scaled[2];
    FLEXPWM0->SM[0].VAL4 = mcTrigPointCompressor.w16TrigTimePoint1; 
    FLEXPWM0->SM[0].VAL5 = mcTrigPointCompressor.w16TrigTimePoint2;
    FLEXPWM0->SM[1].VAL4 = mcTrigPointCompressor.w16TrigTimePoint3;
    
    FLEXPWM0->MCTRL |= PWM_MCTRL_LDOK(7);
}

/*!
 * @brief PMSM field oriented speed control.
 *
 * This function is used to compute PMSM field oriented speed control.
 *
 * @param psSpeed       The pointer of the PMSM speed structure
 *
 * @return None
 */
void MCS_PMSMFocCtrlSpeed(mcs_speed_t *psSpeed)
{
    /* Speed saturation flag given by the Q current controller saturation flag and speed controller saturation flag */
    psSpeed->bSpeedPiStopInteg = (bool_t)((psSpeed->sSpeedPiParams.bLimFlag | psSpeed->bIqPiLimFlag) &
    		(bool_t)(MLIB_Abs_FLT(psSpeed->fltSpeedCmd) >= MLIB_Abs_FLT(psSpeed->fltSpeedFilt)));

    /* Speed ramp generation */
    psSpeed->fltSpeedRamp = GFLIB_Ramp_FLT(psSpeed->fltSpeedCmd, &psSpeed->sSpeedRampParams);

    /* Speed error calculation */
    psSpeed->fltSpeedError = MLIB_Sub_FLT(psSpeed->fltSpeedRamp, psSpeed->fltSpeedFilt);

    /* Desired current by the speed PI controller */
    psSpeed->fltIqReq =
        GFLIB_CtrlPIpAW_FLT(psSpeed->fltSpeedError, &psSpeed->bSpeedPiStopInteg, &psSpeed->sSpeedPiParams);
}

/*!
 * @brief PMSM rotor alignment - 0deg 
 *
 * This function is used for alignment rotor - 0deg 
 *
 * @param psAlignment   The pointer of the motor control alignment structure
 *
 * @return None
 */
void MCS_PMSMAlignment(mcs_alignment_t *psAlignment)
{
    psAlignment->f16PosAlign = FRAC16(0.0);
}

/*!
 * @brief PMSM Open Loop Start-up
 *
 * This function is used to PMSM Open Loop Start-up
 *
 * @param psStartUp     The pointer of the PMSM open loop start up parameters structure
 *
 * @return None
 */
void MCS_PMSMOpenLoopStartUp(mcs_pmsm_startup_t *psStartUp)
{
    /* Open loop startup speed ramp */
    psStartUp->fltSpeedRampOpenLoop = GFLIB_Ramp_FLT(psStartUp->fltSpeedReq, &psStartUp->sSpeedRampOpenLoopParams);

    /* generation of open loop position from the required speed */
    psStartUp->f16PosGen = GFLIB_Integrator_F16(
        MLIB_ConvSc_F16ff(psStartUp->fltSpeedRampOpenLoop, psStartUp->fltSpeedMax), &psStartUp->sSpeedIntegrator);

    /* position merging starts above merging speed threshold*/
    if (MLIB_Abs_FLT(psStartUp->fltSpeedRampOpenLoop) >= psStartUp->fltSpeedCatchUp)
    {
        /* increment position merging coefficient */
        psStartUp->f16RatioMerging = MLIB_AddSat_F16(psStartUp->f16RatioMerging, psStartUp->f16CoeffMerging);

        /* merging equation */
        psStartUp->f16PosMerged = MLIB_Add_F16(
            psStartUp->f16PosGen,
            MLIB_Mul_F16(MLIB_Sub_F16(psStartUp->f16PosEst, psStartUp->f16PosGen), psStartUp->f16RatioMerging));
    }
    else
    {
        psStartUp->f16PosMerged = psStartUp->f16PosGen;
    }

    /* clear open loop flag */
    if (psStartUp->f16RatioMerging == FRAC16(1.0))
    {
        psStartUp->bOpenLoop = FALSE;
    }
}

/*!
 * @brief PMSM scalar control, voltage is set based on required speed
 *
 * This function is used for alignment rotor in two steps - 120deg in first step and 0deg in second
 *
 * @param psScalarPMSM   The pointer of the PMSM scalar control structure
 *
 * @return None
 */
void MCS_PMSMScalarCtrl(mcs_pmsm_scalar_ctrl_t *psScalarPMSM)
{
    /* this part of code is executed when scalar control is turned-on */
    /* frequency ramp */
    psScalarPMSM->fltFreqRamp = GFLIB_Ramp_FLT(psScalarPMSM->fltFreqCmd, &psScalarPMSM->sFreqRampParams);

    /* voltage calculation */
    psScalarPMSM->sUDQReq.fltQ = psScalarPMSM->fltVHzGain * psScalarPMSM->fltFreqRamp;
    psScalarPMSM->sUDQReq.fltD = 0.0F;

    /* voltage limitation to f16UqMin */
    if (psScalarPMSM->sUDQReq.fltQ >= 0.0F)
    {
        psScalarPMSM->sUDQReq.fltQ = GFLIB_LowerLimit_FLT(psScalarPMSM->sUDQReq.fltQ,
                                                                     psScalarPMSM->fltUqMin);
    }
    else
    {
        psScalarPMSM->sUDQReq.fltQ = GFLIB_UpperLimit_FLT(psScalarPMSM->sUDQReq.fltQ,
                                                                     MLIB_Neg_FLT(psScalarPMSM->fltUqMin));
    }

    /* stator voltage angle , used the same integrator as for the open-loop start up*/
    psScalarPMSM->f16PosElScalar = GFLIB_Integrator_F16(
        MLIB_ConvSc_F16ff(psScalarPMSM->fltFreqRamp, psScalarPMSM->fltFreqMax), &psScalarPMSM->sFreqIntegrator);
}