/*
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
 
#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

/*!
 * @addtogroup pin_mux
 * @{
 */

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);

#define PCR_DSE_dse0 0x00u /*!<@brief Drive Strength Enable: Low */
#define PCR_IBE_ibe1 0x01u /*!<@brief Input Buffer Enable: Enables */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitFreeMASTER_UARTPins(void);

#define PCR_IBE_ibe1 0x01u /*!<@brief Input Buffer Enable: Enables */

/*! @name PORT3_6 (number 44), LED_GREEN
  @{ */

/* Symbols to be used with GPIO driver */
#define BOARD_INITLEDSPINS_LED_GREEN_GPIO GPIO3               /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITLEDSPINS_LED_GREEN_INIT_GPIO_VALUE 0U       /*!<@brief GPIO output initial state */
#define BOARD_INITLEDSPINS_LED_GREEN_GPIO_PIN 6U              /*!<@brief GPIO pin number */
#define BOARD_INITLEDSPINS_LED_GREEN_GPIO_PIN_MASK (1U << 6U) /*!<@brief GPIO pin mask */

/* Symbols to be used with PORT driver */
#define BOARD_INITLEDSPINS_LED_GREEN_PORT PORT3               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITLEDSPINS_LED_GREEN_PIN 6U                   /*!<@brief PORT pin number */
#define BOARD_INITLEDSPINS_LED_GREEN_PIN_MASK (1U << 6U)      /*!<@brief PORT pin mask */
                                                              /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitLEDsPins(void);

#define PCR_IBE_ibe1 0x01u /*!<@brief Input Buffer Enable: Enables */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitUARTPins(void);

#define PCR_IBE_ibe1 0x01u /*!<@brief Input Buffer Enable: Enables */

/*! @name PORT3_8 (number 42), Compressor_VH
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITPWM0PINS_Compressor_VH_PORT PORT3               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPWM0PINS_Compressor_VH_PIN 8U                   /*!<@brief PORT pin number */
#define BOARD_INITPWM0PINS_Compressor_VH_PIN_MASK (1U << 8U)      /*!<@brief PORT pin mask */
                                                                  /* @} */

/*! @name PORT3_10 (number 40), Compressor_WH
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITPWM0PINS_Compressor_WH_PORT PORT3                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPWM0PINS_Compressor_WH_PIN 10U                   /*!<@brief PORT pin number */
#define BOARD_INITPWM0PINS_Compressor_WH_PIN_MASK (1U << 10U)      /*!<@brief PORT pin mask */
                                                                   /* @} */

/*! @name PORT3_1 (number 45), Compressor_UL
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITPWM0PINS_Compressor_UL_PORT PORT3               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPWM0PINS_Compressor_UL_PIN 1U                   /*!<@brief PORT pin number */
#define BOARD_INITPWM0PINS_Compressor_UL_PIN_MASK (1U << 1U)      /*!<@brief PORT pin mask */
                                                                  /* @} */

/*! @name PORT3_9 (number 41), Compressor_VL
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITPWM0PINS_Compressor_VL_PORT PORT3               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPWM0PINS_Compressor_VL_PIN 9U                   /*!<@brief PORT pin number */
#define BOARD_INITPWM0PINS_Compressor_VL_PIN_MASK (1U << 9U)      /*!<@brief PORT pin mask */
                                                                  /* @} */

/*! @name PORT3_11 (number 39), Compressor_WL
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITPWM0PINS_Compressor_WL_PORT PORT3                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPWM0PINS_Compressor_WL_PIN 11U                   /*!<@brief PORT pin number */
#define BOARD_INITPWM0PINS_Compressor_WL_PIN_MASK (1U << 11U)      /*!<@brief PORT pin mask */
                                                                   /* @} */

/*! @name PORT2_7 (number 21), Compressor_FO
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITPWM0PINS_Compressor_FO_PORT PORT2               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPWM0PINS_Compressor_FO_PIN 7U                   /*!<@brief PORT pin number */
#define BOARD_INITPWM0PINS_Compressor_FO_PIN_MASK (1U << 7U)      /*!<@brief PORT pin mask */
                                                                  /* @} */

