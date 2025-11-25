/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
 
 /*
 * TLOnlinePITuning.c
 *
 *  Created on: Dec 9, 2014
 *      Author: B45091
 */
#include "TLOnlinePITuning.h"
#include "mc_periph_init.h"

/*function definition*/
void TLOnlineTuningInit(TLONLINE_T *ptr)
{
	/* proportional parameters */
	ptr->w16KpGain = (int16_t)(KP_LDOB*(FRAC16(1.0)>>3));/* Q12;[-8,7.9] */
	ptr->w32KpCof = (int32_t)((M1_N_MAX/M1_MOTOR_PP*1.0L)/TORQUE_MAX*(FRAC32(1.0)>>7));

	ptr->iKpGainShift = 10; /* 31 - (Q_KpGain + QKpCof - 15) */

	/* integral parameters */
	ptr->w16KiGain = (FRAC16(1.0)>>9)* KI_LDOB*1.0L;//Q6 [-2^9,512]

	/* wb/Tb*Period */
	ptr->w32KiCof = FRAC32((M1_N_MAX/M1_MOTOR_PP*1.0L)/TORQUE_MAX/M1_PWM_FREQ);//Q31;  0.0011

	/* inertia parameters */
	ptr->w32TorqueIntegCof = (int32_t)((1/INERTIA_J_LDOB*TORQUE_MAX/(M1_N_MAX/M1_MOTOR_PP*1.0L)/M1_PWM_FREQ)*FRAC32(1.0));//Q31;

	ptr->w16SpeedTLObser = 0;
	ptr->w32KpTLObser = 0;
	ptr->w32KiTLObser = 0;
	ptr->w32KiTLObserOld = 0;
	ptr->w32SpeedTLObser = 0;
	ptr->w32SpeedTLObserOld = 0;
	ptr->w16SpeedTLObser = 0;

	ptr->w16EnableFeedForwCompFlag = 0;

	ptr->f16EntryFeedForwCompRatio = 0;
	
	ptr->w16CofKt = FRAC16(1.5*M1_MOTOR_PP*0.107*M1_I_MAX/TORQUE_MAX);

	ptr->f16RatioTest = FRAC16(0.0);
	
	ptr->uw16IndexOldUpdateFlag = 0;
	ptr->uw16PosIndexOld = 0;
        ptr->f32MaxLoadTorque = FRAC32(MAX_LOAD_TORQUE_LDOB/TORQUE_MAX);
        
        ptr->f16TorqueCompLim = FRAC16(MAX_IQ_REQ_LDOB/M1_I_MAX);
}

void TLOnlineTuningInitRep(TLONLINE_T *ptr)
{
	ptr->uw16UpdateMATLFlag = 1;
	ptr->w16SpeedTLObser = 0;
	ptr->w32KpTLObser = 0;
	ptr->w32KiTLObser = 0;
	ptr->w32KiTLObserOld = 0;
	ptr->w32SpeedTLObser = 0;
	ptr->w32SpeedTLObserOld = 0;
	ptr->w16SpeedTLObser = 0;
	
	ptr->w16EnableFeedForwCompFlag = 0;

	ptr->f16EntryFeedForwCompRatio = 0;
	ptr->f16ExitFeedForwCompRatio = 0;//FRAC16(0.99/1); /* if 1, the compensation current will have non-zero value at the beginning */
	
	ptr->sIDQReqTLObser.f16D = 0;
	ptr->sIDQReqTLObser.f16Q = 0;
	
	ptr->uw16CnterDelayEnter = 0;
	ptr->uw16CnterDelayExit = 0;
	
	ptr->uw16IndexOldUpdateFlag = 0;
	ptr->uw16PosIndexOld = 0;
}

void MechAngleCalcInit(MECH_POSITION_CALC_T *ptr)
{
	ptr->f16ElePositionNew = 0;
	ptr->f16ElePositionOld = 0;
	ptr->f16MechPosition = 0;
	ptr->uw16PairOfPoles = (uint16_t)M1_MOTOR_PP;
	ptr->uw16ElePositionCycleCntr = 0;
        ptr->f16ShiftIndexTable = SHIFT_INDEX_TABLE;
        ptr->f16NumTLTable = NUM_TL_TABLE;
}

