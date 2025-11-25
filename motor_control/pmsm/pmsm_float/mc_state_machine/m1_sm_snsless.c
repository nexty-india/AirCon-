/*
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "m1_sm_snsless.h"
#include "mc_periph_init.h"
#include "current_reconstruction.h"
#include "TLOnlinePITuning.h"
#include "HarmonicCompensation.h"
#include "Flux_observer.h"
#include "MC_Parameters.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
HARMONIC_COMPENSATION g_sM1IdComp;
FLUX_OBSRV_T sFluxObsrv;
MECH_POSITION_CALC_T sMecPositionObser;
TLONLINE_T sTLObser;
GDFLIB_FILTER_IIR1_T_F32        sFluxSpdFilter;
frac32_t Coeff;
uint16_t u16StartupTime;
#define MergeDuration 800//fast loop cycles
bool_t bStartupFinish;

#define M1_SVM_SECTOR_DEFAULT (2)        /* default SVM sector */
#define M1_BLOCK_ROT_FAULT_SH (0.03125F) /* filter window */
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void M1_StateFaultFast(void);
static void M1_StateInitFast(void);
static void M1_StateStopFast(void);
static void M1_StateRunFast(void);

static void M1_StateFaultSlow(void);
static void M1_StateInitSlow(void);
static void M1_StateStopSlow(void);
static void M1_StateRunSlow(void);

static void M1_TransFaultStop(void);
static void M1_TransInitFault(void);
static void M1_TransInitStop(void);
static void M1_TransStopFault(void);
static void M1_TransStopRun(void);
static void M1_TransRunFault(void);
static void M1_TransRunStop(void);

static void M1_StateRunCalibFast(void);
static void M1_StateRunReadyFast(void);
static void M1_StateRunAlignFast(void);
static void M1_StateRunStartupFast(void);
static void M1_StateRunSpinFast(void);
static void M1_StateRunFreewheelFast(void);

static void M1_StateRunCalibSlow(void);
static void M1_StateRunReadySlow(void);
static void M1_StateRunAlignSlow(void);
static void M1_StateRunStartupSlow(void);
static void M1_StateRunSpinSlow(void);
static void M1_StateRunFreewheelSlow(void);

static void M1_TransRunCalibReady(void);
static void M1_TransRunReadyAlign(void);
static void M1_TransRunAlignStartup(void);
static void M1_TransRunAlignReady(void);
static void M1_TransRunStartupSpin(void);
static void M1_TransRunStartupFreewheel(void);
static void M1_TransRunSpinFreewheel(void);
static void M1_TransRunFreewheelReady(void);

static void M1_ClearFOCVariables(void);

static void M1_FaultDetection(void);
/*******************************************************************************
 * Variables
 ******************************************************************************/
extern PFCDEF_DRIVE_T  gsPFC_Drive;
/*! @brief Main control structure */
mcdef_pmsm_t g_sM1Drive;

/*! @brief Main application switch */
bool_t g_bM1SwitchAppOnOff;

/*! @brief M1 structure */
run_substate_t g_eM1StateRun;

/*! @brief FreeMASTER scales */
/*! DO NOT USE THEM in the code to avoid float library include */
volatile float g_fltM1voltageScale;
volatile float g_fltM1DCBvoltageScale;
volatile float g_fltM1currentScale;
volatile float g_fltM1speedScale;
volatile float g_fltM1speedAngularScale;
volatile float g_fltM1speedMechanicalScale;
volatile float g_fltM1TorqueScale;

/*! @brief Application state machine table - fast */
const sm_app_state_fcn_t s_M1_STATE_FAST = {M1_StateFaultFast, M1_StateInitFast, M1_StateStopFast, M1_StateRunFast};

/*! @brief Application state machine table - slow */
const sm_app_state_fcn_t s_M1_STATE_SLOW = {M1_StateFaultSlow, M1_StateInitSlow, M1_StateStopSlow, M1_StateRunSlow};

/*! @brief Application sub-state function field - fast */
static const pfc_app_state_fcn s_M1_STATE_RUN_TABLE_FAST[6] = {
    M1_StateRunCalibFast, M1_StateRunReadyFast,     M1_StateRunAlignFast,  M1_StateRunStartupFast,
    M1_StateRunSpinFast,  M1_StateRunFreewheelFast};

/*! @brief Application sub-state function field - slow */
static const pfc_app_state_fcn s_M1_STATE_RUN_TABLE_SLOW[6] = {
    M1_StateRunCalibSlow, M1_StateRunReadySlow,     M1_StateRunAlignSlow,  M1_StateRunStartupSlow,
    M1_StateRunSpinSlow,  M1_StateRunFreewheelSlow};

/*! @brief Application state-transition functions field  */
static const sm_app_trans_fcn_t s_TRANS = {M1_TransFaultStop, M1_TransInitFault, M1_TransInitStop, M1_TransStopFault,
                                           M1_TransStopRun,   M1_TransRunFault,  M1_TransRunStop};

/*! @brief  State machine structure declaration and initialization */
sm_app_ctrl_t g_sM1Ctrl = {
    /* g_sM1Ctrl.psState, User state functions  */
    &s_M1_STATE_FAST,

    /* g_sM1Ctrl.psState, User state functions  */
    &s_M1_STATE_SLOW,

    /* g_sM1Ctrl..psTrans, User state-transition functions */
    &s_TRANS,

    /* g_sM1Ctrl.uiCtrl, Default no control command */
    SM_CTRL_NONE,

    /* g_sM1Ctrl.eState, Default state after reset */
    kSM_AppInit};
/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Fault state called in fast state machine
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_StateFaultFast(void)
{
    /* read ADC results (ADC triggered by HW trigger from PDB) */
    /* get all adc samples - DC-bus voltage, current, bemf and aux sample */
    M1_MCDRV_ADC_GET(&g_sM1AdcSensor);
    //trigger for temperature sampling
//    ADC1->SWTRIG = 1 << 1;

    g_sM1Drive.sFocPMSM.sIABC.fltA = MLIB_ConvSc_FLTsf(g_sM1AdcSensor.psIABC->f16A, g_fltM1currentScale);
    g_sM1Drive.sFocPMSM.sIABC.fltB = MLIB_ConvSc_FLTsf(g_sM1AdcSensor.psIABC->f16B, g_fltM1currentScale);
    g_sM1Drive.sFocPMSM.sIABC.fltC = MLIB_ConvSc_FLTsf(g_sM1AdcSensor.psIABC->f16C, g_fltM1currentScale);
    
    /* convert voltages from fractional measured values to float */
    g_sM1Drive.sFocPMSM.fltUDcBus = MLIB_ConvSc_FLTsf(g_sM1Drive.sFocPMSM.f16UDcBus, g_fltM1DCBvoltageScale);

    /* Sampled DC-Bus voltage filter */
    g_sM1Drive.sFocPMSM.fltUDcBusFilt =
        GDFLIB_FilterIIR1_FLT(g_sM1Drive.sFocPMSM.fltUDcBus, &g_sM1Drive.sFocPMSM.sUDcBusFilter);

    /* Braking resistor control */
    if (g_sM1Drive.sFocPMSM.fltUDcBusFilt > g_sM1Drive.sFaultThresholds.fltUDcBusTrip)
    {
    	M1_BRAKE_SET();
    }
    else
    {
    	M1_BRAKE_CLEAR();
    }

    /* Disable user application switch */
    g_bM1SwitchAppOnOff = FALSE;
    
//    MCDRV_TemperatureGet(&g_sM1AdcSensor);

    /* PWM peripheral update */
    M1_MCDRV_PWM3PH_SET(&g_sM1Pwm3ph);

    /* Detects faults */
    M1_FaultDetection();
}

