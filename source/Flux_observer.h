/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef FLUX_OBSERVER_H_
#define FLUX_OBSERVER_H_

/* Data type definition */
typedef struct
{
	frac32_t f32U_Scale;		// Umax*Ts/flux_max
	frac32_t f32R_Scale;		// Imax*Rs*Ts/flux_max
	frac32_t f32K_Alpha_Coeff;// Kalpha*Imax*Ts/flux_max
	frac32_t f32K_Beta_Coeff; // Kbeta*Imax*Ts/flux_max
	frac32_t f32Lq_Scale;		// Lq*Imax/flux_max
	long i32Ld_Neg_Scale;	// Q6.26, flux_max/(Imax*Ld)
	long i32Lq_Neg_Scale;	// Q6.26, flux_max/(Imax*Lq)
	long i32Rotor_Ld_Coeff;	// Q6.26, rotor_flux/(Imax*Ld)
	long i32I_Threshold;	// Q6.26
	frac16_t f16I_Threshold;  // Q1.15
} COEFF_T;

typedef struct
{
	frac32_t	f32Theta;
	frac32_t	f32Speed;
	frac32_t	f32I_1;
	frac16_t	f16IntegGain;
	frac16_t	i16IntegGainShift;
	frac16_t	f16PropGain;
	frac16_t	i16PropGainShift;
	frac16_t	f16ThGain;
	frac16_t	i16ThGainShift;
	
} TRACK_OBSRV_T;

typedef struct FLUX_OBSRV_T FLUX_OBSRV_T;
struct FLUX_OBSRV_T
{
	frac32_t f32FluxAlphaEstim;		// estimated alpha flux
	frac32_t f32FluxBetaEstim;		// estimated beta flux
	frac32_t f32FluxAlphaEstim_1;		// estimated alpha flux at last step
	frac32_t f32FluxBetaEstim_1;		// estimated beta flux at last step
	frac32_t f32FluxAlphaExtEstim;	// estimated alpha extended flux
	frac32_t f32FluxBetaExtEstim;		// estimated beta extended flux
	frac32_t f32BemfAlpha_1;			// alpha BEMF of last step
	frac32_t f32BemfBeta_1;			// beta BEMF of last step
	
	// voltage model for flux calculation
	frac16_t f16UAlpha;				// alpha voltage 
	frac16_t f16UBeta;				// beta voltage
	frac16_t f16IAlpha;
	frac16_t f16IBeta;
	
	frac16_t f16Cos_ref;				// come from estimated alpha/beta extended flux
	frac16_t f16Sin_ref;				// come from estimated alpha/beta extended flux
	frac16_t f16Sin_estim;			// come from estimated extended flux position
	frac16_t f16Cos_estim;			// come from estimated extended flux position
	frac16_t f16PosErr;				// position error between estimated extended flux position and the real one
	
	AMCLIB_TRACK_OBSRV_T_F32 acTo;
	frac16_t f16SpeedEstim;			// estimated speed of extended flux
	frac16_t f16PosEstim;				// estimated position of extended flux
	
	// current model for flux calculation
	frac32_t f32FluxDEstim;		    // estimated d-axis flux from alpha/beta flux
	frac32_t f32FluxQEstim;			// estimated q-axis flux from alpha/beta flux
	frac16_t f16IdEstim;				// estimated d-axis current from d/q estimated flux
	frac16_t f16IqEstim;				// estimated q-axis current from d/q estimated flux
	frac16_t f16IAlphaEstim;			// estimated alpha current from estimated d/q current
	frac16_t f16IBetaEstim;			// estimated beta current from estimated d/q current
	frac16_t f16IAlphaErr;			// error between estimated alpha current and real alpha current
	frac16_t f16IBetaErr;				// error between estimated beta current and real beta current
	
	frac16_t f16Te;
	// Coefficients
	COEFF_T sCoeff;
	
	void (*Update)(FLUX_OBSRV_T *ptr);
	void (*Init)(FLUX_OBSRV_T *ptr); 	
};

extern void Init_FLUX_OBSRV_T(FLUX_OBSRV_T *ptr);

#endif /* FLUX_OBSERVER_H_ */