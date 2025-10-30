/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
 
#include "mlib.h"
#include "TLOnlinePITuning.h"

typedef struct
{
    uint16_t uw16PosIndex;
    float_t fltCurrentErr;
    frac16_t Rep_Zk;
    frac16_t f16Num_Table;
    frac16_t f16Shift_Table;
    float_t RepKr;
    float_t ReqQz;
    float_t fltFiltProp;
    float_t flt_Comp_Output;
    float_t RpIdOut[M1_HC_NUM_COMP_TABLE];

    float_t fltTempLAx;
    float_t fltStoreLimit;
    float_t fltOutLimit;
    frac16_t f16Speed;
    frac16_t f16SpeedCoeff;
}HARMONIC_COMPENSATION;

extern float_t Harmonic_Compensation(MECH_POSITION_CALC_T *pMecPosition, HARMONIC_COMPENSATION *ptr);