/*!
 * @brief State initialization routine called in fast state machine
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_StateInitFast(void)
{
    /* Type the code to do when in the INIT state */
    g_sM1Drive.sFocPMSM.sIdPiParams.fltInErrK_1 = 0.0F;
    g_sM1Drive.sFocPMSM.sIdPiParams.bLimFlag    = FALSE;

    g_sM1Drive.sFocPMSM.sIqPiParams.fltInErrK_1 = 0.0F;
    g_sM1Drive.sFocPMSM.sIqPiParams.bLimFlag    = FALSE;

    /* PMSM FOC params */
    g_sM1Drive.sFocPMSM.sIdPiParams.fltPGain    = M1_D_KP_GAIN;
    g_sM1Drive.sFocPMSM.sIdPiParams.fltIGain    = M1_D_KI_GAIN;
    g_sM1Drive.sFocPMSM.sIdPiParams.fltUpperLim = M1_U_MAX;
    g_sM1Drive.sFocPMSM.sIdPiParams.fltLowerLim = -M1_U_MAX;

    g_sM1Drive.sFocPMSM.sIqPiParams.fltPGain    = M1_Q_KP_GAIN;
    g_sM1Drive.sFocPMSM.sIqPiParams.fltIGain    = M1_Q_KI_GAIN;
    g_sM1Drive.sFocPMSM.sIqPiParams.fltUpperLim = M1_U_MAX;
    g_sM1Drive.sFocPMSM.sIqPiParams.fltLowerLim = -M1_U_MAX;

    g_sM1Drive.sFocPMSM.ui16SectorSVM     = M1_SVM_SECTOR_DEFAULT;
    g_sM1Drive.sFocPMSM.fltDutyCycleLimit = M1_CLOOP_LIMIT;

    g_sM1Drive.sFocPMSM.fltUDcBus                     = 0.0F;
    g_sM1Drive.sFocPMSM.fltUDcBusFilt                 = 0.0F;
    g_sM1Drive.sFocPMSM.sUDcBusFilter.sFltCoeff.fltB0 = M1_UDCB_IIR_B0;
    g_sM1Drive.sFocPMSM.sUDcBusFilter.sFltCoeff.fltB1 = M1_UDCB_IIR_B1;
    g_sM1Drive.sFocPMSM.sUDcBusFilter.sFltCoeff.fltA1 = M1_UDCB_IIR_A1;
    /* Filter init not to enter to fault */
    g_sM1Drive.sFocPMSM.sUDcBusFilter.fltFltBfrX[0] = (M1_U_DCB_UNDERVOLTAGE / 2.0F) + (M1_U_DCB_OVERVOLTAGE / 2.0F);
    g_sM1Drive.sFocPMSM.sUDcBusFilter.fltFltBfrY[0] = (M1_U_DCB_UNDERVOLTAGE / 2.0F) + (M1_U_DCB_OVERVOLTAGE / 2.0F);

    g_sM1Drive.sAlignment.fltUdReq = M1_ALIGN_VOLTAGE;
    g_sM1Drive.sAlignment.ui16Time = M1_ALIGN_DURATION;

    /* Position and speed observer */
    g_sM1Drive.sFocPMSM.sSpeedElEstFilt.sFltCoeff.fltB0 = M1_TO_SPEED_IIR_B0;
    g_sM1Drive.sFocPMSM.sSpeedElEstFilt.sFltCoeff.fltB1 = M1_TO_SPEED_IIR_B1;
    g_sM1Drive.sFocPMSM.sSpeedElEstFilt.sFltCoeff.fltA1 = M1_TO_SPEED_IIR_A1;
    GDFLIB_FilterIIR1Init_FLT(&g_sM1Drive.sFocPMSM.sSpeedElEstFilt);

    /* Speed params */
    g_sM1Drive.sSpeed.sSpeedPiParams.fltPGain    = M1_SPEED_PI_PROP_GAIN;
    g_sM1Drive.sSpeed.sSpeedPiParams.fltIGain    = M1_SPEED_PI_INTEG_GAIN;
    g_sM1Drive.sSpeed.sSpeedPiParams.fltUpperLim = M1_SPEED_LOOP_HIGH_LIMIT;
    g_sM1Drive.sSpeed.sSpeedPiParams.fltLowerLim = M1_SPEED_LOOP_LOW_LIMIT;

    g_sM1Drive.sSpeed.sSpeedRampParams.fltRampUp   = M1_SPEED_RAMP_UP;
    g_sM1Drive.sSpeed.sSpeedRampParams.fltRampDown = M1_SPEED_RAMP_DOWN;

    g_sM1Drive.sSpeed.sSpeedFilter.sFltCoeff.fltB0 = M1_SPEED_IIR_B0;
    g_sM1Drive.sSpeed.sSpeedFilter.sFltCoeff.fltB1 = M1_SPEED_IIR_B1;
    g_sM1Drive.sSpeed.sSpeedFilter.sFltCoeff.fltA1 = M1_SPEED_IIR_A1;

    g_sM1Drive.sSpeed.fltSpeedCmd = 0.0F;

    /* Scalar control params */
    g_sM1Drive.sScalarCtrl.fltVHzGain                  = M1_SCALAR_VHZ_FACTOR_GAIN;
    g_sM1Drive.sScalarCtrl.fltUqMin                    = M1_SCALAR_UQ_MIN;
    g_sM1Drive.sScalarCtrl.sFreqRampParams.fltRampUp   = M1_SCALAR_RAMP_UP;
    g_sM1Drive.sScalarCtrl.sFreqRampParams.fltRampDown = M1_SCALAR_RAMP_DOWN;
    g_sM1Drive.sScalarCtrl.sFreqIntegrator.a32Gain     = M1_SCALAR_INTEG_GAIN;
    g_sM1Drive.sScalarCtrl.fltFreqMax                  = M1_FREQ_MAX;

    /* Open loop start up */
    g_sM1Drive.sStartUp.sSpeedIntegrator.a32Gain             = M1_SCALAR_INTEG_GAIN;
    g_sM1Drive.sStartUp.f16CoeffMerging                      = M1_MERG_COEFF;
    g_sM1Drive.sStartUp.fltSpeedCatchUp                      = M1_MERG_SPEED_TRH;
    g_sM1Drive.sStartUp.fltCurrentStartup                    = M1_OL_START_I;
    g_sM1Drive.sStartUp.sSpeedRampOpenLoopParams.fltRampUp   = M1_OL_START_RAMP_INC;
    g_sM1Drive.sStartUp.sSpeedRampOpenLoopParams.fltRampDown = M1_OL_START_RAMP_INC;
    g_sM1Drive.sStartUp.fltSpeedMax                          = M1_N_MAX;
    g_sM1Drive.sStartUp.bOpenLoop                            = TRUE;

    /* MCAT cascade control variables */
    g_sM1Drive.sMCATctrl.sIDQReqMCAT.fltD = 0.0F;
    g_sM1Drive.sMCATctrl.sIDQReqMCAT.fltQ = 0.0F;
    g_sM1Drive.sMCATctrl.sUDQReqMCAT.fltD = 0.0F;
    g_sM1Drive.sMCATctrl.sUDQReqMCAT.fltQ = 0.0F;
    g_sM1Drive.sMCATctrl.ui16PospeSensor  = MCAT_SENSORLESS_CTRL;

    /* Timing control and general variables */
    g_sM1Drive.ui16CounterState           = 0U;
    g_sM1Drive.ui16TimeFullSpeedFreeWheel = M1_FREEWHEEL_DURATION;
    g_sM1Drive.ui16TimeCalibration        = M1_CALIB_DURATION;
    g_sM1Drive.ui16TimeFaultRelease       = M1_FAULT_DURATION;
    g_bM1SwitchAppOnOff                   = FALSE;
    /* Default MCAT control mode after reset */
    g_sM1Drive.eControl = kControlMode_SpeedFOC;

    /* Field weakening params */
    g_sM1Drive.sFocFw.sFwErrorFilter.sFltCoeff.fltB0 = M1_FILTER_FW_B0;
    g_sM1Drive.sFocFw.sFwErrorFilter.sFltCoeff.fltB1 = M1_FILTER_FW_B1;
    g_sM1Drive.sFocFw.sFwErrorFilter.sFltCoeff.fltA1 = M1_FILTER_FW_A1;
    GDFLIB_FilterIIR1Init_FLT(&g_sM1Drive.sFocFw.sFwErrorFilter);

    g_sM1Drive.sFocFw.sFwPiParams.fltPGain = M1_PI_FW_P_GAIN;
    g_sM1Drive.sFocFw.sFwPiParams.fltIGain = M1_PI_FW_I_GAIN;
    g_sM1Drive.sFocFw.sFwPiParams.fltUpperLim = 0;
    g_sM1Drive.sFocFw.sFwPiParams.fltLowerLim = - M1_PI_FW_OUTPUT_LIMIT;

    g_sM1Drive.sFocFw.fltUFwError = 0.0;
    g_sM1Drive.sFocFw.fltIFwError = 0.0;
    g_sM1Drive.sFocFw.fltFwError = 0.0;
    g_sM1Drive.sFocFw.fltFwErrorFilt = 0.0;
    
    g_sM1Drive.sFocFw.bFwPiSatFlag = 0;
    g_sM1Drive.sFocFw.fltSpeedFwOn = M1_SPEED_FW_ON / M1_N_ANGULAR_MAX;
    
    /* fault set to init states */
    FAULT_CLEAR_ALL(g_sM1Drive.sFaultIdCaptured);
    FAULT_CLEAR_ALL(g_sM1Drive.sFaultIdPending);

    /* fault thresholds */
    g_sM1Drive.sFaultThresholds.fltUDcBusOver     = M1_U_DCB_OVERVOLTAGE;
    g_sM1Drive.sFaultThresholds.fltUDcBusUnder    = M1_U_DCB_UNDERVOLTAGE;
    g_sM1Drive.sFaultThresholds.fltUDcBusTrip     = M1_U_DCB_TRIP;

    g_sM1Drive.sFaultThresholds.fltSpeedOver      = M1_N_OVERSPEED;
    g_sM1Drive.sFaultThresholds.fltSpeedMin       = M1_N_MIN;
    g_sM1Drive.sFaultThresholds.fltSpeedNom       = M1_N_NOM;

    /* fault blocked rotor filter */
    g_sM1Drive.msM1BlockedRotorUqFilt.fltLambda = M1_BLOCK_ROT_FAULT_SH;

    /* Defined scaling for FreeMASTER */
    g_fltM1voltageScale         = M1_U_MAX;
    g_fltM1currentScale         = M1_I_MAX;
    g_fltM1DCBvoltageScale      = M1_U_DCB_MAX;
    g_fltM1speedScale           = M1_N_MAX;
    g_fltM1speedAngularScale    = M1_N_ANGULAR_MAX;
    g_fltM1speedMechanicalScale = (60.0F / (2.0F * FLOAT_PI));
    g_fltM1TorqueScale         = TORQUE_MAX;

    /* Application timing */
    g_sM1Drive.ui16FastCtrlLoopFreq = (g_sClockSetup.ui16M1PwmFreq / ((uint16_t)M1_FOC_FREQ_VS_PWM_FREQ));
    g_sM1Drive.ui16SlowCtrlLoopFreq = g_sClockSetup.ui16M1SpeedLoopFreq;
    g_sM1Drive.ui32CpuFrequency = g_sClockSetup.ui32CpuFrequency;

    /* Clear rest of variables  */
    M1_ClearFOCVariables();
    
    Current_reconstruction_init(&phaseCurrents);
    TLOnlineTuningInit(&sTLObser);
    MechAngleCalcInit(&sMecPositionObser);
    TLOnlineTuningInitRep(&sTLObser);
    
    /* Harmonic compensation init */
    g_sM1IdComp.Rep_Zk = M1_HC_ZK;
    g_sM1IdComp.RepKr = M1_HC_GAIN;
    g_sM1IdComp.ReqQz = M1_HC_QZ;
    g_sM1IdComp.fltOutLimit = M1_HC_OUT_LIMIT;
    g_sM1IdComp.fltStoreLimit = M1_HC_STORE_LIMIT;
    g_sM1IdComp.fltFiltProp = M1_HC_FILT_PROP;

    g_sM1IdComp.f16SpeedCoeff = FRAC16(M1_HC_SPEED_COEFF*M1_N_MAX*M1_N_ANGULAR_MAX/60/M1_PWM_FREQ*M1_HC_NUM_COMP_TABLE/32768);
    g_sM1IdComp.f16Num_Table = M1_HC_NUM_COMP_TABLE;
    g_sM1IdComp.f16Shift_Table = M1_HC_SHIFT_TABLE;

    //Flux observer
    Init_FLUX_OBSRV_T(&sFluxObsrv);
    sFluxObsrv.Init(&sFluxObsrv);

    sFluxObsrv.acTo.f16PGain = M1_FLUX_TO_KP_GAIN;
    sFluxObsrv.acTo.i16PGainSh = M1_FLUX_TO_KP_SHIFT;
    sFluxObsrv.acTo.f16IGain = M1_FLUX_TO_KI_GAIN;
    sFluxObsrv.acTo.i16IGainSh = M1_FLUX_TO_KI_SHIFT;
    sFluxObsrv.acTo.f16ThGain = M1_FLUX_TO_THETA_GAIN;
    sFluxObsrv.acTo.i16ThGainSh = M1_FLUX_TO_THETA_SHIFT;
    
    sFluxObsrv.sCoeff.f32U_Scale = M1_FLUX_U_SCALE;
    sFluxObsrv.sCoeff.f32R_Scale = M1_FLUX_R_SCALE;
    sFluxObsrv.sCoeff.f32Lq_Scale = M1_FLUX_LQ_SCALE;
    sFluxObsrv.sCoeff.f32K_Alpha_Coeff = M1_FLUX_KALPHA_COEFF;
    sFluxObsrv.sCoeff.f32K_Beta_Coeff = M1_FLUX_KBETA_COEFF;
    Coeff = M1_FLUX_KALPHA_COEFF;
    sFluxObsrv.sCoeff.f16I_Threshold = M1_I_THRESHOLD_FLUX_OBSRV_F;
    sFluxObsrv.sCoeff.i32I_Threshold = M1_I_THRESHOLD_FLUX_OBSRV;
    sFluxObsrv.sCoeff.i32Ld_Neg_Scale = M1_FLUX_LD_NEG_SCALE;
    sFluxObsrv.sCoeff.i32Lq_Neg_Scale = M1_FLUX_LQ_NEG_SCALE;
    sFluxObsrv.sCoeff.i32Rotor_Ld_Coeff = M1_FLUX_ROTOR_LD_COEFF;
    
    sFluxSpdFilter.sFltCoeff.f32B0 = FRAC32(M1_TO_SPEED_IIR_B0 / 2.0);
    sFluxSpdFilter.sFltCoeff.f32B1 = FRAC32(M1_TO_SPEED_IIR_B1 / 2.0);
    sFluxSpdFilter.sFltCoeff.f32A1 = FRAC32(M1_TO_SPEED_IIR_A1 / 2.0);
    GDFLIB_FilterIIR1Init_F16(&sFluxSpdFilter);
    bStartupFinish = 0;
    
    /* Init sensors/actuators pointers */
    /* For PWM driver */
    g_sM1Pwm3ph.psUABC = &(g_sM1Drive.sFocPMSM.sDutyABC);
    /* For ADC driver */
    g_sM1AdcSensor.pf16UDcBus     = &(g_sM1Drive.sFocPMSM.f16UDcBus);
    g_sM1AdcSensor.psIABC         = &(g_sM1Drive.sFocPMSM.sIABCFrac);
    g_sM1AdcSensor.pui16SVMSector = &(g_sM1Drive.sFocPMSM.ui16SectorSVM);
    g_sM1AdcSensor.pui16AuxChan   = &(g_sM1Drive.f16AdcAuxSample);
    
    /* ADC base address */
    g_sM1AdcSensor.pToAdcBase = (ADC_Type *)ADC0;

    /* INIT_DONE command */
    g_sM1Ctrl.uiCtrl |= SM_CTRL_INIT_DONE;
}

