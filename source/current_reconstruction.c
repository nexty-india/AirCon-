/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
 
#include "current_reconstruction.h"
#include "mlib.h"

SVM_T1_T2 t1_t2;
PHASE_CURRENTS phaseCurrents;
ONE_SHUNT_TRG_T mcTrigPointCompressor;

void PhaseShift_calculation(PHASE_CURRENTS *p_phaseCurrents,SVM_T1_T2 *p_t12,frac16_t svm_sector)
{
    p_phaseCurrents->pwm_delay[0] = 0;
    p_phaseCurrents->pwm_delay[1] = 0;
    p_phaseCurrents->pwm_delay[2] = 0;
    p_phaseCurrents->pwm_delay[3] = 0;
    p_phaseCurrents->pwm_delay[4] = 0;
    p_phaseCurrents->pwm_delay[5] = 0;	
    /*asymmetric PWM for t1*/
    if(p_phaseCurrents->critical_sector > p_t12->t_1)
    {
        p_phaseCurrents->t_delay1 = p_phaseCurrents->critical_sector - p_t12->t_1;
        p_t12->t_1 = p_phaseCurrents->critical_sector;
        p_phaseCurrents->shiftFlag1 = 1;
        switch(svm_sector)
        {
        case 1:
        case 2:
                p_phaseCurrents->pwm_delay[4] = -p_phaseCurrents->t_delay1; // phase shift on C
                p_phaseCurrents->pwm_delay[5] =  p_phaseCurrents->t_delay1;
                break;

        case 3:
        case 4:
                p_phaseCurrents->pwm_delay[0] = -p_phaseCurrents->t_delay1; // phase shift on A
                p_phaseCurrents->pwm_delay[1] =  p_phaseCurrents->t_delay1;
                break;

        case 5:
        case 6:
                p_phaseCurrents->pwm_delay[2] = -p_phaseCurrents->t_delay1; // phase shift on B
                p_phaseCurrents->pwm_delay[3] =  p_phaseCurrents->t_delay1;
                break;

        default :
                p_phaseCurrents->pwm_delay[0] = 0;
                p_phaseCurrents->pwm_delay[1] = 0;
                p_phaseCurrents->pwm_delay[2] = 0;
                p_phaseCurrents->pwm_delay[3] = 0;
                p_phaseCurrents->pwm_delay[4] = 0;
                p_phaseCurrents->pwm_delay[5] = 0;
                break;
        }
    }
    else
    {
            p_phaseCurrents->t_delay1 = 0;
            p_phaseCurrents->shiftFlag1 = 0;
    }

    /* asymmetric pwm shifting for time t2 */
    if(p_phaseCurrents->critical_sector > p_t12->t_2)
    {

            p_phaseCurrents->t_delay2 =  p_phaseCurrents->critical_sector - p_t12->t_2;
            p_t12->t_2 = p_phaseCurrents->critical_sector;
            p_phaseCurrents->shiftFlag2 = 1;
            switch(svm_sector)
            {
            case 1:
            case 6:
                    p_phaseCurrents->pwm_delay[0] =  p_phaseCurrents->t_delay2; // phase shift on A
                    p_phaseCurrents->pwm_delay[1] = -p_phaseCurrents->t_delay2;
                    break;

            case 2:
            case 3:
                    p_phaseCurrents->pwm_delay[2] =  p_phaseCurrents->t_delay2; // phase shift on B
                    p_phaseCurrents->pwm_delay[3] = -p_phaseCurrents->t_delay2;
                    break;

            case 4:
            case 5:
                    p_phaseCurrents->pwm_delay[4] =  p_phaseCurrents->t_delay2; // phase shift on C
                    p_phaseCurrents->pwm_delay[5] = -p_phaseCurrents->t_delay2;
                    break;

            default :
                    p_phaseCurrents->pwm_delay[0] = 0;
                    p_phaseCurrents->pwm_delay[1] = 0;
                    p_phaseCurrents->pwm_delay[2] = 0;
                    p_phaseCurrents->pwm_delay[3] = 0;
                    p_phaseCurrents->pwm_delay[4] = 0;
                    p_phaseCurrents->pwm_delay[5] = 0;
                    break;
            }
    }
    else
    {
            p_phaseCurrents->t_delay2 = 0;
            p_phaseCurrents->shiftFlag2 = 0;
    }
}

