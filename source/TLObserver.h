/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
 
#ifndef TLOBSERVER_H_
#define TLOBSERVER_H_

#include "mlib_types.h"
#include "gmclib_types.h"
#include "gdflib.h"
#include "MC_Parameters.h"

typedef struct
{
	frac16_t f16ElePositionNew;
	frac16_t f16ElePositionOld;
	frac16_t f16MechPosition;
	uint16_t uw16ElePositionCycleCntr;
	uint16_t uw16PairOfPoles;
        frac16_t f16ShiftIndexTable;
        frac16_t f16NumTLTable;
}MECH_POSITION_CALC_T;

typedef struct
{
    int16_t w16SpeedRef;
    int16_t w16SpeedTLObser;

    int16_t w16KpGain;
    int32_t w32KpCof;
    int16_t iKpGainShift;
    int32_t w32KpTLObser;
    
    int16_t w16KiGain;
    int32_t w32KiCof;
    int32_t w32KiTLObser;
    int32_t w32KiTLObserOld;

    int32_t w32PiSumTLObser;
    int32_t w32LoadTorque;
    int16_t w16LoadTLEst;
    int16_t w16TeEl;

    int32_t w32TorqueIntegCof;

    int32_t w32SpeedTLObser;
    int32_t w32SpeedTLObserOld;

    int16_t w16EnableFeedForwCompFlag;
    frac16_t f16EntryFeedForwCompRatio;
    frac16_t f16ExitFeedForwCompRatio;
    int16_t w16LoadTLEstRamp;
    frac16_t f16SpeedRamp;
    GMCLIB_2COOR_DQ_T_F16 sFluxDQTLObser;
    GMCLIB_2COOR_DQ_T_F16 sIDQReqTLObser;
    /* MA filter */
    uint16_t uw16PosIndex;
    frac32_t f32MaxLoadTorque;

    frac16_t f16MATL[NUM_TL_TABLE];
    frac16_t f16TL[NUM_TL_TABLE];
    int32_t w32MATL[NUM_TL_TABLE];
    GDFLIB_FILTER_MA_T_A32 sMAFilter[NUM_TL_TABLE];
    frac16_t f16MATLTable;
    int16_t w16CofKt;
    frac16_t f16RatioTest;
    uint16_t uw16UpdateMATLFlag;
    frac16_t f16TorqueCompLim;
    
    uint16_t uw16CnterDelayEnter;
    uint16_t uw16CnterDelayExit;
    
    uint16_t uw16IndexOldUpdateFlag;
    uint16_t uw16PosIndexOld;
}TLONLINE_T;

extern TLONLINE_T sTLObser;
extern void TLOnlineTuningUpdate(TLONLINE_T *ptr);
extern void MAFilterTLUpdate(TLONLINE_T *ptr, MECH_POSITION_CALC_T *pMecPosition);
#endif /* TLOBSERVER_H_ */