/*!
 * @brief Stop state routine called in fast state machine
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_StateStopFast(void)
{
    /* read ADC results (ADC triggered by HW trigger from PDB) */
    /* get all adc samples - DC-bus voltage, current, bemf and aux sample */
    M1_MCDRV_ADC_GET(&g_sM1AdcSensor);
//    ADC1->SWTRIG = 1 << 1;
    
    g_sM1Drive.sFocPMSM.sIABC.fltA = MLIB_ConvSc_FLTsf(g_sM1AdcSensor.psIABC->f16A, g_fltM1currentScale);
    g_sM1Drive.sFocPMSM.sIABC.fltB = MLIB_ConvSc_FLTsf(g_sM1AdcSensor.psIABC->f16B, g_fltM1currentScale);
    g_sM1Drive.sFocPMSM.sIABC.fltC = MLIB_ConvSc_FLTsf(g_sM1AdcSensor.psIABC->f16C, g_fltM1currentScale);
    
    /* convert voltages from fractional measured values to float */
    g_sM1Drive.sFocPMSM.fltUDcBus = MLIB_ConvSc_FLTsf(g_sM1Drive.sFocPMSM.f16UDcBus, g_fltM1DCBvoltageScale);

    /* Sampled DC-Bus voltage filter */
    g_sM1Drive.sFocPMSM.fltUDcBusFilt =
        GDFLIB_FilterIIR1_FLT(g_sM1Drive.sFocPMSM.fltUDcBus, &g_sM1Drive.sFocPMSM.sUDcBusFilter);

    /* If the user switches on and set non-zero speed*/
    if ((g_bM1SwitchAppOnOff != FALSE) || (g_sM1Drive.sSpeed.fltSpeedCmd != 0.0F))
    {
        /* Set the switch on */
        g_bM1SwitchAppOnOff = TRUE;

        /* Start command */
        g_sM1Ctrl.uiCtrl |= SM_CTRL_START;
    }
    /* If switch is on but speed is zero, set a default speed and start */
    	else if ((g_bM1SwitchAppOnOff != FALSE) && (g_sM1Drive.sSpeed.fltSpeedCmd == 0.0F))
    	{
    		/* Set a default speed command */
    		g_sM1Drive.sSpeed.fltSpeedCmd = 500.0F;  // Set your desired default speed
    		/* Set the switch on */
    		g_bM1SwitchAppOnOff = TRUE;

    		/* Start command */
    		g_sM1Ctrl.uiCtrl |= SM_CTRL_START;
    	}


    /* Braking resistor control */
    if (g_sM1Drive.sFocPMSM.fltUDcBusFilt > g_sM1Drive.sFaultThresholds.fltUDcBusTrip)
    {
    	M1_BRAKE_SET();
    }
    else
    {
    	M1_BRAKE_CLEAR();
    }

    M1_FaultDetection();

    /* If a fault occurred */
    if ((bool_t)g_sM1Drive.sFaultIdPending)
    {
        /* Switches to the FAULT state */
        g_sM1Ctrl.uiCtrl |= SM_CTRL_FAULT;
    }

//    MCDRV_TemperatureGet(&g_sM1AdcSensor);
    
    /* PWM peripheral update */
    M1_MCDRV_PWM3PH_SET(&g_sM1Pwm3ph);
}

/*!
 * @brief Run state routine called in fast state machine
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_StateRunFast(void)
{
    /* get all adc samples - DC-bus voltage, current, bemf and aux sample */
    M1_MCDRV_ADC_GET(&g_sM1AdcSensor);
//    ADC1->SWTRIG = 1 << 1;

    g_sM1Drive.sFocPMSM.sIABC.fltA = MLIB_ConvSc_FLTsf(g_sM1AdcSensor.psIABC->f16A, g_fltM1currentScale);
    g_sM1Drive.sFocPMSM.sIABC.fltB = MLIB_ConvSc_FLTsf(g_sM1AdcSensor.psIABC->f16B, g_fltM1currentScale);
    g_sM1Drive.sFocPMSM.sIABC.fltC = MLIB_ConvSc_FLTsf(g_sM1AdcSensor.psIABC->f16C, g_fltM1currentScale);
    
    /* If the user switches off */
    if (!g_bM1SwitchAppOnOff)
    {
        /* Stop command */
        g_sM1Ctrl.uiCtrl |= SM_CTRL_STOP;
    }

    /* detect fault */
    M1_FaultDetection();

    /* If a fault occurred */
    if (g_sM1Drive.sFaultIdPending != 0U)
    {
        /* Switches to the FAULT state */
        g_sM1Ctrl.uiCtrl |= SM_CTRL_FAULT;
    }

    /* Convert phase currents from fractional measured values to float */
    g_sM1Drive.sFocPMSM.sIABC.fltA = MLIB_ConvSc_FLTsf(g_sM1Drive.sFocPMSM.sIABCFrac.f16A, g_fltM1currentScale);
    g_sM1Drive.sFocPMSM.sIABC.fltB = MLIB_ConvSc_FLTsf(g_sM1Drive.sFocPMSM.sIABCFrac.f16B, g_fltM1currentScale);
    g_sM1Drive.sFocPMSM.sIABC.fltC = MLIB_ConvSc_FLTsf(g_sM1Drive.sFocPMSM.sIABCFrac.f16C, g_fltM1currentScale);

    /* Convert voltages from fractional measured values to float */
    g_sM1Drive.sFocPMSM.fltUDcBus = MLIB_ConvSc_FLTsf(g_sM1Drive.sFocPMSM.f16UDcBus, g_fltM1DCBvoltageScale);

    /* Sampled DC-Bus voltage filter */
    g_sM1Drive.sFocPMSM.fltUDcBusFilt =
        GDFLIB_FilterIIR1_FLT(g_sM1Drive.sFocPMSM.fltUDcBus, &g_sM1Drive.sFocPMSM.sUDcBusFilter);

    /* Braking resistor control */
    if (g_sM1Drive.sFocPMSM.fltUDcBusFilt > g_sM1Drive.sFaultThresholds.fltUDcBusTrip)
    {
    	M1_BRAKE_SET();
    }
    else
    {
    	M1_BRAKE_CLEAR();
    }

    /* Run sub-state function */
    s_M1_STATE_RUN_TABLE_FAST[g_eM1StateRun]();

//    MCDRV_TemperatureGet(&g_sM1AdcSensor);
}

/*!
 * @brief Fault state routine called in slow state machine
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_StateFaultSlow(void)
{
    /* After fault condition ends wait defined time to clear fault state */
    if (!FAULT_ANY(g_sM1Drive.sFaultIdPending))
    {
        if (--g_sM1Drive.ui16CounterState == 0U)
        {
            /* Clear fault state */
            g_sM1Ctrl.uiCtrl |= SM_CTRL_FAULT_CLEAR;
        }
    }
    else
    {
        g_sM1Drive.ui16CounterState = g_sM1Drive.ui16TimeFaultRelease;
    }
}