/*! @name PORT3_0 (number 46), Compressor_UH
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITPWM0PINS_Compressor_UH_PORT PORT3               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPWM0PINS_Compressor_UH_PIN 0U                   /*!<@brief PORT pin number */
#define BOARD_INITPWM0PINS_Compressor_UH_PIN_MASK (1U << 0U)      /*!<@brief PORT pin mask */
                                                                  /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPWM0Pins(void);

#define PCR_IBE_ibe1 0x01u         /*!<@brief Input Buffer Enable: Enables */
#define PORT2_PCR2_MUX_mux01 0x01u /*!<@brief Pin Multiplex Control: Alternative 1 (chip-specific) */

/*! @name PORT3_30 (number 31), Fan_UH
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITPWM1PINS_Fan_UH_PORT PORT3                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPWM1PINS_Fan_UH_PIN 30U                   /*!<@brief PORT pin number */
#define BOARD_INITPWM1PINS_Fan_UH_PIN_MASK (1U << 30U)      /*!<@brief PORT pin mask */
                                                            /* @} */

/*! @name PORT3_14 (number 36), Fan_VH
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITPWM1PINS_Fan_VH_PORT PORT3                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPWM1PINS_Fan_VH_PIN 14U                   /*!<@brief PORT pin number */
#define BOARD_INITPWM1PINS_Fan_VH_PIN_MASK (1U << 14U)      /*!<@brief PORT pin mask */
                                                            /* @} */

/*! @name PORT3_12 (number 38), Fan_WH
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITPWM1PINS_Fan_WH_PORT PORT3                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPWM1PINS_Fan_WH_PIN 12U                   /*!<@brief PORT pin number */
#define BOARD_INITPWM1PINS_Fan_WH_PIN_MASK (1U << 12U)      /*!<@brief PORT pin mask */
                                                            /* @} */

/*! @name PORT3_31 (number 30), Fan_UL
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITPWM1PINS_Fan_UL_PORT PORT3                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPWM1PINS_Fan_UL_PIN 31U                   /*!<@brief PORT pin number */
#define BOARD_INITPWM1PINS_Fan_UL_PIN_MASK (1U << 31U)      /*!<@brief PORT pin mask */
                                                            /* @} */

/*! @name PORT3_15 (number 35), Fan_VL
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITPWM1PINS_Fan_VL_PORT PORT3                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPWM1PINS_Fan_VL_PIN 15U                   /*!<@brief PORT pin number */
#define BOARD_INITPWM1PINS_Fan_VL_PIN_MASK (1U << 15U)      /*!<@brief PORT pin mask */
                                                            /* @} */

/*! @name PORT3_13 (number 37), Fan_WL
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITPWM1PINS_Fan_WL_PORT PORT3                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPWM1PINS_Fan_WL_PIN 13U                   /*!<@brief PORT pin number */
#define BOARD_INITPWM1PINS_Fan_WL_PIN_MASK (1U << 13U)      /*!<@brief PORT pin mask */
                                                            /* @} */

/*! @name PORT2_2 (number 16), Fan_FO
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITPWM1PINS_Fan_FO_PORT PORT2               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPWM1PINS_Fan_FO_PIN 2U                   /*!<@brief PORT pin number */
#define BOARD_INITPWM1PINS_Fan_FO_PIN_MASK (1U << 2U)      /*!<@brief PORT pin mask */
                                                           /* @} */

/*! @name PORT3_27 (number 34), PFC_PWM
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITPWM1PINS_PFC_PWM_PORT PORT3                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPWM1PINS_PFC_PWM_PIN 27U                   /*!<@brief PORT pin number */
#define BOARD_INITPWM1PINS_PFC_PWM_PIN_MASK (1U << 27U)      /*!<@brief PORT pin mask */
                                                             /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPWM1Pins(void);

#define PCR_IBE_ibe0 0x00u         /*!<@brief Input Buffer Enable: Disables */
#define PORT2_PCR4_MUX_mux00 0x00u /*!<@brief Pin Multiplex Control: Alternative 0 (GPIO) */

