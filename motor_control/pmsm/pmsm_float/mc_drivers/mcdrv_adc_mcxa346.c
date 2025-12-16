/*
 * Copyright 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "mcdrv_adc_mcxa346.h"
#include "fsl_gpio.h"
#include "PFC_statemachine.h"
#include "mc_periph_init.h"
#include <math.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define TEMP_INVALID_VALUE   (0xFFFFU)
#define ADC_MAX 65535.0f
/*******************************************************************************
 * Local function prototypes
 ******************************************************************************/
static uint32_t  temp_sensor_factored_discharge;
static uint32_t temp_sensor_factored_ambient;
mcdrv_adc_t tempADC;

float V0, R1, ln, ln_beta, ln_t1, T2_1, T2_act, t1;
float one,two,three,four;

/*******************************************************************************
 * Variables
 ******************************************************************************/
#define V_REF 3.3
#define ADC_LPF_FILTER_FACTOR 4
#define IPM_TEMP_IIR_FILTER_SCALE    8u      /**< Right-shift scale (2^8 = 256) */
#define IPM_TEMP_IIR_FILTER_ALPHA    64u
#define ADC_MAX 27000
/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Reads and calculates 3 phase samples based on SVM sector
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
int CommandIdRecFan, CommandIdRecTemp;


void MCDRV_FanCurrentGet(mcdrv_adc_t *this)
{
    GMCLIB_3COOR_T_F16 sIABCtemp;

    /* Read available converted values from the FIFO. */
    while( LPADC_GetConvResult(this->pToAdcBase, &this->s_ADC_ResultStructure) )
    {
        switch(this->s_ADC_ResultStructure.commandIdSource)
        {
           case 1U:
                    /* Command 1 */
                    this->ui16AdcSmpChannelA = (this->s_ADC_ResultStructure.convValue);
                    break;
                    
           case 2U:
                    /* Command 2 */
                    this->ui16AdcSmpChannelB = (this->s_ADC_ResultStructure.convValue);
                    break;
                    
           default:
                    CommandIdRecFan = this->s_ADC_ResultStructure.commandIdSource;
                    break;
        }
    }
    
    /* direct sensing of phase B and C, calculation of A */
    sIABCtemp.f16C = MLIB_Sub_F16((frac16_t)(this->ui16AdcSmpChannelA), this->sCurrSec16.ui16OffsetPhaB);
    sIABCtemp.f16B = MLIB_Sub_F16((frac16_t)(this->ui16AdcSmpChannelB), this->sCurrSec16.ui16OffsetPhaC);
    sIABCtemp.f16A = MLIB_Neg_F16(MLIB_AddSat_F16(sIABCtemp.f16B, sIABCtemp.f16C));

    /* pass measured phase currents to the main module structure */
    this->psIABC->f16A = sIABCtemp.f16A;
    this->psIABC->f16B = sIABCtemp.f16B;
    this->psIABC->f16C = sIABCtemp.f16C;
}

void MCDRV_TemperatureGet(mcdrv_adc_t *this)
{
//  while( LPADC_GetConvResult(this->pToAdcBase, &this->s_ADC_ResultStructure) )
//  {
//    switch(this->s_ADC_ResultStructure.commandIdSource)
//        {
//           case 4U:
//                    /* Command 1 */
//                    this->ui16EXHTemp = (this->s_ADC_ResultStructure.convValue);
//                    break;
//                    
//           case 5U:
//                    /* Command 2 */
//                    this->ui16PIPTemp = (this->s_ADC_ResultStructure.convValue);
//                    break;
//                    
//           case 6U:
//                    /* Command 3 */
//                    this->ui16ENVTemp = (this->s_ADC_ResultStructure.convValue);
//                    break;
//                    
//           default:
//                    CommandIdRecTemp = this->s_ADC_ResultStructure.commandIdSource;
//                    break;
//        }
//  }
}