/*!
 * @brief Fault state routine called in slow state machine
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_StateInitSlow(void)
{
}

/*!
 * @brief Stop state routine called in slow state machine
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_StateStopSlow(void)
{
}

/*!
 * @brief Run state routine called in slow state machine
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_StateRunSlow(void)
{
    /* Run sub-state function */
    s_M1_STATE_RUN_TABLE_SLOW[g_eM1StateRun]();
}

/*!
 * @brief Transition from Fault to Stop state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_TransFaultStop(void)
{
    /* Type the code to do when going from the FAULT to the INIT state */
    /* Clear all FOC variables, init filters, etc. */
    M1_ClearFOCVariables();
}

/*!
 * @brief Transition from Init to Fault state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_TransInitFault(void)
{
    /* Type the code to do when going from the INIT to the FAULT state */
    /* Disable PWM outputs */
    M1_MCDRV_PWM3PH_DIS(&g_sM1Pwm3ph);
    g_sM1Drive.ui16CounterState = g_sM1Drive.ui16TimeFaultRelease;

    g_sM1Drive.sSpeed.fltSpeedCmd = 0.0F;
}

/*!
 * @brief Transition from Init to Stop state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_TransInitStop(void)
{
    /* Type the code to do when going from the INIT to the STOP state */
    /* Disable PWM outputs */
    M1_MCDRV_PWM3PH_DIS(&g_sM1Pwm3ph);

    /* Enable Open loop start up */
    g_sM1Drive.sStartUp.bOpenLoop = TRUE;
}

/*!
 * @brief Transition from Stop to Fault state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_TransStopFault(void)
{
    /* Type the code to do when going from the STOP to the FAULT state */
    /* Load the fault release time to counter */
    g_sM1Drive.ui16CounterState = g_sM1Drive.ui16TimeFaultRelease;
}

/*!
 * @brief Transition from Stop to Run state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_TransStopRun(void)
{
    /* Type the code to do when going from the STOP to the RUN state */
    /* 50% duty cycle */
    g_sM1Drive.sFocPMSM.sDutyABC.f16A = FRAC16(0.5);
    g_sM1Drive.sFocPMSM.sDutyABC.f16B = FRAC16(0.5);
    g_sM1Drive.sFocPMSM.sDutyABC.f16C = FRAC16(0.5);

    /* PWM duty cycles calculation and update */
    M1_MCDRV_PWM3PH_SET(&g_sM1Pwm3ph);

    /* Clear offset filters */
    M1_MCDRV_CURR_3PH_CALIB_INIT(&g_sM1AdcSensor);

    /* Enable PWM output */
    M1_MCDRV_PWM3PH_EN(&g_sM1Pwm3ph);

    /* pass calibration routine duration to state counter*/
    g_sM1Drive.ui16CounterState = g_sM1Drive.ui16TimeCalibration;

    /* Calibration sub-state when transition to RUN */
    g_eM1StateRun = kRunState_Calib;

    /* Acknowledge that the system can proceed into the RUN state */
    g_sM1Ctrl.uiCtrl |= SM_CTRL_RUN_ACK;
}

/*!
 * @brief Transition from Run to Fault state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_TransRunFault(void)
{
    /* Type the code to do when going from the RUN to the FAULT state */
    /* Disable PWM output */
    M1_MCDRV_PWM3PH_DIS(&g_sM1Pwm3ph);
    g_sM1Drive.ui16CounterState = g_sM1Drive.ui16TimeFaultRelease;

    /* Clear over load flag */
    g_sM1Drive.sSpeed.bSpeedPiStopInteg = FALSE;

    g_sM1Drive.sSpeed.fltSpeedCmd         = 0.0F;
    g_sM1Drive.sScalarCtrl.fltFreqCmd     = 0.0F;
    g_sM1Drive.sScalarCtrl.sUDQReq.fltQ   = 0.0F;
    g_sM1Drive.sMCATctrl.sUDQReqMCAT.fltQ = 0.0F;
    g_sM1Drive.sMCATctrl.sUDQReqMCAT.fltD = 0.0F;
    g_sM1Drive.sMCATctrl.sIDQReqMCAT.fltQ = 0.0F;
    g_sM1Drive.sMCATctrl.sIDQReqMCAT.fltD = 0.0F;

    /* Clear actual speed values */
    g_sM1Drive.sScalarCtrl.fltFreqRamp = 0.0F;
    g_sM1Drive.sSpeed.fltSpeed         = 0.0F;
    g_sM1Drive.sSpeed.fltSpeedFilt     = 0.0F;
}

/*!
 * @brief Transition from Run to Stop state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_TransRunStop(void)
{
    /* Type the code to do when going from the RUN to the STOP state */
    /* Disable PWM outputs */
    M1_MCDRV_PWM3PH_DIS(&g_sM1Pwm3ph);

    g_sM1Drive.sSpeed.fltSpeedCmd         = 0.0F;
    g_sM1Drive.sScalarCtrl.fltFreqCmd     = 0.0F;
    g_sM1Drive.sScalarCtrl.sUDQReq.fltQ   = 0.0F;
    g_sM1Drive.sMCATctrl.sUDQReqMCAT.fltQ = 0.0F;
    g_sM1Drive.sMCATctrl.sUDQReqMCAT.fltD = 0.0F;
    g_sM1Drive.sMCATctrl.sIDQReqMCAT.fltQ = 0.0F;
    g_sM1Drive.sMCATctrl.sIDQReqMCAT.fltD = 0.0F;

    M1_ClearFOCVariables();

    /* Acknowledge that the system can proceed into the STOP state */
    g_sM1Ctrl.uiCtrl |= SM_CTRL_STOP_ACK;
}

/*!
 * @brief Calibration process called in fast state machine as Run sub state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_StateRunCalibFast(void)
{
    /* Type the code to do when in the RUN CALIB sub-state
       performing ADC offset calibration */

    /* Call offset measurement */
    M1_MCDRV_CURR_3PH_CALIB(&g_sM1AdcSensor);

    /* Change SVM sector in range <1;6> to measure all AD channel mapping combinations */
    if (++g_sM1Drive.sFocPMSM.ui16SectorSVM > 6U)
    {
    	g_sM1Drive.sFocPMSM.ui16SectorSVM = 1U;
    }
}

/*!
 * @brief Ready state called in fast state machine as Run sub state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_StateRunReadyFast(void)
{
    /* Type the code to do when in the RUN READY sub-state */
    /* Clear actual speed values */
    g_sM1Drive.sScalarCtrl.fltFreqRamp = 0.0F;
    g_sM1Drive.sSpeed.fltSpeed         = 0.0F;
    g_sM1Drive.sSpeed.fltSpeedFilt     = 0.0F;
    g_sM1Drive.sFocPMSM.f16PosElEst    = FRAC16(0.0);
    g_sM1Drive.sFocPMSM.fltSpeedElEst  = 0.0F;

    /* MCAT control structure switch */
    switch (g_sM1Drive.eControl)
    {
        case kControlMode_Scalar:
            if (!(g_sM1Drive.sScalarCtrl.fltFreqCmd == 0.0F))
            {
                g_sM1Drive.sScalarCtrl.fltFreqRamp  = 0.0F;
                g_sM1Drive.sScalarCtrl.sUDQReq.fltQ = 0.0F;

                /* Transition to the RUN ALIGN sub-state */
                M1_TransRunReadyAlign();
            }
            break;

        case kControlMode_VoltageFOC:
            if (!(g_sM1Drive.sMCATctrl.sUDQReqMCAT.fltQ == 0.0F))
            {
                if (g_sM1Drive.sMCATctrl.sUDQReqMCAT.fltQ > 0.0F)
                {
                  g_sM1Drive.sSpeed.fltSpeedCmd = g_sM1Drive.sStartUp.fltSpeedCatchUp * 2.0F;
                }
                else
                {
                  g_sM1Drive.sSpeed.fltSpeedCmd = MLIB_Neg_FLT(g_sM1Drive.sStartUp.fltSpeedCatchUp * 2.0F);
                }

                /* Transition to the RUN ALIGN sub-state */
                M1_TransRunReadyAlign();
            }
            break;

        case kControlMode_CurrentFOC:
            if (!(g_sM1Drive.sMCATctrl.sIDQReqMCAT.fltQ == 0.0F))
            {
                if (g_sM1Drive.sMCATctrl.sIDQReqMCAT.fltQ > 0.0F)
                {
                	g_sM1Drive.sSpeed.fltSpeedCmd = g_sM1Drive.sStartUp.fltSpeedCatchUp * 2.0F;
                }
                else
                {
                	g_sM1Drive.sSpeed.fltSpeedCmd = MLIB_Neg_FLT(g_sM1Drive.sStartUp.fltSpeedCatchUp * 2.0F);
                }

                /* Transition to the RUN ALIGN sub-state */
                M1_TransRunReadyAlign();
            }
            break;

        default:
            /* Align function */
            /* Value of fltSpeedNom is increased because of float conversion */
            if (((MLIB_Abs_FLT(g_sM1Drive.sSpeed.fltSpeedCmd) > g_sM1Drive.sFaultThresholds.fltSpeedMin) &&
                 (MLIB_Abs_FLT(g_sM1Drive.sSpeed.fltSpeedCmd) <= (g_sM1Drive.sFaultThresholds.fltSpeedNom + 0.1F))))
            {
                /* Transition to the RUN ALIGN sub-state */
                M1_TransRunReadyAlign();
            }
            else
            {
                g_sM1Drive.sSpeed.fltSpeedCmd = 0.0F;
            }
            break;
    }
}