void Current_samplingSetup(PHASE_CURRENTS *p_phaseCurrents,SVM_T1_T2 *p_t12,frac16_t svm_sector)
{
	/* according to SVM sector set DC-bus current sampling time*/
	switch(svm_sector)
	{
	case 1:
		p_phaseCurrents->samp[0] = p_phaseCurrents->dutyCycleChannel0;//u0
		p_phaseCurrents->samp[1] = p_phaseCurrents->dutyCycleChannel2;//u60
		break;

	case 2:
		p_phaseCurrents->samp[0] = p_phaseCurrents->dutyCycleChannel2;//u120
		p_phaseCurrents->samp[1] = p_phaseCurrents->dutyCycleChannel0;//u60
		break;

	case 3:
		p_phaseCurrents->samp[0] = p_phaseCurrents->dutyCycleChannel2;//u120
		p_phaseCurrents->samp[1] = p_phaseCurrents->dutyCycleChannel4;//u180
		break;

	case 4:
		p_phaseCurrents->samp[0] = p_phaseCurrents->dutyCycleChannel4;//u240
		p_phaseCurrents->samp[1] = p_phaseCurrents->dutyCycleChannel2;//u180
		break;

	case 5:
		p_phaseCurrents->samp[0] = p_phaseCurrents->dutyCycleChannel4;//u240
		p_phaseCurrents->samp[1] = p_phaseCurrents->dutyCycleChannel0;//u300
		break;

	case 6:
		p_phaseCurrents->samp[0] = p_phaseCurrents->dutyCycleChannel0;//u0
		p_phaseCurrents->samp[1] = p_phaseCurrents->dutyCycleChannel4;//u300
		break;

	default :
	    break;
	}

	/* phase current sampling times according to the adjacent sectors */

	p_phaseCurrents->samp_t1_scaled = (frac16_t)MLIB_Mul_F16(p_t12->t_1,(VAL1_F>>1));
	p_phaseCurrents->samp_t2_scaled = (frac16_t)MLIB_Mul_F16(p_t12->t_2,(VAL1_F>>1));

	/* final sampling times  */
	switch(svm_sector)
	{
	case 1:
	case 3:
	case 5:
		p_phaseCurrents->samp_scaled[0] = p_phaseCurrents->samp[0] + p_phaseCurrents->samp_t2_scaled + p_phaseCurrents->delay_0;
		p_phaseCurrents->samp_scaled[1] = p_phaseCurrents->samp[1] + p_phaseCurrents->samp_t1_scaled + p_phaseCurrents->delay_1;
		break;
	case 2:
	case 4:
	case 6:
		p_phaseCurrents->samp_scaled[0] = p_phaseCurrents->samp[0] + p_phaseCurrents->samp_t2_scaled + p_phaseCurrents->delay_0;
		p_phaseCurrents->samp_scaled[1] = p_phaseCurrents->samp[1] + p_phaseCurrents->samp_t1_scaled + p_phaseCurrents->delay_1;
		break;
	default :
	    break;
	}
	p_phaseCurrents->samp_scaled[2] = p_phaseCurrents->samp[2] + p_phaseCurrents->delay_2;
}

void Current_reconstruction_init(PHASE_CURRENTS *p_phaseCurrents)
{
	p_phaseCurrents->delay_0 = HW_DELAY_0_F;
	p_phaseCurrents->delay_1 = HW_DELAY_1_F;
	p_phaseCurrents->delay_2 = HW_DELAY_2_F;
	p_phaseCurrents->samp[2] = 0;
	p_phaseCurrents->critical_sector = CRITICAL_VECTOR_LAST_TIME_F;
}