/*!
 * @brief Initializes phase current channel offset measurement
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_Curr3Ph2ShCalibInit(mcdrv_adc_t *this)
{
    /* clear offset values */
    this->sCurrSec16.ui16OffsetPhaB = 0U;
    this->sCurrSec16.ui16OffsetPhaC = 0U;
    this->sCurrSec23.ui16OffsetPhaA = 0U;
    this->sCurrSec23.ui16OffsetPhaC = 0U;
    this->sCurrSec45.ui16OffsetPhaA = 0U;
    this->sCurrSec45.ui16OffsetPhaB = 0U;

    this->sCurrSec16.ui16CalibPhaB = 0U;
    this->sCurrSec16.ui16CalibPhaC = 0U;
    this->sCurrSec23.ui16CalibPhaA = 0U;
    this->sCurrSec23.ui16CalibPhaC = 0U;
    this->sCurrSec45.ui16CalibPhaA = 0U;
    this->sCurrSec45.ui16CalibPhaB = 0U;

    /* initialize offset filters */
    this->sCurrSec16.ui16FiltPhaB.u16Sh = this->ui16OffsetFiltWindow;
    this->sCurrSec16.ui16FiltPhaC.u16Sh = this->ui16OffsetFiltWindow;
    this->sCurrSec23.ui16FiltPhaA.u16Sh = this->ui16OffsetFiltWindow;
    this->sCurrSec23.ui16FiltPhaC.u16Sh = this->ui16OffsetFiltWindow;
    this->sCurrSec45.ui16FiltPhaA.u16Sh = this->ui16OffsetFiltWindow;
    this->sCurrSec45.ui16FiltPhaB.u16Sh = this->ui16OffsetFiltWindow;

    GDFLIB_FilterMAInit_F16((frac16_t)0, &this->sCurrSec16.ui16FiltPhaB);
    GDFLIB_FilterMAInit_F16((frac16_t)0, &this->sCurrSec16.ui16FiltPhaC);
    GDFLIB_FilterMAInit_F16((frac16_t)0, &this->sCurrSec23.ui16FiltPhaA);
    GDFLIB_FilterMAInit_F16((frac16_t)0, &this->sCurrSec23.ui16FiltPhaC);
    GDFLIB_FilterMAInit_F16((frac16_t)0, &this->sCurrSec45.ui16FiltPhaA);
    GDFLIB_FilterMAInit_F16((frac16_t)0, &this->sCurrSec45.ui16FiltPhaB);
}

void MCDRV_Curr3Ph2ShChanAssign(mcdrv_adc_t *this)
{

}


/*!
 * @brief Get three phase current from two shunts measurements
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_Curr3Ph2ShGet(mcdrv_adc_t *this)
{

}

/*!
 * @brief Function reads current samples and filter them based on SVM sector
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_Curr3Ph2ShCalib(mcdrv_adc_t *this)
{
    /* sensing of offset IB, IC*/
    this->sCurrSec16.ui16CalibPhaB =
        GDFLIB_FilterMA_F16((frac16_t)(this->ui16AdcSmpChannelA), &this->sCurrSec16.ui16FiltPhaB);
    this->sCurrSec16.ui16CalibPhaC =
        GDFLIB_FilterMA_F16((frac16_t)(this->ui16AdcSmpChannelB), &this->sCurrSec16.ui16FiltPhaC);
}

/*!
 * @brief Function passes measured offset values to main structure
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_Curr3Ph2ShCalibSet(mcdrv_adc_t *this)
{

    /* pass calibration data for sector 1 and 6 */
    this->sCurrSec16.ui16OffsetPhaB = this->sCurrSec16.ui16CalibPhaB;
    this->sCurrSec16.ui16OffsetPhaC = this->sCurrSec16.ui16CalibPhaC;

    /* pass calibration data for sector 2 and 3 */
    this->sCurrSec23.ui16OffsetPhaA = this->sCurrSec23.ui16CalibPhaA;
    this->sCurrSec23.ui16OffsetPhaC = this->sCurrSec23.ui16CalibPhaC;

    /* pass calibration data for sector 4 and 5 */
    this->sCurrSec45.ui16OffsetPhaA = this->sCurrSec45.ui16CalibPhaA;
    this->sCurrSec45.ui16OffsetPhaB = this->sCurrSec45.ui16CalibPhaB;

}