/*!
 * @brief Alignment process called in fast state machine as Run sub state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_StateRunAlignFast(void)
{
    /* Type the code to do when in the RUN ALIGN sub-state */
    /* When alignment elapsed go to Startup */
    if (--g_sM1Drive.ui16CounterState == 0U)
    {
        /* Transition to the RUN kRunState_Startup sub-state */
        M1_TransRunAlignStartup();
    }

    /* If zero speed command go back to Ready */
    if ((g_sM1Drive.sMCATctrl.ui16PospeSensor == MCAT_SENSORLESS_CTRL) && (g_sM1Drive.sSpeed.fltSpeedCmd == 0.0F) &&
        (g_sM1Drive.sScalarCtrl.fltFreqCmd == 0.0F))
    {
    	M1_TransRunAlignReady();
    }

    /* Clear actual speed values */
    g_sM1Drive.sScalarCtrl.fltFreqRamp = 0.0F;
    g_sM1Drive.sSpeed.fltSpeed         = 0.0F;
    g_sM1Drive.sSpeed.fltSpeedFilt     = 0.0F;
    g_sM1Drive.sFocPMSM.f16PosElEst    = FRAC16(0.0);
    g_sM1Drive.sFocPMSM.fltSpeedElEst  = 0.0F;

    MCS_PMSMAlignment(&g_sM1Drive.sAlignment);
    g_sM1Drive.sFocPMSM.f16PosElExt = g_sM1Drive.sAlignment.f16PosAlign;
    M1_PMSMFocPositionObsDQ(&g_sM1Drive.sFocPMSM);
    MCS_PMSMFocCurrentCtrl(&g_sM1Drive.sFocPMSM);
    MCS_PMSMDeadTimeComp(&g_sM1Drive.sFocPMSM);
    M1_PMSMFocPhaseShiftCtrl(&g_sM1Drive.sFocPMSM);
}

/*!
 * @brief Start-up process called in fast state machine as Run sub state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_StateRunStartupFast(void)
{
    /* If f16SpeedCmd = 0, go to Free-wheel state */
    if ((g_sM1Drive.sSpeed.fltSpeedCmd == 0.0F) && (g_sM1Drive.eControl == kControlMode_SpeedFOC))
    {
    	M1_TransRunStartupFreewheel();
    }

    /* Type the code to do when in the RUN STARTUP sub-state */
    /* Pass actual estimation position to OL startup structure */
    g_sM1Drive.sStartUp.f16PosEst = g_sM1Drive.sFocPMSM.f16PosElEst;

    /* Open loop startup */
    MCS_PMSMOpenLoopStartUp(&g_sM1Drive.sStartUp);

    /* Pass f16SpeedRampOpenloop to f16SpeedRamp*/
    g_sM1Drive.sSpeed.fltSpeedRamp = g_sM1Drive.sStartUp.fltSpeedRampOpenLoop;

    /* Position and speed for FOC */
    g_sM1Drive.sFocPMSM.f16PosElExt = g_sM1Drive.sStartUp.f16PosMerged;

    /* MCAT control structure switch */
    switch (g_sM1Drive.eControl)
    {
        case kControlMode_Scalar:
            /* Switch directly to SPIN state */
            M1_TransRunStartupSpin();
            break;

        case kControlMode_VoltageFOC:
            /* Pass MCAT required values in run-time */
            g_sM1Drive.sFocPMSM.sUDQReq.fltD = g_sM1Drive.sMCATctrl.sUDQReqMCAT.fltD;
            g_sM1Drive.sFocPMSM.sUDQReq.fltQ = g_sM1Drive.sMCATctrl.sUDQReqMCAT.fltQ;
            /* FOC */
            g_sM1Drive.sFocPMSM.bCurrentLoopOn = FALSE;
            M1_PMSMFocPositionObsDQ(&g_sM1Drive.sFocPMSM);
            MCS_PMSMFocCurrentCtrl(&g_sM1Drive.sFocPMSM);
            MCS_PMSMDeadTimeComp(&g_sM1Drive.sFocPMSM);
            M1_PMSMFocPhaseShiftCtrl(&g_sM1Drive.sFocPMSM);
            break;

        case kControlMode_CurrentFOC:
            /* FOC */
            g_sM1Drive.sFocPMSM.sIDQReq.fltD   = g_sM1Drive.sMCATctrl.sIDQReqMCAT.fltD;
            g_sM1Drive.sFocPMSM.sIDQReq.fltQ   = g_sM1Drive.sMCATctrl.sIDQReqMCAT.fltQ;
            g_sM1Drive.sFocPMSM.bCurrentLoopOn = TRUE;
            
            sFluxObsrv.sCoeff.f32K_Alpha_Coeff = Coeff;
            sFluxObsrv.sCoeff.f32K_Beta_Coeff = Coeff;
            
            M1_PMSMFocPositionObsDQ(&g_sM1Drive.sFocPMSM);
            MCS_PMSMFocCurrentCtrl(&g_sM1Drive.sFocPMSM);
            MCS_PMSMDeadTimeComp(&g_sM1Drive.sFocPMSM);
            M1_PMSMFocPhaseShiftCtrl(&g_sM1Drive.sFocPMSM);

            /* pass estimated speed to actual speed value */
            g_sM1Drive.sSpeed.fltSpeed = g_sM1Drive.sFocPMSM.fltSpeedElEst;
            break;

        default:
            /* Current control loop */
            g_sM1Drive.sFocPMSM.sIDQReq.fltD = 0.0F;

            /* During the open loop start up the values of required Iq current are kept in pre-defined level*/
            if (g_sM1Drive.sSpeed.fltSpeedCmd > 0.0F)
            {
            	g_sM1Drive.sFocPMSM.sIDQReq.fltQ = g_sM1Drive.sStartUp.fltCurrentStartup;
            }
            else
            {
            	g_sM1Drive.sFocPMSM.sIDQReq.fltQ = MLIB_Neg_FLT(g_sM1Drive.sStartUp.fltCurrentStartup);
            }

            /* FOC */
            g_sM1Drive.sFocPMSM.bCurrentLoopOn = TRUE;
            
            M1_PMSMFocPositionObsDQ(&g_sM1Drive.sFocPMSM);
            MCS_PMSMFocCurrentCtrl(&g_sM1Drive.sFocPMSM);
            MCS_PMSMDeadTimeComp(&g_sM1Drive.sFocPMSM);
            M1_PMSMFocPhaseShiftCtrl(&g_sM1Drive.sFocPMSM);

            /* pass estimated speed to actual speed value */
            g_sM1Drive.sSpeed.fltSpeed = g_sM1Drive.sFocPMSM.fltSpeedElEst;
            break;

    }

    /* Switch to close loop  */
    if (!g_sM1Drive.sStartUp.bOpenLoop)
    {
        M1_TransRunStartupSpin();
    }
}

/*!
 * @brief Spin state called in fast state machine as Run sub state
 *
 * @param void  No input parameter
 *
 * @return None
 */