/*! @name PORT2_15 (number 24), Compressor_IDC
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITADC0PINS_Compressor_IDC_PORT PORT2                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITADC0PINS_Compressor_IDC_PIN 15U                   /*!<@brief PORT pin number */
#define BOARD_INITADC0PINS_Compressor_IDC_PIN_MASK (1U << 15U)      /*!<@brief PORT pin mask */
                                                                    /* @} */

/*! @name PORT2_15 (number 24), Compressor_IDC
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITADC0PINS_OPAMP0_OUT_PORT PORT2                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITADC0PINS_OPAMP0_OUT_PIN 15U                   /*!<@brief PORT pin number */
#define BOARD_INITADC0PINS_OPAMP0_OUT_PIN_MASK (1U << 15U)      /*!<@brief PORT pin mask */
                                                                /* @} */

/*! @name PORT2_4 (number 18), ENV_TEMP
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITADC0PINS_ENV_TEMP_PORT PORT2               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITADC0PINS_ENV_TEMP_PIN 4U                   /*!<@brief PORT pin number */
#define BOARD_INITADC0PINS_ENV_TEMP_PIN_MASK (1U << 4U)      /*!<@brief PORT pin mask */
                                                             /* @} */

/*! @name PORT2_3 (number 17), PIP_TEMP
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITADC0PINS_PIP_TEMP_PORT PORT2               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITADC0PINS_PIP_TEMP_PIN 3U                   /*!<@brief PORT pin number */
#define BOARD_INITADC0PINS_PIP_TEMP_PIN_MASK (1U << 3U)      /*!<@brief PORT pin mask */
                                                             /* @} */

/*! @name PORT0_6 (number 53), C_IPMT
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITADC0PINS_C_IPMT_PORT PORT0               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITADC0PINS_C_IPMT_PIN 6U                   /*!<@brief PORT pin number */
#define BOARD_INITADC0PINS_C_IPMT_PIN_MASK (1U << 6U)      /*!<@brief PORT pin mask */
                                                           /* @} */

/*! @name PORT1_7 (number 1), EXH_TEMP
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITADC0PINS_EXH_TEMP_PORT PORT1               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITADC0PINS_EXH_TEMP_PIN 7U                   /*!<@brief PORT pin number */
#define BOARD_INITADC0PINS_EXH_TEMP_PIN_MASK (1U << 7U)      /*!<@brief PORT pin mask */
                                                             /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitADC0Pins(void);

#define PCR_IBE_ibe0 0x00u          /*!<@brief Input Buffer Enable: Disables */
#define PORT2_PCR19_MUX_mux00 0x00u /*!<@brief Pin Multiplex Control: Alternative 0 (GPIO) */
#define PORT2_PCR5_MUX_mux00 0x00u  /*!<@brief Pin Multiplex Control: Alternative 0 (GPIO) */
#define PORT2_PCR6_MUX_mux00 0x00u  /*!<@brief Pin Multiplex Control: Alternative 0 (GPIO) */

/*! @name PORT2_5 (number 19), F_IPMT
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITADC1PINS_F_IPMT_PORT PORT2               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITADC1PINS_F_IPMT_PIN 5U                   /*!<@brief PORT pin number */
#define BOARD_INITADC1PINS_F_IPMT_PIN_MASK (1U << 5U)      /*!<@brief PORT pin mask */
                                                           /* @} */

/*! @name PORT2_19 (number 27), FAN_IW
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITADC1PINS_FAN_IW_PORT PORT2                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITADC1PINS_FAN_IW_PIN 19U                   /*!<@brief PORT pin number */
#define BOARD_INITADC1PINS_FAN_IW_PIN_MASK (1U << 19U)      /*!<@brief PORT pin mask */
                                                            /* @} */

/*! @name PORT2_19 (number 27), FAN_IW
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITADC1PINS_OPAMP1_OUT_PORT PORT2                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITADC1PINS_OPAMP1_OUT_PIN 19U                   /*!<@brief PORT pin number */
#define BOARD_INITADC1PINS_OPAMP1_OUT_PIN_MASK (1U << 19U)      /*!<@brief PORT pin mask */
                                                                /* @} */

/*! @name PORT2_6 (number 20), FAN_IV
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITADC1PINS_FAN_IV_PORT PORT2               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITADC1PINS_FAN_IV_PIN 6U                   /*!<@brief PORT pin number */
#define BOARD_INITADC1PINS_FAN_IV_PIN_MASK (1U << 6U)      /*!<@brief PORT pin mask */
                                                           /* @} */