/*!
 * @brief Function reads and passes DCB voltage sample
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_VoltDcBusGet(mcdrv_adc_t *this)
{

}

/*!
 * @brief Function reads and passes auxiliary sample
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_AuxValGet(mcdrv_adc_t *this)
{

}
/***************************************************************************//*!
*
* @brief   Update ADC sample channel when SVM sector is updated.
*
* @param   this   Pointer to the current object
*
* @return  none
*
******************************************************************************/
int CommandIdRecComp;uint16_t sIDCtemp[3];
uint16_t raw_current_1 =0;
uint16_t raw_current_2 =0;
uint16_t raw_current_3 =0;

void InitTempSensor(TEMP_SENSOR_t *Temp_Sensors)
{



	    Temp_Sensors->TEMP_SENSOR_DISCHARGE.SENSOR_BETA_VALUE = 3950;
		Temp_Sensors->TEMP_SENSOR_DISCHARGE.SENSOR_TYPICAL_RESISTANCE_AT_SET_TEMP_VALUE = 50000; //20000
		Temp_Sensors->TEMP_SENSOR_DISCHARGE.SENSOR_TYPICAL_VALUE_OF_SET_TEMP_KELVIN = 298.15f;//20000
		Temp_Sensors->TEMP_SENSOR_DISCHARGE.SENSOR_PU_PD_RESISTANCE_VALUE = 20000;//10000

		Temp_Sensors->TEMP_SENSOR_AMBIENT.SENSOR_BETA_VALUE = 3950;
		Temp_Sensors->TEMP_SENSOR_AMBIENT.SENSOR_TYPICAL_RESISTANCE_AT_SET_TEMP_VALUE = 10000; //50000
		Temp_Sensors->TEMP_SENSOR_AMBIENT.SENSOR_TYPICAL_VALUE_OF_SET_TEMP_KELVIN = 298.15f;//4700
		Temp_Sensors->TEMP_SENSOR_AMBIENT.SENSOR_PU_PD_RESISTANCE_VALUE = 4700;//298.15

		Temp_Sensors->TEMP_SENSOR_COMPRESSOR.SENSOR_BETA_VALUE = 3950;
		Temp_Sensors->TEMP_SENSOR_COMPRESSOR.SENSOR_TYPICAL_RESISTANCE_AT_SET_TEMP_VALUE = 10000; //50000
		Temp_Sensors->TEMP_SENSOR_COMPRESSOR.SENSOR_TYPICAL_VALUE_OF_SET_TEMP_KELVIN = 298.15f;//4700
		Temp_Sensors->TEMP_SENSOR_COMPRESSOR.SENSOR_PU_PD_RESISTANCE_VALUE = 4700;//298.15
}

//void lpf_adc_values(mcdrv_adc_t *Temp_Sensor)
//{
//
//	temp_sensor_factored_discharge = (uint32_t)((uint32_t)temp_sensor_factored_discharge + (uint32_t)((uint32_t)Temp_Sensor->ui16DischargeTempRaw - (uint32_t)Temp_Sensor->DischargeTempFiltered)); 		// IIR Filter
//	Temp_Sensor->DischargeTempFiltered = (uint32_t)temp_sensor_factored_discharge >> ADC_LPF_FILTER_FACTOR; 									// IIR Filter
//
//	temp_sensor_factored_ambient = (uint32_t)(temp_sensor_factored_ambient + (uint32_t)((uint32_t)Temp_Sensor->ui16AmbientTempRaw - (uint32_t)Temp_Sensor->AmbientTempFiltered)); 		// IIR Filter
//	Temp_Sensor->AmbientTempFiltered = (uint32_t)temp_sensor_factored_ambient >> ADC_LPF_FILTER_FACTOR; 									// IIR Filter
//
//
//}

uint32_t  DischargeTempFilter(uint16_t raw_value)
{

	 uint32_t filtered_discharge_value = (((uint32_t)IPM_TEMP_IIR_FILTER_ALPHA * raw_value) +
		(((1UL << IPM_TEMP_IIR_FILTER_SCALE) - IPM_TEMP_IIR_FILTER_ALPHA) * tempADC.DischargeTempFiltered)) >> IPM_TEMP_IIR_FILTER_SCALE;

         return filtered_discharge_value;
}

uint32_t  AmbinetTempFilter(uint32_t raw_value)
{

	 uint32_t filtered_ambient_value = (((uint32_t)IPM_TEMP_IIR_FILTER_ALPHA * raw_value) +
		(((1UL << IPM_TEMP_IIR_FILTER_SCALE) - IPM_TEMP_IIR_FILTER_ALPHA) * tempADC.AmbientTempFiltered)) >> IPM_TEMP_IIR_FILTER_SCALE;

         return filtered_ambient_value;
}

