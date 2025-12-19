/*
    * Copyright 2025 NXP
    *
    * SPDX-License-Identifier: BSD-3-Clause
*/

/*
    * FILE NAME: MC_Parameters.h
    * DATE: Fri Dec 19 2025, 10:51:53
*/

/*
{
    "compressorParameters": {
        "compressorParametersPP": 3,
        "compressorParametersR": 2.53,
        "compressorParametersLD": 0.0148,
        "compressorParametersLQ": 0.01165,
        "compressorParametersSpeedNom": 4000,
        "compressorParametersFlux": 0.097475163,
        "compressorParametersUnom": 179,
        "compressorParametersImax": 10.12,
        "compressorParametersUdcbMax": 443.3,
        "compressorParametersSpeedMax": 4200,
        "compressorParametersUdcbUnder": 180,
        "compressorParametersUdcbTrip": 380,
        "compressorParametersUdcbOver": 400,
        "compressorParametersSpeedOver": 4500,
        "compressorParametersSpeedMin": 100,
        "compressorParametersCalibDuration": 200,
        "compressorParametersFaultDurationTime": 3000,
        "compressorParametersFreewheelDurationTime": 1500
    },
    "compressorCurrentLoop": {
        "compressorFastLoopFreq": 6250,
        "compressorCurrentLoopKsi": 1.5,
        "compressorCurrentLoopF0": 150,
        "compressorCurrentLoopOutputLimit": 90,
        "compressorCurrentLoopUdcbFilterFreq": 5
    },
    "compressorSpeedLoop": {
        "compressorSpeedLoopPiPropGain": 0.0096616,
        "compressorSpeedLoopPIIntegGain": 0.000185629,
        "compressorSlowLoopFreq": 1000,
        "compressorSpeedLoopRampUp": 15000,
        "compressorSpeedLoopRampDown": 80,
        "compressorSpeedLoopUpperLimit": 4.5,
        "compressorSpeedLoopLowerLimit": -2,
        "compressorSpeedFilterFreq": 600,
        "compressorSpeedToFilterFreq": 400,
        "compressorOlStartSpeedRamp": 15000,
        "compressorOlStartI": 2,
        "compressorOlMergeSpeed": 1500,
        "compressorMergeCoeff": 100,
        "compressorSpeedLoopCutOffFreq": 600,
        "compressorSpeedLoopManualConstantTunning": false
    },
    "compressorObserver": {
        "compressorHybridFluxGain": 4,
        "compressorFluxCurrentLimit": 5,
        "compressorFluxToAtt": 0.707,
        "compressorFluxToF0": 140
    },
    "compressorScalar": {
        "compressorScalarUqMin": 8,
        "compressorAlignVoltage": 8,
        "compressorAlignDurationTime": 0.1,
        "compressorScalarVhzFactor": 0.05
    },
    "compressorFw": {
        "compressorFwOn": 2600,
        "compressorPiFwPGain": 0.5,
        "compressorPiFwIGain": 0.1,
        "compressorPiFwOutputLimit": 2,
        "compressorFwFilterCutoffFreq": 300
    },
    "compressorHc": {
        "compressorHcShiftTable": 6,
        "compressorHcGain": 0.2,
        "compressorHcOutLimit": 0.5,
        "compressorHcZk": 12,
        "compressorHcQz": 0.98,
        "compressorHcStoreLimit": 10,
        "compressorHcFiltProp": 0.5,
        "compressorHcSpeedCoeff": 3.75
    },
    "compressorTc": {
        "compressorShiftIndexTable": 8,
        "compressorLoadTorqueLdob": 8,
        "compressorKpLdob": 0.5,
        "compressorKiLdob": 5,
        "compressorInertiaJLdob": 0.00029,
        "compressorMaPointIndex": 5,
        "compressorMaxIqReqLdob": 3,
        "compressorFluxMax": 1.5
    },
    "fanParameters": {
        "fanParametersPP": 5,
        "fanParametersR": 35.85,
        "fanParametersLD": 0.0925,
        "fanParametersLQ": 0.108,
        "fanParametersSpeedNom": 1100,
        "fanParametersFlux": 0.248098003,
        "fanParametersUnom": 179,
        "fanParametersImax": 1.584,
        "fanParametersUdcbMax": 443.3,
        "fanParametersSpeedMax": 1500,
        "fanParametersSpeedOver": 1300,
        "fanParametersSpeedMin": 150,
        "fanParametersCalibDuration": 200,
        "fanParametersFaultDurationTime": 3000,
        "fanParametersFreewheelDurationTime": 1500
    },
    "fanCloop": {
        "fanFastLoopFreq": 8000,
        "fanCloopAtt": 1,
        "fanCloopFreq": 150,
        "fanCurrentCloopLimit": 90,
        "fanCurrentLoopUdcbFilterFreq": 5
    },
    "fanSpeedLoop": {
        "fanSlowLoopFreq": 1000,
        "fanSpeedPIPropGain": 0.36837,
        "fanSpeedPIIntegGain": 0.00000192878,
        "fanSpeedLoopRampUp": 400,
        "fanSpeedLoopRampDown": 100,
        "fanSpeedLoopHighLimit": 0.5,
        "fanSpeedLoopLowLimit": -0.3,
        "fanSpeedFilterFreq": 20,
        "fanToSpeedFilterFreq": 100
    },
    "fanScalar": {
        "fanScalarUqMin": 5,
        "fanAlignVoltage": 20,
        "fanAlignDurationTime": 2,
        "fanScalarVHzFactor": 1,
        "fanOpenLoopStartI": 0.2,
        "fanOpenLoopMergeSpeed": 200
    },
    "fanObserver": {
        "fanFluxToAtt": 0.707,
        "fanFluxToF0": 60,
        "fanObserverGain": 1.5
    },
    "sensors": {
        "sensorEncPulseNumber": "",
        "sensorEncDir": "",
        "sensorEncNmin": "",
        "sensorObsrvParSampleTime": "",
        "sensorObsrvParF0": "",
        "sensorObsrvParKsi": "",
        "positionLoopPLKp": ""
    }
}
*/

