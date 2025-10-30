/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
 
#ifndef _FLUX_H_
#define _FLUX_H_
#include "mlib_types.h"
#include "amclib.h"
#include "gflib_FP.h"
#include "mlib_FP.h"

typedef struct 
{
    float x1;
    float x2;
    float lambda_est;
    float i_alpha_last;
    float i_beta_last;
} observer_state;

typedef struct Flux_Obs_t_a1
{
    //Input parameters
    GMCLIB_2COOR_ALBE_T_FLT sIAlBe;
    GMCLIB_2COOR_ALBE_T_FLT sUAlBe;
    float Id;
    
    //Motor paraemters
    float R;
    float R_Origin;
    float L;
    float L_Origin;
    float Lambda;
    float Lambda_Origin;
    float L_diff;
    
    float dt;
    float L_ia;
    float L_ib;
    float R_ia;
    float R_ib;
    float fltBEMF_Alpha;
    float fltBEMF_Beta;
    float Gamma;
    float Gamma_half;
    float Lambda_est;
    float I_abs_filter;
    float fltPositionEst;
    float mag;
    float x1_dot;
    float x2_dot;
    float fluxErr;
    
    frac16_t f16PositionEst;
    observer_state state;
    AMCLIB_TRACK_OBSRV_T_F32 sTo;
    frac16_t f16SpdEstimOTF;
    frac16_t f16ToErr;
} Flux_Obs_t;
extern void Flux_Init(Flux_Obs_t *ptr);
extern void Flux_update(Flux_Obs_t *ptr);
extern void Flux_clear_variables(Flux_Obs_t *ptr);
extern Flux_Obs_t Flux_Obs;
#endif /* FLUX_H_ */