uint32_t CondensorTempFilter(uint32_t raw_value)
{
	uint32_t  condesnor_temp = (((uint32_t)IPM_TEMP_IIR_FILTER_ALPHA * raw_value) +
			(((1UL << IPM_TEMP_IIR_FILTER_SCALE) - IPM_TEMP_IIR_FILTER_ALPHA) * tempADC.CondensorTempFiltered)) >> IPM_TEMP_IIR_FILTER_SCALE;
	return condesnor_temp;

}



void AmbinetTempCalculation(mcdrv_adc_t *Temp_Sensor,TEMP_SENSOR_t *Sensor_param)
{
            V0 = 0.0f;R1 = 0.0f;ln = 0.0f;ln_beta = 0.0f;T2_1 = 0.0f;T2_act = 0.0f;t1 = 0.0f;
			V0 = (float)(((float)V_REF * (float)Temp_Sensor->AmbientTempFiltered) / (float)ADC_MAX);
			R1 = (float)((((float)V_REF - (float)V0) * (float)Sensor_param->TEMP_SENSOR_AMBIENT.SENSOR_PU_PD_RESISTANCE_VALUE) / (float)V0);
			ln = (float)((R1 / (float)Sensor_param->TEMP_SENSOR_AMBIENT.SENSOR_TYPICAL_RESISTANCE_AT_SET_TEMP_VALUE));
			if(ln > 0.00f)
			{
				ln = logf(ln);
			}
			else
			{
				ln = 0.00f;
			}
			ln_beta = (float)(ln / (float)Sensor_param->TEMP_SENSOR_AMBIENT.SENSOR_BETA_VALUE);
			t1 = (float)((float)1.00 / (float)Sensor_param->TEMP_SENSOR_AMBIENT.SENSOR_TYPICAL_VALUE_OF_SET_TEMP_KELVIN);
			ln_t1 = (float)((float)ln_beta + (float)t1);
			T2_1 = (float)(1.00 / ln_t1);
			T2_act = T2_1 - 273.15f;
			//Temp_Sensor->AmbinetTempFloating = T2_act;
			Temp_Sensor->AmbientTempActual = (int16_t)T2_act;


}

void DischargeTempCalculation(mcdrv_adc_t *Temp_Sensor,TEMP_SENSOR_t *Sensor_param)
{

		V0 = (float)((V_REF * (float)Temp_Sensor->DischargeTempFiltered) / (float)ADC_MAX);
		R1 = (float)(((V_REF - V0) * (float)Sensor_param->TEMP_SENSOR_DISCHARGE.SENSOR_PU_PD_RESISTANCE_VALUE) / V0);
	    ln = (float)(((float)R1 / (float)Sensor_param->TEMP_SENSOR_DISCHARGE.SENSOR_TYPICAL_RESISTANCE_AT_SET_TEMP_VALUE));
	     if(ln > 0.00f)
		{
				ln = logf(ln);
		}
		else
		{
				ln = 0.00f;
		}

			ln_beta = (float)(ln / (float)Sensor_param->TEMP_SENSOR_DISCHARGE.SENSOR_BETA_VALUE);
			t1 = (float)((float)1.00 / (float)Sensor_param->TEMP_SENSOR_DISCHARGE.SENSOR_TYPICAL_VALUE_OF_SET_TEMP_KELVIN);
			ln_t1 = (float)((float)ln_beta + (float)t1);
			T2_1 = (float)(1.00 / ln_t1);
			T2_act =  T2_1 - 273.15f;;
			//Temp_Sensor->DischargeTempFloating = T2_act;
			Temp_Sensor->DischargeTempActual = (int16_t)T2_act;

}