/* mechanical angle calculation for LODB */
void MechAngleCalcUpdate(MECH_POSITION_CALC_T *ptr)
{
	frac16_t f16TempCompensation;
	frac32_t f32Temp, f32PositionTempNew, f32PositionTempOld;
	
	f32PositionTempNew = (frac32_t)ptr->f16ElePositionNew;
	f32PositionTempOld = (frac32_t)ptr->f16ElePositionOld;

	f32Temp = MLIB_Sub_F32(f32PositionTempOld, f32PositionTempNew);
	/* ex: 32767 - (-32768) > 32768
	 * ex: 32767- 32765  < 32768
	 * ex: -32765 - (-32767) < 32768 */
	if (f32Temp > 32768 )
	{
		ptr->uw16ElePositionCycleCntr++;
	}
	
	if (ptr->uw16ElePositionCycleCntr >= ptr->uw16PairOfPoles)
	{
		ptr->uw16ElePositionCycleCntr = 0;
	}
	
	if (0 == ptr->uw16ElePositionCycleCntr)
	{
		f16TempCompensation = 0;
		ptr->f16MechPosition = MLIB_Add_F16(MLIB_Mul_F16(ptr->f16ElePositionNew, FRAC16(1.0/3)), f16TempCompensation);
	}
	else if (1 == ptr->uw16ElePositionCycleCntr)
	{
		f16TempCompensation = FRAC16((120.0)/180.0);
		ptr->f16MechPosition = MLIB_Add_F16(MLIB_Mul_F16(ptr->f16ElePositionNew, FRAC16(1.0/3)), f16TempCompensation);
	}
	else if (2 == ptr->uw16ElePositionCycleCntr)
	{
		f16TempCompensation = MLIB_Add_F16(FRAC16((120.0)/180.0),FRAC16((120.0)/180.0));
		ptr->f16MechPosition = MLIB_Add_F16(MLIB_Mul_F16(ptr->f16ElePositionNew, FRAC16(1.0/3)), f16TempCompensation);
	}
	
}
void MAFilterTLInit(TLONLINE_T *ptr)
{
    for (ptr->uw16PosIndex=0; ptr->uw16PosIndex<NUM_TL_TABLE; ptr->uw16PosIndex++)
    {
            ptr->sMAFilter[ptr->uw16PosIndex].u16Sh = MA_POINT_INDEX;
            ptr->sMAFilter[ptr->uw16PosIndex].a32Acc = 0;
            GDFLIB_FilterMAInit_F16(0, &ptr->sMAFilter[ptr->uw16PosIndex]);
            
            ptr->w32MATL[ptr->uw16PosIndex] = 0;
            ptr->f16MATL[ptr->uw16PosIndex] = 0;
            ptr->f16TL[ptr->uw16PosIndex] = 0;
    }
}

void EntryFeedForwardCompensation(TLONLINE_T *ptr)
{
	frac16_t f16UpperLimit;
        frac16_t f16LowerLimit;

	/* define max and min value */
	f16UpperLimit = FRAC16(0.99/1);
	f16LowerLimit = 0;

	if (++ptr->uw16CnterDelayEnter >= 2)
	{
		ptr->uw16CnterDelayEnter = 0;
		/* ramp decrease ratio */
		ptr->f16EntryFeedForwCompRatio = GFLIB_Limit_F16(MLIB_Add_F16(ptr->f16EntryFeedForwCompRatio,2), f16LowerLimit, f16UpperLimit);
	}
	
	/* ramp decrease load torque compensation */
	ptr->w16LoadTLEstRamp = MLIB_Mul_F16(ptr->f16MATL[ptr->uw16PosIndex], ptr->f16EntryFeedForwCompRatio);

	/* ready for ExitFeedForwardCompensation */
	ptr->f16ExitFeedForwCompRatio = ptr->f16EntryFeedForwCompRatio;//FRAC16(0.99/1);
}

void ExitFeedForwardCompensation(TLONLINE_T *ptr)
{
	frac16_t f16UpperLimit;
        frac16_t f16LowerLimit;

	/* define max and min value */
	f16UpperLimit = FRAC16(0.99/1);
	f16LowerLimit = 0;

	if (++ptr->uw16CnterDelayExit >= 2)
	{
		ptr->uw16CnterDelayExit = 0;
		/* ramp decrease ratio */
		ptr->f16ExitFeedForwCompRatio = GFLIB_Limit_F16(MLIB_Sub_F16(ptr->f16ExitFeedForwCompRatio,2), f16LowerLimit, f16UpperLimit);
	}

	/* ramp decrease load torque compensation */
	ptr->w16LoadTLEstRamp = MLIB_Mul_F16(ptr->f16MATL[ptr->uw16PosIndex], ptr->f16ExitFeedForwCompRatio);
	
	/* ready for EntryFeedForwardCompensation */
	ptr->f16EntryFeedForwCompRatio = ptr->f16ExitFeedForwCompRatio;//0;
}

