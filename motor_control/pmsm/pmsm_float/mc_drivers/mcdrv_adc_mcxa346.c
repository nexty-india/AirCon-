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

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

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
void PhaseCurrentReconstruction(mcdrv_adc_t *this)
{
    while( LPADC_GetConvResult(this->pToAdcBase, &this->s_ADC_ResultStructure) )
    {       
    	switch( this->s_ADC_ResultStructure.commandIdSource )
    	{
            case 1U:
                    /* Command 1 */
                    sIDCtemp[0] = (this->s_ADC_ResultStructure.convValue);
                    break;

            case 2U:
                    /* Command 2 */
                    sIDCtemp[1] = (this->s_ADC_ResultStructure.convValue);
                    break;

            case 3U:
                    /* Command 3 */
                    sIDCtemp[2] = (this->s_ADC_ResultStructure.convValue);
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