/*! @name PORT2_6 (number 20), FAN_IV
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITADC1PINS_OPAMP2_OUT_PORT PORT2               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITADC1PINS_OPAMP2_OUT_PIN 6U                   /*!<@brief PORT pin number */
#define BOARD_INITADC1PINS_OPAMP2_OUT_PIN_MASK (1U << 6U)      /*!<@brief PORT pin mask */
                                                               /* @} */

/*! @name PORT1_10 (number 4), UDCB
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITADC1PINS_UDCB_PORT PORT1                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITADC1PINS_UDCB_PIN 10U                   /*!<@brief PORT pin number */
#define BOARD_INITADC1PINS_UDCB_PIN_MASK (1U << 10U)      /*!<@brief PORT pin mask */
                                                          /* @} */

/*! @name PORT1_11 (number 5), PFC_I
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITADC1PINS_PFC_I_PORT PORT1                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITADC1PINS_PFC_I_PIN 11U                   /*!<@brief PORT pin number */
#define BOARD_INITADC1PINS_PFC_I_PIN_MASK (1U << 11U)      /*!<@brief PORT pin mask */
                                                           /* @} */

/*! @name PORT1_12 (number 6), VAC
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITADC1PINS_VAC_PORT PORT1                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITADC1PINS_VAC_PIN 12U                   /*!<@brief PORT pin number */
#define BOARD_INITADC1PINS_VAC_PIN_MASK (1U << 12U)      /*!<@brief PORT pin mask */
                                                         /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitADC1Pins(void);

#define PCR_IBE_ibe1 0x01u /*!<@brief Input Buffer Enable: Enables */

/*! @name PORT1_13 (number 7), RLY_IN
  @{ */

/* Symbols to be used with GPIO driver */
#define BOARD_INITGPIOPINS_RLY_IN_GPIO GPIO1                /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITGPIOPINS_RLY_IN_INIT_GPIO_VALUE 0U        /*!<@brief GPIO output initial state */
#define BOARD_INITGPIOPINS_RLY_IN_GPIO_PIN 13U              /*!<@brief GPIO pin number */
#define BOARD_INITGPIOPINS_RLY_IN_GPIO_PIN_MASK (1U << 13U) /*!<@brief GPIO pin mask */

/* Symbols to be used with PORT driver */
#define BOARD_INITGPIOPINS_RLY_IN_PORT PORT1                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITGPIOPINS_RLY_IN_PIN 13U                   /*!<@brief PORT pin number */
#define BOARD_INITGPIOPINS_RLY_IN_PIN_MASK (1U << 13U)      /*!<@brief PORT pin mask */
                                                            /* @} */

/*! @name PORT3_7 (number 43), WV4
  @{ */

/* Symbols to be used with GPIO driver */
#define BOARD_INITGPIOPINS_WV4_GPIO GPIO3               /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITGPIOPINS_WV4_INIT_GPIO_VALUE 0U       /*!<@brief GPIO output initial state */
#define BOARD_INITGPIOPINS_WV4_GPIO_PIN 7U              /*!<@brief GPIO pin number */
#define BOARD_INITGPIOPINS_WV4_GPIO_PIN_MASK (1U << 7U) /*!<@brief GPIO pin mask */

/* Symbols to be used with PORT driver */
#define BOARD_INITGPIOPINS_WV4_PORT PORT3               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITGPIOPINS_WV4_PIN 7U                   /*!<@brief PORT pin number */
#define BOARD_INITGPIOPINS_WV4_PIN_MASK (1U << 7U)      /*!<@brief PORT pin mask */
                                                        /* @} */

/*! @name PORT1_30 (number 9), EVALVE0
  @{ */

/* Symbols to be used with GPIO driver */
#define BOARD_INITGPIOPINS_EVALVE0_GPIO GPIO1                /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITGPIOPINS_EVALVE0_INIT_GPIO_VALUE 0U        /*!<@brief GPIO output initial state */
#define BOARD_INITGPIOPINS_EVALVE0_GPIO_PIN 30U              /*!<@brief GPIO pin number */
#define BOARD_INITGPIOPINS_EVALVE0_GPIO_PIN_MASK (1U << 30U) /*!<@brief GPIO pin mask */