void CondensorTempCalculation(mcdrv_adc_t *Temp_Sensor,TEMP_SENSOR_t *Sensor_param)
{
	//
	//			V0 = 0.0f;R1 = 0.0f;ln = 0.0f;ln_beta = 0.0f;T2_1 = 0.0f;T2_act = 0.0f;t1 = 0.0f;
	//						V0 = (float)(((float)V_REF * (float)Temp_Sensor->CondensorTempFiltered) / (float)ADC_MAX);
	//						R1 = (float)((((float)V_REF - (float)V0) * (float)Sensor_param->TEMP_SENSOR_COMPRESSOR.SENSOR_PU_PD_RESISTANCE_VALUE) / (float)V0);
	//						ln = (float)((R1 / (float)Sensor_param->TEMP_SENSOR_COMPRESSOR.SENSOR_TYPICAL_RESISTANCE_AT_SET_TEMP_VALUE));
	//
	//						if(ln > 0.00f)
	//						{
	//							ln = logf(ln);
	//						}
	//						else
	//						{
	//							ln = 0.00f;
	//						}
	//						ln_beta = (float)(ln / (float)Sensor_param->TEMP_SENSOR_COMPRESSOR.SENSOR_BETA_VALUE);
	//						t1 = (float)((float)1.00 / (float)Sensor_param->TEMP_SENSOR_COMPRESSOR.SENSOR_TYPICAL_VALUE_OF_SET_TEMP_KELVIN);
	//						ln_t1 = (float)((float)ln_beta + (float)t1);
	//						T2_1 = (float)(1.00 / ln_t1);
	//						T2_act = T2_1 - 273.15f;
	//						//Temp_Sensor->AmbinetTempFloating = T2_act;
	//						Temp_Sensor->CondesnorTempActual = (int16_t)T2_act;

}

void PhaseCurrentReconstruction(mcdrv_adc_t *this)
{
    while( LPADC_GetConvResult(this->pToAdcBase, &this->s_ADC_ResultStructure) )
    {

    	switch( this->s_ADC_ResultStructure.commandIdSource )
    	{
            case 1U:
                    /* Command 1 */
                    sIDCtemp[0] = (this->s_ADC_ResultStructure.convValue);
	raw_current_1 = (this->s_ADC_ResultStructure.convValue);
                    break;

            case 2U:
                    /* Command 2 */
                    sIDCtemp[1] = (this->s_ADC_ResultStructure.convValue);
    raw_current_2 = (this->s_ADC_ResultStructure.convValue);
                    break;

            case 3U:
                    /* Command 3 */
                    sIDCtemp[2] = (this->s_ADC_ResultStructure.convValue);
	raw_current_3 = (this->s_ADC_ResultStructure.convValue);
                    break;
            case 4U:
            	this->ui16CondensorTempRaw = (this->s_ADC_ResultStructure.convValue);
            	//this->CondensorTempFiltered = CondensorTempCalculation(this->ui16CondensorTempRaw);
            	//convert_adc_to_actual_temperature(this, &Sensor_param);
            	break;

            case 5U:
            	this->ui16AmbientTempRaw = (this->s_ADC_ResultStructure.convValue);
            	this->AmbientTempFiltered = AmbinetTempFilter(this->ui16AmbientTempRaw);
            	AmbinetTempCalculation(this, &Sensor_param);
            	break;

            case 6U:
            	this->ui16DischargeTempRaw = (this->s_ADC_ResultStructure.convValue);
            	this->DischargeTempFiltered =  DischargeTempFilter(this->ui16DischargeTempRaw);
            	DischargeTempCalculation(this, &Sensor_param);
            	break;

            case 7U:
            	this->ui16HeatSinkTempRaw = (this->s_ADC_ResultStructure.convValue);

            	break;

            default:
              CommandIdRecComp = this->s_ADC_ResultStructure.commandIdSource;
                    break;
    	}
    }
    


    switch(*this->pui16SVMSector)
    {
    case 1:
        this->psIABC->f16A  =   MLIB_Sub_F16(sIDCtemp[2], sIDCtemp[0]);
        this->psIABC->f16C  =   MLIB_Neg_F16(MLIB_Sub_F16(sIDCtemp[2], sIDCtemp[1]));
        this->psIABC->f16B  =   MLIB_Neg_F16((this->psIABC->f16A + this->psIABC->f16C));
        break;
    case 2:
        this->psIABC->f16B =   MLIB_Sub_F16(sIDCtemp[2], sIDCtemp[0]);
        this->psIABC->f16C =   MLIB_Neg_F16(MLIB_Sub_F16(sIDCtemp[2], sIDCtemp[1]));
        this->psIABC->f16A =   MLIB_Neg_F16((this->psIABC->f16C + this->psIABC->f16B));
        break;
    case 3:
        this->psIABC->f16B =   MLIB_Sub_F16(sIDCtemp[2], sIDCtemp[0]);
        this->psIABC->f16A =   MLIB_Neg_F16(MLIB_Sub_F16(sIDCtemp[2], sIDCtemp[1]));
        this->psIABC->f16C =   MLIB_Neg_F16((this->psIABC->f16A + this->psIABC->f16B));
        break;
    case 4:
        this->psIABC->f16C =   MLIB_Sub_F16(sIDCtemp[2], sIDCtemp[0]);
        this->psIABC->f16A =   MLIB_Neg_F16(MLIB_Sub_F16(sIDCtemp[2], sIDCtemp[1]));
        this->psIABC->f16B =   MLIB_Neg_F16((this->psIABC->f16A + this->psIABC->f16C));
        break;
    case 5:
        this->psIABC->f16C =   MLIB_Sub_F16(sIDCtemp[2], sIDCtemp[0]);
        this->psIABC->f16B =   MLIB_Neg_F16(MLIB_Sub_F16(sIDCtemp[2], sIDCtemp[1]));
        this->psIABC->f16A =   MLIB_Neg_F16((this->psIABC->f16C + this->psIABC->f16B));
        break;
    case 6:
        this->psIABC->f16A =   MLIB_Sub_F16(sIDCtemp[2], sIDCtemp[0]);
        this->psIABC->f16B =   MLIB_Neg_F16(MLIB_Sub_F16(sIDCtemp[2], sIDCtemp[1]));
        this->psIABC->f16C =   MLIB_Neg_F16((this->psIABC->f16A + this->psIABC->f16B));
        break;
    }
}