extern float_t fltIdComp;
static void M1_StateRunSpinFast(void)
{
    /* Type the code to do when in the RUN SPIN sub-state */
    /* MCAT control structure switch */
    switch (g_sM1Drive.eControl)
    {
        case kControlMode_Scalar:
            /* Scalar control */
            MCS_PMSMScalarCtrl(&g_sM1Drive.sScalarCtrl);

            /* Pass required voltages to Bemf Observer to work */
            g_sM1Drive.sFocPMSM.sUDQReq.fltQ = g_sM1Drive.sScalarCtrl.sUDQReq.fltQ;
            g_sM1Drive.sFocPMSM.sUDQReq.fltD = g_sM1Drive.sScalarCtrl.sUDQReq.fltD;
            g_sM1Drive.sFocPMSM.f16PosElExt  = g_sM1Drive.sScalarCtrl.f16PosElScalar;

            sFluxObsrv.sCoeff.f32K_Alpha_Coeff = Coeff;
            sFluxObsrv.sCoeff.f32K_Beta_Coeff = Coeff;
            
            /* Call voltage FOC to calculate PWM duty cycles */
            M1_PMSMFocPositionObsDQ(&g_sM1Drive.sFocPMSM);
            MCS_PMSMFocCurrentCtrl(&g_sM1Drive.sFocPMSM);
            MCS_PMSMDeadTimeComp(&g_sM1Drive.sFocPMSM);
            M1_PMSMFocPhaseShiftCtrl(&g_sM1Drive.sFocPMSM);
            
            /* Sub-state RUN FREEWHEEL */
            if (g_sM1Drive.sScalarCtrl.fltFreqCmd == 0.0F)
            {
            	M1_TransRunSpinFreewheel();
            }
            break;

        case kControlMode_VoltageFOC:
            /* FOC */
            g_sM1Drive.sFocPMSM.sUDQReq.fltQ   = g_sM1Drive.sMCATctrl.sUDQReqMCAT.fltQ;
            g_sM1Drive.sFocPMSM.sUDQReq.fltD   = g_sM1Drive.sMCATctrl.sUDQReqMCAT.fltD;
            g_sM1Drive.sFocPMSM.bCurrentLoopOn = FALSE;

            g_sM1Drive.sFocPMSM.bPosExtOn = FALSE;

            M1_PMSMFocPositionObsDQ(&g_sM1Drive.sFocPMSM);
            MCS_PMSMFocCurrentCtrl(&g_sM1Drive.sFocPMSM);
            MCS_PMSMDeadTimeComp(&g_sM1Drive.sFocPMSM);
            M1_PMSMFocPhaseShiftCtrl(&g_sM1Drive.sFocPMSM);

            /* Sub-state RUN FREEWHEEL */
            if (g_sM1Drive.sMCATctrl.sUDQReqMCAT.fltQ == 0.0F)
            {
            	M1_TransRunSpinFreewheel();
            }
            break;

        case kControlMode_CurrentFOC:
            /* Current FOC */
            g_sM1Drive.sFocPMSM.sIDQReq.fltQ = g_sM1Drive.sMCATctrl.sIDQReqMCAT.fltQ;
            g_sM1Drive.sFocPMSM.sIDQReq.fltD = g_sM1Drive.sMCATctrl.sIDQReqMCAT.fltD;

            g_sM1Drive.sFocPMSM.bCurrentLoopOn = TRUE;
            
            sFluxObsrv.sCoeff.f32K_Alpha_Coeff = Coeff;
            sFluxObsrv.sCoeff.f32K_Beta_Coeff = Coeff;
            
            M1_PMSMFocPositionObsDQ(&g_sM1Drive.sFocPMSM);
            MCS_PMSMFocCurrentCtrl(&g_sM1Drive.sFocPMSM);
            MCS_PMSMDeadTimeComp(&g_sM1Drive.sFocPMSM);
            M1_PMSMFocPhaseShiftCtrl(&g_sM1Drive.sFocPMSM);

            g_sM1Drive.sSpeed.fltSpeed = g_sM1Drive.sFocPMSM.fltSpeedElEst;
            /* Sub-state RUN FREEWHEEL */
            if (g_sM1Drive.sMCATctrl.sIDQReqMCAT.fltQ == 0.0F)
            {
            	M1_TransRunSpinFreewheel();
            }
            break;

        case kControlMode_SpeedFOC:
        default:
            if ((MLIB_Abs_FLT(g_sM1Drive.sSpeed.fltSpeedRamp) < g_sM1Drive.sFaultThresholds.fltSpeedMin) &&
                (g_sM1Drive.sMCATctrl.ui16PospeSensor == MCAT_SENSORLESS_CTRL))
            {
                /* Sub-state RUN FREEWHEEL */
                M1_TransRunSpinFreewheel();
            }

            g_sM1Drive.sFocPMSM.bPosExtOn = FALSE;

            /* FOC */
            g_sM1Drive.sFocPMSM.bCurrentLoopOn = TRUE;
            
            sFluxObsrv.sCoeff.f32K_Alpha_Coeff = Coeff;
            sFluxObsrv.sCoeff.f32K_Beta_Coeff = Coeff;
            
            M1_PMSMFocPositionObsDQ(&g_sM1Drive.sFocPMSM);
            
            //LDOB
            /* Mechanical position calculation */
            sMecPositionObser.f16ElePositionNew = g_sM1Drive.sFocPMSM.f16PosElEst;
            MechAngleCalcUpdate(&sMecPositionObser);
            sMecPositionObser.f16ElePositionOld = g_sM1Drive.sFocPMSM.f16PosElEst;
            
            //Id Compensation
            g_sM1IdComp.fltCurrentErr = g_sM1Drive.sFocPMSM.sIDQReq.fltD - g_sM1Drive.sFocPMSM.sIDQ.fltD;

            g_sM1IdComp.f16Speed = MLIB_ConvSc_F16ff(g_sM1Drive.sSpeed.fltSpeed, M1_N_MAX);
              
            fltIdComp = Harmonic_Compensation(&sMecPositionObser, &g_sM1IdComp);

            g_sM1Drive.sSpeed.f16Speed = MLIB_ConvSc_F16ff(g_sM1Drive.sSpeed.fltSpeed, M1_N_MAX);
            if (g_sM1Drive.sSpeed.f16Speed > FRAC16(200.0/(M1_N_MAX*M1_N_ANGULAR_MAX)))
            {
		/* torque transfer */ 
                sTLObser.w16TeEl = sFluxObsrv.f16Te;//f16TeElec + f16TeReluctance;
                /* speed transfer */
		sTLObser.w16SpeedRef = g_sM1Drive.sSpeed.f16Speed;
                
		/* D current transfer */
		sTLObser.sIDQReqTLObser.f16D = MLIB_ConvSc_F16ff(g_sM1Drive.sFocPMSM.sIDQReq.fltD, M1_I_MAX);
		/* Speed Ramp tansfer */
		sTLObser.f16SpeedRamp = MLIB_ConvSc_F16ff(g_sM1Drive.sSpeed.fltSpeedRamp, M1_N_MAX);
		/* load torque estimation */
		TLOnlineTuningUpdate(&sTLObser);
		
		/* Move average filter for Load torque */
		MAFilterTLUpdate(&sTLObser, &sMecPositionObser);
		
		/* load torque compensation */
		TLOnlineCompenStateMachine(&sTLObser);
                
                g_sM1Drive.sFocPMSM.sIDQReq.fltQ = MLIB_Add_FLT(g_sM1Drive.sSpeed.fltIqReq, MLIB_ConvSc_FLTsf(sTLObser.sIDQReqTLObser.f16Q, M1_I_MAX));
            }
            
            /* Call voltage FOC to calculate PWM duty cycles */
            MCS_PMSMFocCurrentCtrl(&g_sM1Drive.sFocPMSM);
            MCS_PMSMDeadTimeComp(&g_sM1Drive.sFocPMSM);
            M1_PMSMFocPhaseShiftCtrl(&g_sM1Drive.sFocPMSM);
            
            /* Pass estimated speed to actual speed value */
            g_sM1Drive.sSpeed.fltSpeed = g_sM1Drive.sFocPMSM.fltSpeedElEst;
            break;
    }
}

/*!
 * @brief Free-wheel process called in fast state machine as Run sub state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_StateRunFreewheelFast(void)
{
    /* Type the code to do when in the RUN FREEWHEEL sub-state */

    /* Clear actual speed values */
    g_sM1Drive.sScalarCtrl.fltFreqRamp = 0.0F;
    g_sM1Drive.sSpeed.fltSpeed         = 0.0F;
    g_sM1Drive.sSpeed.fltSpeedFilt     = 0.0F;
    g_sM1Drive.sSpeed.fltSpeedRamp     = 0.0F;
}

/*!
 * @brief Calibration process called in slow state machine as Run sub state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_StateRunCalibSlow(void)
{
    /* Write calibrated offset values */
    M1_MCDRV_CURR_3PH_CALIB_SET(&g_sM1AdcSensor);
    
    if (--g_sM1Drive.ui16CounterState == 0U)
    {
      /* To switch to the RUN READY sub-state */
      M1_TransRunCalibReady();
    }
}

/*!
 * @brief Ready state called in slow state machine as Run sub state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_StateRunReadySlow(void)
{
}

/*!
 * @brief Alignment process called in slow state machine as Run sub state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_StateRunAlignSlow(void)
{
}

/*!
 * @brief Start-up process called in slow state machine as Run sub state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_StateRunStartupSlow(void)
{
    if (g_sM1Drive.eControl == kControlMode_SpeedFOC)
    {
        /* Actual speed filter */
        g_sM1Drive.sSpeed.fltSpeedFilt =
            GDFLIB_FilterIIR1_FLT(g_sM1Drive.sSpeed.fltSpeed, &g_sM1Drive.sSpeed.sSpeedFilter);

        /* Pass required speed values lower than nominal speed */
        if ((MLIB_Abs_FLT(g_sM1Drive.sSpeed.fltSpeedCmd) > g_sM1Drive.sFaultThresholds.fltSpeedNom))
        {
            /* Set required speed to nominal speed if over speed command > speed nominal */
            if (g_sM1Drive.sSpeed.fltSpeedCmd > 0.0F)
            {
            	g_sM1Drive.sSpeed.fltSpeedCmd = g_sM1Drive.sFaultThresholds.fltSpeedNom;
            }
            else
            {
            	g_sM1Drive.sSpeed.fltSpeedCmd = MLIB_Neg_FLT(g_sM1Drive.sFaultThresholds.fltSpeedNom);
            }
        }
    }
}

/*!
 * @brief Spin state called in slow state machine as Run sub state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_StateRunSpinSlow(void)
{
    if (g_sM1Drive.eControl == kControlMode_SpeedFOC)
    {
        /* Actual speed filter */
        g_sM1Drive.sSpeed.fltSpeedFilt =
            GDFLIB_FilterIIR1_FLT(g_sM1Drive.sSpeed.fltSpeed, &g_sM1Drive.sSpeed.sSpeedFilter);

        /* Pass required speed values lower than nominal speed */
        if ((MLIB_Abs_FLT(g_sM1Drive.sSpeed.fltSpeedCmd) > g_sM1Drive.sFaultThresholds.fltSpeedNom))
        {
            /* Set required speed to nominal speed if over speed command > speed nominal */
            if (g_sM1Drive.sSpeed.fltSpeedCmd > 0.0F)
            {
            	g_sM1Drive.sSpeed.fltSpeedCmd = g_sM1Drive.sFaultThresholds.fltSpeedNom;
            }
            else
            {
            	g_sM1Drive.sSpeed.fltSpeedCmd = MLIB_Neg_FLT(g_sM1Drive.sFaultThresholds.fltSpeedNom);
            }

        }
        //switch speed loop control parameters after speed stable
        if(!bStartupFinish)
        {
            u16StartupTime++;
            g_sM1Drive.sSpeed.sSpeedPiParams.fltPGain = M1_SPEED_PI_PROP_GAIN + u16StartupTime*(0.00116616F-M1_SPEED_PI_PROP_GAIN)/MergeDuration;
            g_sM1Drive.sSpeed.sSpeedPiParams.fltIGain = M1_SPEED_PI_INTEG_GAIN + u16StartupTime*(0.0000185629F-M1_SPEED_PI_INTEG_GAIN)/MergeDuration;
            if(u16StartupTime>=MergeDuration)
            {
                // Set a low bandwidth of speed loop and acceleration after startup successfully
                g_sM1Drive.sSpeed.sSpeedRampParams.fltRampUp = 60.0/M1_N_ANGULAR_MAX/M1_SLOW_LOOP_FREQ;
                g_sM1Drive.sSpeed.sSpeedPiParams.fltPGain    = 0.00116616F;
                g_sM1Drive.sSpeed.sSpeedPiParams.fltIGain    = 0.0000185629F;
                bStartupFinish = TRUE;
                
                //F0=80Hz, ksi=0.3
//                g_sM1Drive.sFocPMSM.sIdPiParams.fltPGain    = 2.299;
//                g_sM1Drive.sFocPMSM.sIdPiParams.fltIGain    = 0.4892;
//                g_sM1Drive.sFocPMSM.sIqPiParams.fltPGain    = 4.380;
//                g_sM1Drive.sFocPMSM.sIqPiParams.fltIGain    = 0.7681;
                
                //F0=220Hz, ksi=1.3
                g_sM1Drive.sFocPMSM.sIdPiParams.fltPGain    = 20;
                g_sM1Drive.sFocPMSM.sIdPiParams.fltIGain    = 3;
                g_sM1Drive.sFocPMSM.sIqPiParams.fltPGain    = 25;
                g_sM1Drive.sFocPMSM.sIqPiParams.fltIGain    = 4;
//                sTLObser.w16EnableFeedForwCompFlag = TRUE;
            }
        }

        if ((MLIB_Abs_FLT(g_sM1Drive.sSpeed.fltSpeedRamp) < g_sM1Drive.sFaultThresholds.fltSpeedMin) &&
            (g_sM1Drive.sMCATctrl.ui16PospeSensor == MCAT_SENSORLESS_CTRL))
        {
        	M1_TransRunSpinFreewheel();
        }

        /* call PMSM speed control */
        g_sM1Drive.sSpeed.bIqPiLimFlag = g_sM1Drive.sFocPMSM.sIqPiParams.bLimFlag;
        MCS_FocFieldWeakeningCtrl(&g_sM1Drive.sFocPMSM, &g_sM1Drive.sFocFw, &g_sM1Drive.sSpeed);
        MCS_PMSMFocCtrlSpeed(&g_sM1Drive.sSpeed);
    }
}