/* Symbols to be used with PORT driver */
#define BOARD_INITGPIOPINS_EVALVE0_PORT PORT1                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITGPIOPINS_EVALVE0_PIN 30U                   /*!<@brief PORT pin number */
#define BOARD_INITGPIOPINS_EVALVE0_PIN_MASK (1U << 30U)      /*!<@brief PORT pin mask */
                                                             /* @} */

/*! @name PORT1_5 (number 63), EVALVE1
  @{ */

/* Symbols to be used with GPIO driver */
#define BOARD_INITGPIOPINS_EVALVE1_GPIO GPIO1               /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITGPIOPINS_EVALVE1_INIT_GPIO_VALUE 0U       /*!<@brief GPIO output initial state */
#define BOARD_INITGPIOPINS_EVALVE1_GPIO_PIN 5U              /*!<@brief GPIO pin number */
#define BOARD_INITGPIOPINS_EVALVE1_GPIO_PIN_MASK (1U << 5U) /*!<@brief GPIO pin mask */

/* Symbols to be used with PORT driver */
#define BOARD_INITGPIOPINS_EVALVE1_PORT PORT1               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITGPIOPINS_EVALVE1_PIN 5U                   /*!<@brief PORT pin number */
#define BOARD_INITGPIOPINS_EVALVE1_PIN_MASK (1U << 5U)      /*!<@brief PORT pin mask */
                                                            /* @} */

/*! @name PORT1_6 (number 64), EVALVE2
  @{ */

/* Symbols to be used with GPIO driver */
#define BOARD_INITGPIOPINS_EVALVE2_GPIO GPIO1               /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITGPIOPINS_EVALVE2_INIT_GPIO_VALUE 0U       /*!<@brief GPIO output initial state */
#define BOARD_INITGPIOPINS_EVALVE2_GPIO_PIN 6U              /*!<@brief GPIO pin number */
#define BOARD_INITGPIOPINS_EVALVE2_GPIO_PIN_MASK (1U << 6U) /*!<@brief GPIO pin mask */

/* Symbols to be used with PORT driver */
#define BOARD_INITGPIOPINS_EVALVE2_PORT PORT1               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITGPIOPINS_EVALVE2_PIN 6U                   /*!<@brief PORT pin number */
#define BOARD_INITGPIOPINS_EVALVE2_PIN_MASK (1U << 6U)      /*!<@brief PORT pin mask */
                                                            /* @} */

/*! @name PORT3_28 (number 33), EVALVE3
  @{ */

/* Symbols to be used with GPIO driver */
#define BOARD_INITGPIOPINS_EVALVE3_GPIO GPIO3                /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITGPIOPINS_EVALVE3_INIT_GPIO_VALUE 0U        /*!<@brief GPIO output initial state */
#define BOARD_INITGPIOPINS_EVALVE3_GPIO_PIN 28U              /*!<@brief GPIO pin number */
#define BOARD_INITGPIOPINS_EVALVE3_GPIO_PIN_MASK (1U << 28U) /*!<@brief GPIO pin mask */

/* Symbols to be used with PORT driver */
#define BOARD_INITGPIOPINS_EVALVE3_PORT PORT3                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITGPIOPINS_EVALVE3_PIN 28U                   /*!<@brief PORT pin number */
#define BOARD_INITGPIOPINS_EVALVE3_PIN_MASK (1U << 28U)      /*!<@brief PORT pin mask */
                                                             /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitGPIOPins(void);

#define PCR_IBE_ibe0 0x00u /*!<@brief Input Buffer Enable: Disables */

/*! @name PORT1_4 (number 62), PFC_OC
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITCMP0PINS_PFC_OC_PORT PORT1               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITCMP0PINS_PFC_OC_PIN 4U                   /*!<@brief PORT pin number */
#define BOARD_INITCMP0PINS_PFC_OC_PIN_MASK (1U << 4U)      /*!<@brief PORT pin mask */
                                                           /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitCMP0Pins(void);

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