int CommandIdRecPFC;//for debug
uint32_t raw_dc_bus = 0;


void PFC_CurrAndVoltDcBusGet(mcdrv_adc_t *this)
{


    /* Read available converted values from the FIFO. */
    while(LPADC_GetConvResult(ADC1, &this->s_ADC_ResultStructure))
    {

      switch( this->s_ADC_ResultStructure.commandIdSource )
      {
          case 3U:
          /* Command 3 - UDCB */
            gsPFC_Drive.sUInPeakDetection.f16Udcb = this->s_ADC_ResultStructure.convValue >> 1;
            raw_dc_bus = this->s_ADC_ResultStructure.convValue;
                break;
            
          case 4U:
          /* Command 4 - PFC_I */
            gsPFC_Drive.sUInPeakDetection.f16Imos = this->s_ADC_ResultStructure.convValue >> 1;
                break;
            
          case 5U:
          /* Command 5 - VAC*/
            gsPFC_Drive.sUInPeakDetection.f16UIn = this->s_ADC_ResultStructure.convValue >> 1;
                break;
          
          default:
            CommandIdRecPFC = this->s_ADC_ResultStructure.commandIdSource;
          break;
      }
    }
#if 1
    gsPFC_Drive.sUInPeakDetection.fltUIn        = MLIB_ConvSc_FLTsf(gsPFC_Drive.sUInPeakDetection.f16UIn, g_fltPFCVoltageScale);
    gsPFC_Drive.sUInPeakDetection.fltImos       = MLIB_ConvSc_FLTsf(gsPFC_Drive.sUInPeakDetection.f16Imos , g_fltPFCCurrentScale);
    gsPFC_Drive.sUInPeakDetection.fltUdcb       = MLIB_ConvSc_FLTsf(gsPFC_Drive.sUInPeakDetection.f16Udcb , g_fltPFCVoltageScale);
#else
    gsPFC_Drive.sUInPeakDetection.fltUIn        = MLIB_ConvSc_FLTsf(gsPFC_Drive.sUInPeakDetection.f16UIn , 3.3f);
    gsPFC_Drive.sUInPeakDetection.fltImos       = MLIB_ConvSc_FLTsf(gsPFC_Drive.sUInPeakDetection.f16Imos , 3.3f);
    gsPFC_Drive.sUInPeakDetection.fltUdcb       = MLIB_ConvSc_FLTsf(gsPFC_Drive.sUInPeakDetection.f16Udcb , 3.3f);
#endif
}