void TLOnlineCompenStateMachine(TLONLINE_T *ptr)
{
	frac16_t f16Temp,f16Temp1,f16TempNumerator,f16TempDenominator;

	//enable the torque compensation when speed over 100 rpm
	if (ptr->f16SpeedRamp > FRAC16(100.0/(M1_N_MAX*M1_N_ANGULAR_MAX)))
	{
		/* feed forward compensation */
		if (1 == ptr->w16EnableFeedForwCompFlag)
		{
			EntryFeedForwardCompensation(ptr);
		}
		else if (0 == ptr->w16EnableFeedForwCompFlag)
		{
			ExitFeedForwardCompensation(ptr);
		}
//		if (ptr->f16SpeedRamp > FRAC16(2700.0/(M1_N_MAX*M1_N_ANGULAR_MAX)))
//		{
//			ptr->w16EnableFeedForwCompFlag = 0;
//		}
//		else if (ptr->f16SpeedRamp > FRAC16(100.0/(M1_N_MAX*M1_N_ANGULAR_MAX)))
//		{
//			ptr->w16EnableFeedForwCompFlag = 1;
//		}
//		else
//		{
//			ptr->w16EnableFeedForwCompFlag = 0;
//		}
	}
	/* Here torque current limit must be noticed and FW speed limit must be considered after compensation
	 * get torque current according to the above load torque
	 * iq = ( Te + flux_q * id ) / flux_d */
    if ( (0 == ptr->w16EnableFeedForwCompFlag)&&(0 == ptr->f16ExitFeedForwCompRatio) )
    {
    	ptr->sIDQReqTLObser.f16Q = 0;
    }
    else
    {
    	//Set different torque compensation ratio based on different speed
        if ((ptr->f16SpeedRamp >=FRAC16(0.0/(M1_N_MAX*M1_N_ANGULAR_MAX)))&&(ptr->f16SpeedRamp <= FRAC16(500.0/(M1_N_MAX*M1_N_ANGULAR_MAX))))
        {
                f16TempNumerator = MLIB_Sub_F16(FRAC16(500.0/(M1_N_MAX*M1_N_ANGULAR_MAX)),ptr->f16SpeedRamp);
                f16TempDenominator = MLIB_Sub_F16(FRAC16(500.0/(M1_N_MAX*M1_N_ANGULAR_MAX)),FRAC16(0.0/(M1_N_MAX*M1_N_ANGULAR_MAX)));
                f16Temp = MLIB_Div_F16(f16TempNumerator, f16TempDenominator);
                f16Temp1 = MLIB_Sub_F16(FRAC16(0.45),FRAC16(0.4));//Q15  
                f16Temp = MLIB_Mul_F16(f16Temp,f16Temp1);//Q15
                ptr->f16RatioTest = MLIB_Add_F16(FRAC16(0.4),f16Temp);//Q15	
        }
        else if((ptr->f16SpeedRamp >=FRAC16(500.0/(M1_N_MAX*M1_N_ANGULAR_MAX)))&&(ptr->f16SpeedRamp <= FRAC16(1000.0/(M1_N_MAX*M1_N_ANGULAR_MAX))))
        {
                ptr->f16RatioTest = FRAC16(0.4);
        }
        else if ((ptr->f16SpeedRamp >=FRAC16(1000.0/(M1_N_MAX*M1_N_ANGULAR_MAX)))&&(ptr->f16SpeedRamp <= FRAC16(2000.0/(M1_N_MAX*M1_N_ANGULAR_MAX))))
        {
                f16TempNumerator = MLIB_Sub_F16(FRAC16(2000.0/(M1_N_MAX*M1_N_ANGULAR_MAX)),ptr->f16SpeedRamp);
                f16TempDenominator = MLIB_Sub_F16(FRAC16(2000.0/(M1_N_MAX*M1_N_ANGULAR_MAX)),FRAC16(1000.0/(M1_N_MAX*M1_N_ANGULAR_MAX)));
                f16Temp = MLIB_Div_F16(f16TempNumerator, f16TempDenominator);
                f16Temp1 = MLIB_Sub_F16(FRAC16(0.4),FRAC16(0.23));//Q15  
                f16Temp = MLIB_Mul_F16(f16Temp,f16Temp1);//Q15
                ptr->f16RatioTest = MLIB_Add_F16(FRAC16(0.23),f16Temp);//Q15	
        }
        else if ((ptr->f16SpeedRamp >=FRAC16(2000.0/(M1_N_MAX*M1_N_ANGULAR_MAX)))&&(ptr->f16SpeedRamp <= FRAC16(2400.0/(M1_N_MAX*M1_N_ANGULAR_MAX))))
        {
                f16TempNumerator = MLIB_Sub_F16(FRAC16(2400.0/(M1_N_MAX*M1_N_ANGULAR_MAX)),ptr->f16SpeedRamp);
                f16TempDenominator = MLIB_Sub_F16(FRAC16(2400.0/(M1_N_MAX*M1_N_ANGULAR_MAX)),FRAC16(2000.0/(M1_N_MAX*M1_N_ANGULAR_MAX)));
                f16Temp = MLIB_Div_F16(f16TempNumerator, f16TempDenominator);
                f16Temp1 = MLIB_Sub_F16(FRAC16(0.23),FRAC16(0.18));//Q15  
                f16Temp = MLIB_Mul_F16(f16Temp,f16Temp1);//Q15
                ptr->f16RatioTest = MLIB_Add_F16(FRAC16(0.18),f16Temp);//Q15	
        }
        else if ((ptr->f16SpeedRamp >=FRAC16(2400.0/(M1_N_MAX*M1_N_ANGULAR_MAX)))&&(ptr->f16SpeedRamp <= FRAC16(2600.0/(M1_N_MAX*M1_N_ANGULAR_MAX))))
        {
                f16TempNumerator = MLIB_Sub_F16(FRAC16(2600.0/(M1_N_MAX*M1_N_ANGULAR_MAX)),ptr->f16SpeedRamp);
                f16TempDenominator = MLIB_Sub_F16(FRAC16(2600.0/(M1_N_MAX*M1_N_ANGULAR_MAX)),FRAC16(2400.0/(M1_N_MAX*M1_N_ANGULAR_MAX)));
                f16Temp = MLIB_Div_F16(f16TempNumerator, f16TempDenominator);
                f16Temp1 = MLIB_Sub_F16(FRAC16(0.18),FRAC16(0.13));//Q15  
                f16Temp = MLIB_Mul_F16(f16Temp,f16Temp1);//Q15
                ptr->f16RatioTest = MLIB_Add_F16(FRAC16(0.13),f16Temp);//Q15	
        }
        else if ((ptr->f16SpeedRamp >=FRAC16(2600.0/(M1_N_MAX*M1_N_ANGULAR_MAX)))&&(ptr->f16SpeedRamp <= FRAC16(2800.0/(M1_N_MAX*M1_N_ANGULAR_MAX))))
    	{
     		f16TempNumerator = MLIB_Sub_F16(FRAC16(2800.0/(M1_N_MAX*M1_N_ANGULAR_MAX)),ptr->f16SpeedRamp);
     		f16TempDenominator = MLIB_Sub_F16(FRAC16(2800.0/(M1_N_MAX*M1_N_ANGULAR_MAX)),FRAC16(2600.0/(M1_N_MAX*M1_N_ANGULAR_MAX)));
     		f16Temp = MLIB_Div_F16(f16TempNumerator, f16TempDenominator);
     		ptr->f16RatioTest = MLIB_Mul_F16(f16Temp,FRAC16(0.13));//Q15	
    	}
        else
        {
                ptr->f16RatioTest = 0;
        }
    	
        /* Torque = Kt * iq */	
    	ptr->sIDQReqTLObser.f16Q = MLIB_Mul_F16(MLIB_DivSat_F16(ptr->w16LoadTLEstRamp, ptr->w16CofKt), ptr->f16RatioTest);

    	if (ptr->sIDQReqTLObser.f16Q > ptr->f16TorqueCompLim)
    	{
    		ptr->sIDQReqTLObser.f16Q = ptr->f16TorqueCompLim;
    	}
    	else if (ptr->sIDQReqTLObser.f16Q < -ptr->f16TorqueCompLim)
    	{
    		ptr->sIDQReqTLObser.f16Q = -ptr->f16TorqueCompLim;
    	}
    }
}