/*!
 * @brief Free-wheel process called in slow state machine as Run sub state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_StateRunFreewheelSlow(void)
{
    /* Wait until free-wheel time passes */
    if (--g_sM1Drive.ui16CounterState == 0U)
    {
        /* Switch to sub state READY */
        M1_TransRunFreewheelReady();
    }
}

/*!
 * @brief Transition from Calib to Ready state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_TransRunCalibReady(void)
{
    /* Type the code to do when going from the RUN CALIB to the RUN READY sub-state */

    /* Set 50% PWM duty cycle */
    g_sM1Drive.sFocPMSM.sDutyABC.f16A = FRAC16(0.5);
    g_sM1Drive.sFocPMSM.sDutyABC.f16B = FRAC16(0.5);
    g_sM1Drive.sFocPMSM.sDutyABC.f16C = FRAC16(0.5);

    /* Switch to sub state READY */
    g_eM1StateRun = kRunState_Ready;
}

/*!
 * @brief Transition from Ready to Align state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_TransRunReadyAlign(void)
{
    /* Type the code to do when going from the RUN kRunState_Ready to the RUN kRunState_Align sub-state */
    /* Alignment duration set-up */
    g_sM1Drive.ui16CounterState = g_sM1Drive.sAlignment.ui16Time;
    /* Counter of half alignment duration */
    g_sM1Drive.sAlignment.ui16TimeHalf = (uint16_t)MLIB_ShR_F16((int16_t)g_sM1Drive.sAlignment.ui16Time, 1);

    /* set required alignment voltage to Ud */
    g_sM1Drive.sFocPMSM.sUDQReq.fltD = g_sM1Drive.sAlignment.fltUdReq;
    g_sM1Drive.sFocPMSM.sUDQReq.fltQ = 0.0F;

    /* enable passing required position to FOC */
    g_sM1Drive.sFocPMSM.bPosExtOn = TRUE;

    /* disable current FOC */
    g_sM1Drive.sFocPMSM.bCurrentLoopOn = FALSE;

    /* enable Open loop mode in main control structure */
    g_sM1Drive.sFocPMSM.bOpenLoop = TRUE;
    
    TLOnlineTuningInitRep(&sTLObser);
    MechAngleCalcInit(&sMecPositionObser);
    MAFilterTLInit(&sTLObser);
    bStartupFinish = FALSE;
    u16StartupTime = 0;
    g_sM1Drive.sSpeed.sSpeedRampParams.fltRampUp = M1_SPEED_RAMP_UP;
    g_sM1Drive.sSpeed.sSpeedPiParams.fltPGain    = M1_SPEED_PI_PROP_GAIN;
    g_sM1Drive.sSpeed.sSpeedPiParams.fltIGain    = M1_SPEED_PI_INTEG_GAIN;

    /* Enable PWM output */
    M1_MCDRV_PWM3PH_EN(&g_sM1Pwm3ph);

    /* Sub-state RUN ALIGN */
    g_eM1StateRun = kRunState_Align;
}

/*!
 * @brief Transition from Align to Startup state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_TransRunAlignStartup(void)
{
    /* Type the code to do when going from the RUN kRunState_Align to the RUN kRunState_Startup sub-state */

    /* Clear application parameters */
    M1_ClearFOCVariables();

    /* Pass required speed to open loop start-up structure */
    if (g_sM1Drive.sSpeed.fltSpeedCmd > 0.0F)
    {
    	g_sM1Drive.sStartUp.fltSpeedReq = g_sM1Drive.sStartUp.fltSpeedCatchUp;
    }
    else
    {
    	g_sM1Drive.sStartUp.fltSpeedReq = MLIB_Neg_FLT(g_sM1Drive.sStartUp.fltSpeedCatchUp);
    }

    /* Enable Open loop mode in main control structure */
    g_sM1Drive.sStartUp.bOpenLoop = TRUE;
    g_sM1Drive.sFocPMSM.bOpenLoop = TRUE;

    /* Enable Open loop mode in FOC module */
    g_sM1Drive.sFocPMSM.bPosExtOn = TRUE;

    g_sM1Drive.sFocPMSM.ui16SectorSVM = M1_SVM_SECTOR_DEFAULT;
    GDFLIB_FilterIIR1Init_FLT(&g_sM1Drive.sSpeed.sSpeedFilter);
    
    sFluxObsrv.Init(&sFluxObsrv);

    /* Go to sub-state RUN STARTUP */
    g_eM1StateRun = kRunState_Startup;
}

/*!
 * @brief Transition from Align to Ready state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_TransRunAlignReady(void)
{
    /* Type the code to do when going from the RUN kRunState_Align to the RUN kRunState_Ready sub-state */

    /* Clear FOC accumulators */
    M1_ClearFOCVariables();

    /* Go to sub-state RUN READY */
    g_eM1StateRun = kRunState_Ready;
}

/*!
 * @brief Transition from Startup to Spin state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_TransRunStartupSpin(void)
{
    /* Type the code to do when going from the RUN kRunState_Startup to the RUN kRunState_Spin sub-state */
    /* For FOC control switch open loop off in DQ observer */
    if (g_sM1Drive.eControl != kControlMode_Scalar)
    {
        g_sM1Drive.sFocPMSM.bPosExtOn = FALSE; /* disable passing external electrical position to FOC */
        g_sM1Drive.sFocPMSM.bOpenLoop = FALSE; /* disable parallel running open-loop and estimator */
    }

    g_sM1Drive.sSpeed.sSpeedPiParams.fltIAccK_1 = g_sM1Drive.sFocPMSM.sIDQReq.fltQ;
    g_sM1Drive.sSpeed.sSpeedRampParams.fltState = g_sM1Drive.sStartUp.fltSpeedRampOpenLoop;

    /* To switch to the RUN kRunState_Spin sub-state */
    g_eM1StateRun = kRunState_Spin;
}

/*!
 * @brief Transition from Startup to Free-wheel state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_TransRunStartupFreewheel(void)
{
    M1_MCDRV_PWM3PH_DIS(&g_sM1Pwm3ph);

    /* Free-wheel duration set-up */
    g_sM1Drive.ui16CounterState = g_sM1Drive.ui16TimeFullSpeedFreeWheel;

    /* Enter FREEWHEEL sub-state */
    g_eM1StateRun = kRunState_Freewheel;
}

/*!
 * @brief Transition from Spin to Free-wheel state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_TransRunSpinFreewheel(void)
{
    /* Type the code to do when going from the RUN SPIN to the RUN FREEWHEEL sub-state */
    /* Set 50% PWM duty cycle */
    g_sM1Drive.sFocPMSM.sDutyABC.f16A = FRAC16(0.5);
    g_sM1Drive.sFocPMSM.sDutyABC.f16B = FRAC16(0.5);
    g_sM1Drive.sFocPMSM.sDutyABC.f16C = FRAC16(0.5);

    g_sM1Drive.sFocPMSM.ui16SectorSVM = M1_SVM_SECTOR_DEFAULT;

    M1_MCDRV_PWM3PH_DIS(&g_sM1Pwm3ph);

    /* Generates a time gap before the alignment to assure the rotor is not rotating */
    g_sM1Drive.ui16CounterState = g_sM1Drive.ui16TimeFullSpeedFreeWheel;

    g_sM1Drive.sFocPMSM.sIDQReq.fltD = 0.0F;
    g_sM1Drive.sFocPMSM.sIDQReq.fltQ = 0.0F;

    g_sM1Drive.sFocPMSM.sUDQReq.fltD = 0.0F;
    g_sM1Drive.sFocPMSM.sUDQReq.fltQ = 0.0F;

    g_sM1Drive.sFocPMSM.sIAlBe.fltAlpha    = 0.0F;
    g_sM1Drive.sFocPMSM.sIAlBe.fltBeta     = 0.0F;
    g_sM1Drive.sFocPMSM.sUAlBeReq.fltAlpha = 0.0F;
    g_sM1Drive.sFocPMSM.sUAlBeReq.fltBeta  = 0.0F;

    /* Enter FREEWHEEL sub-state */
    g_eM1StateRun = kRunState_Freewheel;
}

/*!
 * @brief Transition from Free-wheel to Ready state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_TransRunFreewheelReady(void)
{
    /* Type the code to do when going from the RUN kRunState_FreeWheel to the RUN kRunState_Ready sub-state */
    /* Clear application parameters */
    M1_ClearFOCVariables();

    M1_MCDRV_PWM3PH_EN(&g_sM1Pwm3ph);

    /* Sub-state RUN READY */
    g_eM1StateRun = kRunState_Ready;
}