/***************************************************************************//*!
*
* @brief   SVM with 000Null vector only dedicated for one-shunt sampling
*          A 111Null vector is inserted when either basic vector lasting 
*          time is less than predefined critical time.  
*
* @param   GMCLIB_2COOR_ALBE_T_F16 *pUAlBe
*			- structure of input Alpha Beta voltages
*			IN:->f16Alpha - input Alpha voltage 
*			IN:->f16Beta - input Beta voltage
*          GMCLIB_3COOR_T_F16 *pDutyABC
*                       - structure of output duties
*                       OUT:->f16A - output duty of phase A
*                       OUT:->f16B - output duty of phase B
*                       OUT:->f16C - output duty of phase C
*          SVM_T1_T2 *pT1T2
*                       - structure of output boundary vector lasting time (in ratio) 
*                       OUT:->t_2 - lasting time of the outside vector (in ratio)
*                       OUT:->t_1 - lasting time of the inside vector (in ratio) 
*          PHASE_CURRENTS *p_phaseCurrents
*                       - structure of one-shunt related variables
*                       IN:->critical_sector - minimum last time of basic vector(in ratio)
*                                              the inserted 111Null vector length
*	
*
* @return  SVM sector number
*
******************************************************************************/
int16_t SvmU0n_t1t2(GMCLIB_2COOR_ALBE_T_F16 *pUAlBe, GMCLIB_3COOR_T_F16 *pDutyABC, SVM_T1_T2 *pT1T2,PHASE_CURRENTS *p_phaseCurrents)
{
  frac16_t x,y,z;
  frac16_t ref1, ref2, ref3;
  frac16_t zero_center;

  zero_center=FRAC16(0.0);
 //----------------------------------------------- SVM 000Null only--------------------------------------------------------- 
  // Saturation is enabled to avoid situation when sqrt(Ualpha^2 + Ubeta^2) is larger than 1

  x = pUAlBe->f16Beta;
  y = MLIB_AddSat_F16(MLIB_Mul_F16(pUAlBe->f16Beta,FRAC16(0.5)), MLIB_Mul_F16(pUAlBe->f16Alpha,FRAC16(0.8660254))); // (beta+1.732alpha)/2
  z = MLIB_SubSat_F16(MLIB_Mul_F16(pUAlBe->f16Beta,FRAC16(0.5)), MLIB_Mul_F16(pUAlBe->f16Alpha,FRAC16(0.8660254))); // (beta-1.732alpha)/2
  
  //ref1 is -90 degree£¬ref2 is 30 degree£¬ref3 is 150 degree
  ref1 = x;
  ref2 = -z;
  ref3 = -y;
  
  /*  Decide sector number */
  if(ref3 <= 0)
  {
    if(ref2 > 0)
    {
      if(ref1 <= 0)
      {
        // sector 6£¬0~60 e-degree
        pT1T2->t_1 = -x;
        pT1T2->t_2 = y;

        zero_center= MLIB_Sh1R_F16(MLIB_Sub_F16(FRAC16(1.0),MLIB_Add_F16(pT1T2->t_2,pT1T2->t_1)));

        pDutyABC->f16B = zero_center;   
        pDutyABC->f16C = MLIB_AddSat_F16(pDutyABC->f16B,pT1T2->t_1);        
        pDutyABC->f16A = MLIB_AddSat_F16(pDutyABC->f16C,pT1T2->t_2);
        return 6;
      }
      else
      {
        // sector 1, 0~-60 e-degree
        pT1T2->t_1 = x;
        pT1T2->t_2 = -z;
        zero_center= MLIB_Sh1R_F16(MLIB_Sub_F16(FRAC16(1.0),MLIB_Add_F16(pT1T2->t_2,pT1T2->t_1)));
        pDutyABC->f16C = zero_center;        
        pDutyABC->f16B = MLIB_AddSat_F16(pDutyABC->f16C,pT1T2->t_1);        
        pDutyABC->f16A = MLIB_AddSat_F16(pDutyABC->f16B,pT1T2->t_2);
        return 1;
      }
    }
    else
    {
      // sector 2, -60~-120 e-degree
      pT1T2->t_1 = y;
      pT1T2->t_2 = z;
      zero_center= MLIB_Sh1R_F16(MLIB_Sub_F16(FRAC16(1.0),MLIB_Add_F16(pT1T2->t_2,pT1T2->t_1)));
      pDutyABC->f16C = zero_center;        
      pDutyABC->f16A = MLIB_AddSat_F16(pDutyABC->f16C,pT1T2->t_1);
      pDutyABC->f16B = MLIB_AddSat_F16(pDutyABC->f16A,pT1T2->t_2);
      return 2;
    }
  }
  else
  {
    if(ref2 <= 0)
    {
      if(ref1 <= 0)
      {
        // sector 4, 120~180 e-degree
        pT1T2->t_1 = z;
        pT1T2->t_2 = -x;
        zero_center= MLIB_Sh1R_F16(MLIB_Sub_F16(FRAC16(1.0),MLIB_Add_F16(pT1T2->t_2,pT1T2->t_1)));
        pDutyABC->f16A = zero_center;
        pDutyABC->f16B = MLIB_AddSat_F16(pDutyABC->f16A,pT1T2->t_1);
        pDutyABC->f16C = MLIB_AddSat_F16(pDutyABC->f16B,pT1T2->t_2);
        return 4;
      }
      else
      {
        // sector 3, -120~-180 e-degree
        pT1T2->t_1 = -y;
        pT1T2->t_2 = x;
        zero_center= MLIB_Sh1R_F16(MLIB_Sub_F16(FRAC16(1.0),MLIB_Add_F16(pT1T2->t_2,pT1T2->t_1)));
        pDutyABC->f16A = zero_center;
        pDutyABC->f16C = MLIB_AddSat_F16(pDutyABC->f16A,pT1T2->t_1);
        pDutyABC->f16B = MLIB_AddSat_F16(pDutyABC->f16C,pT1T2->t_2);
        return 3;
      }
    }
    else
    {
      // sector 5, 60~120 e-degree
      pT1T2->t_1 = -z;
      pT1T2->t_2 = -y;
      zero_center= MLIB_Sh1R_F16(MLIB_Sub_F16(FRAC16(1.0),MLIB_Add_F16(pT1T2->t_2,pT1T2->t_1)));
      pDutyABC->f16B = zero_center;      
      pDutyABC->f16A = MLIB_AddSat_F16(pDutyABC->f16B,pT1T2->t_1);
      pDutyABC->f16C = MLIB_AddSat_F16(pDutyABC->f16A,pT1T2->t_2);
      return 5;
    }
  }
}