/*
{
    "motorName": "",
    "motorDescription": ""
}
*/

#ifndef __MC_PARAMETERS_H_ 
#define __MC_PARAMETERS_H_ 

#include "trigonometric.h"

/* COMPRESSORPARAMETERS*/
#define M1_MOTOR_PP (3)
#define M1_R (2.53F)
#define M1_LD (0.0148F)
#define M1_LQ (0.01165F)
#define M1_SPEED_NOM (4000.0F)
#define M1_FLUX (0.0974752F)
#define M1_U_NOM (179.0F)
#define M1_I_MAX (10.12F)
#define M1_U_DCB_MAX (443.3F)
#define M1_SPEED_MAX (4200.0F)
#define M1_U_MAX  (M1_U_DCB_MAX/1.732F) 
#define M1_N_ANGULAR_MAX (60.0/PI/2/M1_MOTOR_PP) 
#define M1_N_NOM (M1_SPEED_NOM/M1_N_ANGULAR_MAX) 
#define M1_N_MAX (M1_SPEED_MAX/M1_N_ANGULAR_MAX) 
#define M1_U_DCB_UNDERVOLTAGE (180.0F)
#define M1_U_DCB_TRIP (380.0F)
#define M1_U_DCB_OVERVOLTAGE (400.0F)
#define M1_SPEED_OVER (4500.0F)
#define M1_SPEED_MIN (100.0F)
#define M1_N_OVERSPEED (M1_SPEED_OVER/M1_N_ANGULAR_MAX) 
#define M1_N_MIN (M1_SPEED_MIN/M1_N_ANGULAR_MAX) 
#define M1_CALIB_DURATION (200)
#define M1_FAULT_DURATION_TIME (3000)
#define M1_FREEWHEEL_DURATION_TIME (1500)
#define M1_FAULT_DURATION (M1_FAULT_DURATION_TIME*M1_SLOW_LOOP_FREQ/1000.0) 
#define M1_FREEWHEEL_DURATION (M1_FREEWHEEL_DURATION_TIME*M1_SLOW_LOOP_FREQ/1000.0) 
/* COMPRESSORCURRENTLOOP*/
#define M1_FASTLOOP_FREQ (6250.0F)
#define M1_CURRENT_CLOOP_ATT (1.5F)
#define M1_CURRENT_CLOOP_FREQ (150.0F)
#define M1_CURRENT_CLOOP_LIMIT (0.9F)
#define M1_UDCB_FILTER_FREQ (5.0F)
#define M1_D_KP_GAIN ((2*2*PI*M1_CURRENT_CLOOP_ATT*M1_CURRENT_CLOOP_FREQ*M1_LD)-M1_R) 
#define M1_D_KI_GAIN (2*PI*M1_CURRENT_CLOOP_FREQ)*(2*PI*M1_CURRENT_CLOOP_FREQ)*M1_LD/M1_PWM_FREQ 
#define M1_Q_KP_GAIN ((2*2*PI*M1_CURRENT_CLOOP_ATT*M1_CURRENT_CLOOP_FREQ*M1_LQ)-M1_R) 
#define M1_Q_KI_GAIN (2*PI*M1_CURRENT_CLOOP_FREQ)*(2*PI*M1_CURRENT_CLOOP_FREQ)*M1_LQ/M1_PWM_FREQ 
#define M1_CLOOP_LIMIT (M1_CURRENT_CLOOP_LIMIT/1.732F) 
#define M1_UDCB_IIR_B0 WARP(M1_UDCB_FILTER_FREQ, M1_PWM_FREQ)/(WARP(M1_UDCB_FILTER_FREQ, M1_PWM_FREQ) + 2.0F) 
#define M1_UDCB_IIR_B1 WARP(M1_UDCB_FILTER_FREQ, M1_PWM_FREQ)/(WARP(M1_UDCB_FILTER_FREQ, M1_PWM_FREQ) + 2.0F) 
#define M1_UDCB_IIR_A1 (1.0F - (WARP(M1_UDCB_FILTER_FREQ, M1_PWM_FREQ)/(WARP(M1_UDCB_FILTER_FREQ, M1_PWM_FREQ) + 2.0F)) * 2.0F) 
/* COMPRESSORSPEEDLOOP*/
#define M1_SPEED_PI_PROP_GAIN (0.00966160F)
#define M1_SPEED_PI_INTEG_GAIN (0.000185629F)
#define M1_SLOWLOOP_FREQ (1000.0F)
#define M1_SPEED_LOOP_RAMP_UP (15000.0F)
#define M1_SPEED_LOOP_RAMP_DOWN (80.0F)
#define M1_SPEED_LOOP_HIGH_LIMIT (4.5F)
#define M1_SPEED_LOOP_LOW_LIMIT (-2.0F)
#define M1_SPEED_FILTER_FREQ (600.0F)
#define M1_TO_SPEED_FILTER_FREQ (400.0F)
#define M1_OL_START_SPEED_RAMP (15000.0F)
#define M1_OL_START_I (2.0F)
#define M1_OL_MERGE_SPEED (1500.0F)
#define M1_MERGE_COEFF (100.0F)
#define M1_SPEED_IIR_B0 ((2*PI*M1_SPEED_FILTER_FREQ/M1_PWM_FREQ)/(2*PI*M1_SPEED_FILTER_FREQ/M1_PWM_FREQ + 2.0F)) 
#define M1_SPEED_IIR_B1 ((2*PI*M1_SPEED_FILTER_FREQ/M1_PWM_FREQ)/(2*PI*M1_SPEED_FILTER_FREQ/M1_PWM_FREQ + 2.0F)) 
#define M1_SPEED_IIR_A1 (-(2*PI*M1_SPEED_FILTER_FREQ/M1_PWM_FREQ - 2.0F)/(2*PI*M1_SPEED_FILTER_FREQ/M1_PWM_FREQ + 2.0F)) 
#define M1_TO_SPEED_IIR_B0 WARP(M1_TO_SPEED_FILTER_FREQ, M1_PWM_FREQ)/(WARP(M1_TO_SPEED_FILTER_FREQ, M1_PWM_FREQ) + 2.0F) 
#define M1_TO_SPEED_IIR_B1 WARP(M1_TO_SPEED_FILTER_FREQ, M1_PWM_FREQ)/(WARP(M1_TO_SPEED_FILTER_FREQ, M1_PWM_FREQ) + 2.0F) 
#define M1_TO_SPEED_IIR_A1 (1.0F - (WARP(M1_TO_SPEED_FILTER_FREQ, M1_PWM_FREQ)/(WARP(M1_TO_SPEED_FILTER_FREQ, M1_PWM_FREQ) + 2.0F)) * 2.0F) 
#define M1_SPEED_RAMP_UP (M1_SPEED_LOOP_RAMP_UP/M1_N_ANGULAR_MAX/M1_SLOW_LOOP_FREQ) 
#define M1_SPEED_RAMP_DOWN (M1_SPEED_LOOP_RAMP_DOWN/M1_N_ANGULAR_MAX/M1_SLOW_LOOP_FREQ) 
#define M1_OL_START_RAMP_INC (M1_OL_START_SPEED_RAMP/M1_N_ANGULAR_MAX/M1_PWM_FREQ) 
#define M1_MERG_SPEED_TRH (M1_OL_MERGE_SPEED/M1_N_ANGULAR_MAX) 
#define M1_MERG_COEFF FRAC16((M1_MERGE_COEFF/100.0F)*(M1_OL_MERGE_SPEED/60*M1_MOTOR_PP)/M1_PWM_FREQ) 
/* COMPRESSOROBSERVER*/
#define M1_HYBRID_FLUX_GAIN (4.0F)
#define M1_FLUX_CURRENT_LIMIT (5.0F)
#define M1_FLUX_TO_ATT (0.707F)
#define M1_FLUX_TO_F0 (140.0F)
#define M1_FLUX_TO_KP_GAIN FRAC16(0.942667)
#define M1_FLUX_TO_KP_SHIFT (0)
#define M1_FLUX_TO_KI_GAIN FRAC16(0.750631)
#define M1_FLUX_TO_KI_SHIFT (-3)
#define M1_FLUX_TO_THETA_GAIN FRAC16(0.537600)
#define M1_FLUX_TO_THETA_SHIFT (-3)
#define M1_FLUX_U_SCALE	FRAC32(M1_U_MAX/M1_PWM_FREQ/FLUX_MAX) 
#define M1_FLUX_R_SCALE	FRAC32(M1_R*M1_I_MAX/M1_PWM_FREQ/FLUX_MAX) 
#define M1_FLUX_KALPHA_COEFF FRAC32(M1_HYBRID_FLUX_GAIN*M1_I_MAX/M1_PWM_FREQ/FLUX_MAX) 
#define M1_FLUX_KBETA_COEFF	FRAC32(M1_HYBRID_FLUX_GAIN*M1_I_MAX/M1_PWM_FREQ/FLUX_MAX) 
#define M1_FLUX_LQ_SCALE FRAC32(M1_LQ*M1_I_MAX/FLUX_MAX) 
#define M1_FLUX_LD_NEG_SCALE FRAC32(FLUX_MAX/M1_I_MAX/M1_LD/32.0) 
#define M1_FLUX_LQ_NEG_SCALE FRAC32(FLUX_MAX/M1_I_MAX/M1_LQ/32.0) 
#define M1_FLUX_ROTOR_LD_COEFF FRAC32(M1_FLUX/M1_I_MAX/M1_LD/32.0) 
#define M1_I_THRESHOLD_FLUX_OBSRV FRAC32(M1_FLUX_CURRENT_LIMIT/M1_I_MAX/32.0) 
#define M1_I_THRESHOLD_FLUX_OBSRV_F	FRAC16(M1_FLUX_CURRENT_LIMIT/M1_I_MAX) 
/* COMPRESSORSCALAR*/
#define M1_SCALAR_UQ_MIN (8.0F)
#define M1_ALIGN_VOLTAGE (8.0F)
#define M1_ALIGN_DURATION_TIME (0.1F)
#define M1_FREQ_MAX (M1_SPEED_MAX*M1_MOTOR_PP/60) 
#define M1_SCALAR_VHZ_FACTOR (0.05F)
#define M1_SCALAR_VHZ_FACTOR_GAIN (M1_U_NOM*M1_SCALAR_VHZ_FACTOR/(M1_SPEED_NOM*M1_MOTOR_PP/60.0)) 
#define M1_SCALAR_INTEG_GAIN ACC32(2*M1_MOTOR_PP*M1_SPEED_MAX/60.0/M1_PWM_FREQ) 
#define M1_SCALAR_RAMP_UP (M1_SPEED_LOOP_RAMP_UP/M1_PWM_FREQ/60.0*M1_MOTOR_PP) 
#define M1_SCALAR_RAMP_DOWN (M1_SPEED_LOOP_RAMP_DOWN/M1_PWM_FREQ/60.0*M1_MOTOR_PP) 
#define M1_ALIGN_DURATION (M1_ALIGN_DURATION_TIME*M1_PWM_FREQ) 
/* COMPRESSORFW*/
#define M1_SPEED_FW_ON (2600.0F)
#define M1_PI_FW_P_GAIN (0.5F)
#define M1_PI_FW_I_GAIN (0.1F)
#define M1_PI_FW_OUTPUT_LIMIT (2.0F)
#define M1_FW_FILTER_CUTOFF_FREQ (300.0F)
#define M1_FILTER_FW_B0 WARP(M1_FW_FILTER_CUTOFF_FREQ, M1_SLOW_LOOP_FREQ)/(WARP(M1_FW_FILTER_CUTOFF_FREQ, M1_SLOW_LOOP_FREQ) + 2.0F) 
#define M1_FILTER_FW_B1 WARP(M1_FW_FILTER_CUTOFF_FREQ, M1_SLOW_LOOP_FREQ)/(WARP(M1_FW_FILTER_CUTOFF_FREQ, M1_SLOW_LOOP_FREQ) + 2.0F) 
#define M1_FILTER_FW_A1 (1.0F - (WARP(M1_FW_FILTER_CUTOFF_FREQ, M1_SLOW_LOOP_FREQ)/(WARP(M1_FW_FILTER_CUTOFF_FREQ, M1_SLOW_LOOP_FREQ) + 2.0F)) * 2.0F) 
/* COMPRESSORHC*/
#define M1_HC_SHIFT_TABLE (6)
#define M1_HC_GAIN (0.2F)
#define M1_HC_OUT_LIMIT (0.5F)
#define M1_HC_ZK (12.0F)
#define M1_HC_QZ (0.98F)
#define M1_HC_STORE_LIMIT (10.0F)
#define M1_HC_FILT_PROP (0.5F)
#define M1_HC_SPEED_COEFF (3.75F)
#define M1_HC_NUM_COMP_TABLE (1 << (16 - M1_HC_SHIFT_TABLE)) 
/* COMPRESSORTC*/
#define SHIFT_INDEX_TABLE (8)
#define MAX_LOAD_TORQUE_LDOB (8.0F)
#define KP_LDOB (0.5F)
#define KI_LDOB (5.0F)
#define INERTIA_J_LDOB (0.00029F)
#define MA_POINT_INDEX (5)
#define MAX_IQ_REQ_LDOB (3.0F)
#define FLUX_MAX (1.5F)
#define TORQUE_MAX (1.5*M1_MOTOR_PP*FLUX_MAX*M1_I_MAX) 
#define NUM_TL_TABLE (1 << (16 - SHIFT_INDEX_TABLE)) 
/* FANPARAMETERS*/
#define M2_MOTOR_PP (5)
#define M2_R (35.85F)
#define M2_LD (0.0925F)
#define M2_LQ (0.108F)
#define M2_SPEED_NOM (1100.0F)
#define M2_FLUX (0.248098F)
#define M2_U_NOM (179.0F)
#define M2_I_MAX (1.584F)
#define M2_U_DCB_MAX (443.3F)
#define M2_SPEED_MAX (1500.0F)
#define M2_U_MAX (M2_U_DCB_MAX/1.732F) 
#define M2_N_ANGULAR_MAX (60.0/PI/2/M2_MOTOR_PP) 
#define M2_N_NOM (M2_SPEED_NOM/M2_N_ANGULAR_MAX) 
#define M2_N_MAX (M2_SPEED_MAX/M2_N_ANGULAR_MAX) 
#define M2_FAST_LOOP_FREQ (M2_PWM_FREQ/M2_FOC_FREQ_VS_PWM_FREQ) 
#define M2_SPEED_OVER (1300.0F)
#define M2_SPEED_MIN (150.0F)
#define M2_N_OVERSPEED (M2_SPEED_OVER/M2_N_ANGULAR_MAX) 
#define M2_N_MIN (M2_SPEED_MIN/M2_N_ANGULAR_MAX) 
#define M2_CALIB_DURATION (200)
#define M2_FAULT_DURATION_TIME (3000)
#define M2_FREEWHEEL_DURATION_TIME (1500)
#define M2_FAULT_DURATION (M2_FAULT_DURATION_TIME*M2_SLOW_LOOP_FREQ/1000.0) 
#define M2_FREEWHEEL_DURATION (M2_FREEWHEEL_DURATION_TIME*M2_SLOW_LOOP_FREQ/1000.0) 
/* FANCLOOP*/
#define M2_FASTLOOP_FREQ (8000.0F)
#define M2_CURRENT_CLOOP_ATT (1.0F)
#define M2_CURRENT_CLOOP_FREQ (150.0F)
#define M2_CURRENT_CLOOP_LIMIT (0.9F)
#define M2_UDCB_FILTER_FREQ (5.0F)
#define M2_UDCB_IIR_B0 WARP(M2_UDCB_FILTER_FREQ, M2_FAST_LOOP_FREQ)/(WARP(M2_UDCB_FILTER_FREQ, M2_FAST_LOOP_FREQ) + 2.0F) 
#define M2_UDCB_IIR_B1 WARP(M2_UDCB_FILTER_FREQ, M2_FAST_LOOP_FREQ)/(WARP(M2_UDCB_FILTER_FREQ, M2_FAST_LOOP_FREQ) + 2.0F) 
#define M2_UDCB_IIR_A1 (1.0F - (WARP(M2_UDCB_FILTER_FREQ, M2_FAST_LOOP_FREQ)/(WARP(M2_UDCB_FILTER_FREQ, M2_FAST_LOOP_FREQ) + 2.0F)) * 2.0F) 
#define M2_D_KP_GAIN ((2*2*PI*M2_CURRENT_CLOOP_ATT*M2_CURRENT_CLOOP_FREQ*M2_LD)-M2_R) 
#define M2_D_KI_GAIN (2*PI*M2_CURRENT_CLOOP_FREQ)*(2*PI*M2_CURRENT_CLOOP_FREQ)*M2_LD/M2_FAST_LOOP_FREQ 
#define M2_Q_KP_GAIN ((2*2*PI*M2_CURRENT_CLOOP_ATT*M2_CURRENT_CLOOP_FREQ*M2_LQ)-M2_R) 
#define M2_Q_KI_GAIN (2*PI*M2_CURRENT_CLOOP_FREQ)*(2*PI*M2_CURRENT_CLOOP_FREQ)*M2_LQ/M2_FAST_LOOP_FREQ 
#define M2_CLOOP_LIMIT (M2_CURRENT_CLOOP_LIMIT/1.732F) 
/* FANSPEEDLOOP*/
#define M2_SLOWLOOP_FREQ (1000.0F)
#define M2_SPEED_PI_PROP_GAIN (0.36837F)
#define M2_SPEED_PI_INTEG_GAIN (0.00000192878F)
#define M2_SPEED_LOOP_RAMP_UP (400.0F)
#define M2_SPEED_LOOP_RAMP_DOWN (100.0F)
#define M2_SPEED_LOOP_HIGH_LIMIT (0.5F)
#define M2_SPEED_LOOP_LOW_LIMIT (-0.3F)
#define M2_SPEED_FILTER_FREQ (20.0F)
#define M2_SPEED_IIR_B0 WARP(M2_SPEED_FILTER_FREQ, M2_FAST_LOOP_FREQ)/(WARP(M2_SPEED_FILTER_FREQ, M2_FAST_LOOP_FREQ) + 2.0F) 
#define M2_SPEED_IIR_B1 WARP(M2_SPEED_FILTER_FREQ, M2_FAST_LOOP_FREQ)/(WARP(M2_SPEED_FILTER_FREQ, M2_FAST_LOOP_FREQ) + 2.0F) 
#define M2_SPEED_IIR_A1 (1.0F - (WARP(M2_SPEED_FILTER_FREQ, M2_FAST_LOOP_FREQ)/(WARP(M2_SPEED_FILTER_FREQ, M2_FAST_LOOP_FREQ) + 2.0F)) * 2.0F) 
#define M2_SPEED_RAMP_UP (M2_SPEED_LOOP_RAMP_UP/M2_N_ANGULAR_MAX/M2_SLOW_LOOP_FREQ) 
#define M2_SPEED_RAMP_DOWN (M2_SPEED_LOOP_RAMP_DOWN/M2_N_ANGULAR_MAX/M2_SLOW_LOOP_FREQ) 
#define M2_TO_SPEED_FILTER_FREQ (100.0F)
#define M2_TO_SPEED_IIR_B0 WARP(M2_TO_SPEED_FILTER_FREQ, M2_FAST_LOOP_FREQ)/(WARP(M2_TO_SPEED_FILTER_FREQ, M2_FAST_LOOP_FREQ) + 2.0F) 
#define M2_TO_SPEED_IIR_B1 WARP(M2_TO_SPEED_FILTER_FREQ, M2_FAST_LOOP_FREQ)/(WARP(M2_TO_SPEED_FILTER_FREQ, M2_FAST_LOOP_FREQ) + 2.0F) 
#define M2_TO_SPEED_IIR_A1 (1.0F - (WARP(M2_TO_SPEED_FILTER_FREQ, M2_FAST_LOOP_FREQ)/(WARP(M2_TO_SPEED_FILTER_FREQ, M2_FAST_LOOP_FREQ) + 2.0F)) * 2.0F) 
/* FANSCALAR*/
#define M2_SCALAR_UQ_MIN (5.0F)
#define M2_ALIGN_VOLTAGE (20.0F)
#define M2_ALIGN_DURATION_TIME (2.0F)
#define M2_FREQ_MAX (M2_SPEED_MAX*M2_MOTOR_PP/60) 
#define M2_SCALAR_VHZ_FACTOR (1.0F)
#define M2_SCALAR_VHZ_FACTOR_GAIN (M2_U_NOM*M2_SCALAR_VHZ_FACTOR/(M2_SPEED_NOM*M2_MOTOR_PP/60.0)) 
#define M2_SCALAR_INTEG_GAIN ACC32(2*M2_MOTOR_PP*M2_SPEED_MAX/60.0/M2_FAST_LOOP_FREQ) 
#define M2_SCALAR_RAMP_UP (M2_SPEED_LOOP_RAMP_UP/M2_FAST_LOOP_FREQ/60.0*M2_MOTOR_PP) 
#define M2_SCALAR_RAMP_DOWN (M2_SPEED_LOOP_RAMP_DOWN/M2_FAST_LOOP_FREQ/60.0*M2_MOTOR_PP) 
#define M2_ALIGN_DURATION (M2_ALIGN_DURATION_TIME*M2_FAST_LOOP_FREQ) 
#define M2_OL_START_I (0.2F)
#define M2_OL_MERGE_SPEED (200.0F)
#define M2_OL_START_RAMP_INC (M2_OL_START_SPEED_RAMP/M2_N_ANGULAR_MAX/M2_FAST_LOOP_FREQ) 
#define M2_MERG_SPEED_TRH (M2_OL_MERGE_SPEED/M2_N_ANGULAR_MAX) 
/* FANOBSERVER*/
#define M2_FLUX_TO_ATT (0.707F)
#define M2_FLUX_TO_F0 (60.0F)
#define M2_FLUX_GAMMA (1.5F)
#define M2_FLUX_TO_KP_GAIN FRAC16(0.678720)
#define M2_FLUX_TO_KP_SHIFT (0)
#define M2_FLUX_TO_KI_GAIN FRAC16(0.723823)
#define M2_FLUX_TO_KI_SHIFT (-5)
#define M2_FLUX_TO_THETA_GAIN FRAC16(1.00000)
#define M2_FLUX_TO_THETA_SHIFT (-5)
/* SENSORS*/
#define M1_POSPE_ENC_PULSES 
#define M1_POSPE_ENC_DIRECTION 
#define M1_POSPE_ENC_N_MIN 
#define M1_POSPE_MECH_POS_GAIN 
#define M1_POS_P_PROP_GAIN 
#define M1_POSPE_TO_KP_GAIN 
#define M1_POSPE_TO_KI_GAIN 
#define M1_POSPE_TO_THETA_GAIN 
/* USER INPUT START */
/* USER INPUT END */
#endif /* __MC_PARAMETERS_H_ */