/*!
 * @brief Clear FOC variables in global variable
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_ClearFOCVariables(void)
{
    g_sM1Drive.sAlignment.ui16TimeHalf = 0U;

    /* Clear FOC variables */
    g_sM1Drive.sFocPMSM.sIABC.fltA             = 0.0F;
    g_sM1Drive.sFocPMSM.sIABC.fltB             = 0.0F;
    g_sM1Drive.sFocPMSM.sIABC.fltC             = 0.0F;
    g_sM1Drive.sFocPMSM.sIAlBe.fltAlpha        = 0.0F;
    g_sM1Drive.sFocPMSM.sIAlBe.fltBeta         = 0.0F;
    g_sM1Drive.sFocPMSM.sIDQ.fltD              = 0.0F;
    g_sM1Drive.sFocPMSM.sIDQ.fltQ              = 0.0F;
    g_sM1Drive.sFocPMSM.sIDQReq.fltD           = 0.0F;
    g_sM1Drive.sFocPMSM.sIDQReq.fltQ           = 0.0F;
    g_sM1Drive.sFocPMSM.sIDQError.fltD         = 0.0F;
    g_sM1Drive.sFocPMSM.sIDQError.fltQ         = 0.0F;
    g_sM1Drive.sFocPMSM.sDutyABC.f16A          = FRAC16(0.5);
    g_sM1Drive.sFocPMSM.sDutyABC.f16B          = FRAC16(0.5);
    g_sM1Drive.sFocPMSM.sDutyABC.f16C          = FRAC16(0.5);
    g_sM1Drive.sFocPMSM.sUAlBeReq.fltAlpha     = 0.0F;
    g_sM1Drive.sFocPMSM.sUAlBeReq.fltBeta      = 0.0F;
    g_sM1Drive.sFocPMSM.sUDQReq.fltD           = 0.0F;
    g_sM1Drive.sFocPMSM.sUDQReq.fltQ           = 0.0F;
    g_sM1Drive.sFocPMSM.sAnglePosEl.fltSin     = 0.0F;
    g_sM1Drive.sFocPMSM.sAnglePosEl.fltCos     = 0.0F;
    g_sM1Drive.sFocPMSM.sAnglePosEl.fltSin     = 0.0F;
    g_sM1Drive.sFocPMSM.sAnglePosEl.fltCos     = 0.0F;
    g_sM1Drive.sFocPMSM.sIdPiParams.bLimFlag   = FALSE;
    g_sM1Drive.sFocPMSM.sIqPiParams.bLimFlag   = FALSE;
    g_sM1Drive.sFocPMSM.sIdPiParams.fltIAccK_1 = 0.0F;
    g_sM1Drive.sFocPMSM.sIdPiParams.fltIAccK_1 = 0.0F;
    g_sM1Drive.sFocPMSM.sIqPiParams.fltIAccK_1 = 0.0F;
    g_sM1Drive.sFocPMSM.sIqPiParams.fltIAccK_1 = 0.0F;
    GDFLIB_FilterIIR1Init_FLT(&g_sM1Drive.sFocPMSM.sSpeedElEstFilt);
    g_sM1Drive.sFocPMSM.bIdPiStopInteg = FALSE;
    g_sM1Drive.sFocPMSM.bIqPiStopInteg = FALSE;

    /* Clear Speed control state variables */
    g_sM1Drive.sSpeed.sSpeedRampParams.fltState  = 0.0F;
    g_sM1Drive.sSpeed.fltSpeed                   = 0.0F;
    g_sM1Drive.sSpeed.fltSpeedFilt               = 0.0F;
    g_sM1Drive.sSpeed.fltSpeedError              = 0.0F;
    g_sM1Drive.sSpeed.fltSpeedRamp               = 0.0F;
    g_sM1Drive.sSpeed.sSpeedPiParams.fltIAccK_1  = 0.0F;
    g_sM1Drive.sSpeed.sSpeedPiParams.bLimFlag    = FALSE;
    g_sM1Drive.sSpeed.sSpeedFilter.fltFltBfrX[0] = 0.0F;
    g_sM1Drive.sSpeed.sSpeedFilter.fltFltBfrY[0] = 0.0F;
    g_sM1Drive.sSpeed.bSpeedPiStopInteg          = FALSE;
    GDFLIB_FilterIIR1Init_FLT(&g_sM1Drive.sSpeed.sSpeedFilter);

    /* Init Blocked rotor filter */
    GDFLIB_FilterMAInit_FLT(0.0F, &g_sM1Drive.msM1BlockedRotorUqFilt);

    /* Clear Scalar control variables */
    g_sM1Drive.sScalarCtrl.fltFreqRamp                 = 0.0F;
    g_sM1Drive.sScalarCtrl.f16PosElScalar              = FRAC16(0.0);
    g_sM1Drive.sScalarCtrl.sUDQReq.fltD                = 0.0F;
    g_sM1Drive.sScalarCtrl.sUDQReq.fltQ                = 0.0F;
    g_sM1Drive.sScalarCtrl.sFreqIntegrator.f32IAccK_1  = 0;
    g_sM1Drive.sScalarCtrl.sFreqIntegrator.f16InValK_1 = 0;
    g_sM1Drive.sScalarCtrl.sFreqRampParams.fltState    = 0.0F;

    /* Clear Startup variables */
    g_sM1Drive.sStartUp.f16PosMerged                      = 0;
    g_sM1Drive.sStartUp.f16PosEst                         = 0;
    g_sM1Drive.sStartUp.f16PosGen                         = 0;
    g_sM1Drive.sStartUp.f16RatioMerging                   = 0;
    g_sM1Drive.sStartUp.fltSpeedRampOpenLoop              = 0.0F;
    g_sM1Drive.sStartUp.fltSpeedReq                       = 0.0F;
    g_sM1Drive.sStartUp.sSpeedIntegrator.f32IAccK_1       = 0;
    g_sM1Drive.sStartUp.sSpeedIntegrator.f16InValK_1      = 0;
    g_sM1Drive.sStartUp.sSpeedRampOpenLoopParams.fltState = 0.0F;

    /* Clear BEMF and Tracking observers state variables */
    AMCLIB_PMSMBemfObsrvDQInit_A32fff(&g_sM1Drive.sFocPMSM.sBemfObsrv);
    AMCLIB_TrackObsrvInit_A32af(ACC32(0.0), &g_sM1Drive.sFocPMSM.sTo);
}

/*!
 * @brief Fault detention routine - check various faults
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void M1_FaultDetection(void)
{
    /* Clearing actual faults before detecting them again  */
    /* Clear all pending faults */
    FAULT_CLEAR_ALL(g_sM1Drive.sFaultIdPending);

    /* Clear fault captured manually if required. */
    if (g_sM1Drive.bFaultClearMan)
    {
        /* Clear fault captured */
        FAULT_CLEAR_ALL(g_sM1Drive.sFaultIdCaptured);
        g_sM1Drive.bFaultClearMan = FALSE;
    }

    /* Fault:   DC-bus over-current */
    if (M1_MCDRV_PWM3PH_FLT_GET(&g_sM1Pwm3ph))
    {
    	FAULT_SET(g_sM1Drive.sFaultIdPending, FAULT_I_DCBUS_OVER);
    }

    /* Fault:   DC-bus over-voltage */
    if (g_sM1Drive.sFocPMSM.fltUDcBusFilt > g_sM1Drive.sFaultThresholds.fltUDcBusOver)
    {
    	FAULT_SET(g_sM1Drive.sFaultIdPending, FAULT_U_DCBUS_OVER);
    }

    /* Fault:   DC-bus under-voltage */
    if (g_sM1Drive.sFocPMSM.fltUDcBusFilt < g_sM1Drive.sFaultThresholds.fltUDcBusUnder)
    {
    	FAULT_SET(g_sM1Drive.sFaultIdPending, FAULT_U_DCBUS_UNDER);
    }

    /* Check only in SPEED_FOC control, RUN state, kRunState_Spin and kRunState_FreeWheel sub-states */
    if ((g_sM1Drive.eControl == kControlMode_SpeedFOC) && (g_sM1Ctrl.eState == kSM_AppRun) &&
        (g_eM1StateRun == kRunState_Spin || g_eM1StateRun == kRunState_Freewheel))
    {
        /* Fault: Over-speed  */
        if ((MLIB_Abs_FLT(g_sM1Drive.sSpeed.fltSpeedFilt) > g_sM1Drive.sFaultThresholds.fltSpeedOver) &&
            (MLIB_Abs_FLT(g_sM1Drive.sSpeed.fltSpeedCmd) > g_sM1Drive.sFaultThresholds.fltSpeedMin))
        {
        	FAULT_SET(g_sM1Drive.sFaultIdPending, FAULT_SPEED_OVER);
        }
    }
    /* Pass fault to Fault ID Captured */
    g_sM1Drive.sFaultIdCaptured |= g_sM1Drive.sFaultIdPending;
}

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @brief Set application switch value to On or Off mode
 *
 * @param bValue  bool value, true - On of false - Off
 *
 * @return None
 */
void M1_SetAppSwitch(bool_t bValue)
{
    g_bM1SwitchAppOnOff = bValue;
}

/*!
 * @brief Get application switch value
 *
 * @param void  No input parameter
 *
 * @return bool_t Return bool value, true or false
 */
bool_t M1_GetAppSwitch(void)
{
    return (g_bM1SwitchAppOnOff);
}

/*!
 * @brief Get application state
 *
 * @param void  No input parameter
 *
 * @return uint16_t Return current application state
 */
uint16_t M1_GetAppState(void)
{
    return ((uint16_t)g_sM1Ctrl.eState);
}

/*!
 * @brief Set spin speed of the motor in float value
 *
 * @param fltSpeedCmd  Speed command - set speed
 *
 * @return None
 */
void M1_SetSpeed(float_t fltSpeedCmd)
{
    if (g_bM1SwitchAppOnOff)
    {
        /* Set speed */
        if (MLIB_Abs_FLT(fltSpeedCmd / g_fltM1speedAngularScale) < g_sM1Drive.sStartUp.fltSpeedCatchUp)
        {
            g_sM1Drive.sSpeed.fltSpeedCmd = 0.0F;
        }
        else if (MLIB_Abs_FLT(fltSpeedCmd / g_fltM1speedAngularScale) > M1_N_NOM)
        {
            g_sM1Drive.sSpeed.fltSpeedCmd = 0.0F;
        }
        else
        {
            g_sM1Drive.sSpeed.fltSpeedCmd = fltSpeedCmd / g_fltM1speedAngularScale;
        }
    }
    else
    {
        /* Set zero speed */
        g_sM1Drive.sSpeed.fltSpeedCmd = 0.0F;
    }
}

/*!
 * @brief Get spin speed of the motor in float value
 *
 * @param void  No input parameter
 *
 * @return float_t Float value of the current speed
 */
float_t M1_GetSpeed(void)
{
    /* Return speed */
    return g_sM1Drive.sSpeed.fltSpeedCmd;
}
