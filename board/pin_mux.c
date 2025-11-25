/*
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
 
/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Pins v17.0
processor: MCXA346
package_id: MCXA346VLH
mcu_data: ksdk2_0
processor_version: 0.2412.70
pin_labels:
- {pin_num: '44', pin_signal: P3_6/CLKOUT/LPSPI1_PCS3/LPUART3_RTS_B/CT4_MAT2/PWM0_A3/PWM1_A0/SmartDMA_PIO6/FREQME_CLK_OUT1/ADC3_A12, label: LED_GREEN, identifier: LED_GREEN}
- {pin_num: '46', pin_signal: P3_0/WUU0_IN22/TRIG_IN0/LPUART3_RXD/CT_INP16/PWM0_A0/PWM1_X0/SmartDMA_PIO0, label: Compressor_UH, identifier: CompUH;Compressor_UH}
- {pin_num: '42', pin_signal: P3_8/WUU0_IN23/TRIG_IN3/LPSPI1_SDO/LPUART1_RXD/CT_INP4/PWM0_A1/LPUART5_CTS_B/SmartDMA_PIO8/CLKOUT, label: Compressor_VH, identifier: Compressor_VH}
- {pin_num: '40', pin_signal: P3_10/TRIG_IN5/LPSPI1_SCK/LPUART1_RTS_B/CT1_MAT0/PWM0_A2/LPUART5_TXD/SmartDMA_PIO10, label: Compressor_WH, identifier: Compressor_WH}
- {pin_num: '45', pin_signal: P3_1/TRIG_IN1/LPUART3_TXD/CT_INP17/PWM0_B0/PWM1_X1/SmartDMA_PIO1/FREQME_CLK_OUT0, label: Compressor_UL, identifier: Compressor_UL}
- {pin_num: '41', pin_signal: P3_9/TRIG_IN4/LPSPI1_SDI/LPUART1_TXD/CT_INP5/PWM0_B1/LPUART5_RTS_B/SmartDMA_PIO9, label: Compressor_VL, identifier: Compressor_VL}
- {pin_num: '39', pin_signal: P3_11/WUU0_IN24/TRIG_IN6/LPSPI1_PCS0/LPUART1_CTS_B/CT1_MAT1/PWM0_B2/LPUART5_RXD/SmartDMA_PIO11, label: Compressor_WL, identifier: Compressor_WL}
- {pin_num: '21', pin_signal: P2_7/TRIG_IN5/LPUART4_TXD/CT_INP19/CT1_MAT3/SmartDMA_PIO31/VREFI/ADC1_A7/ADC3_A7/ADC0_A7/ADC2_A7, label: Compressor_FO, identifier: Compressor_FO}
- {pin_num: '31', pin_signal: P3_30/TRIG_OUT6/LPI2C3_SCLS/LPUART4_RTS_B/CT0_MAT2/PWM1_A0/SmartDMA_PIO30/ADC1_A21, label: Fan_UH, identifier: Fan_UH}
- {pin_num: '36', pin_signal: P3_14/WUU0_IN25/LPUART2_RXD/LPUART3_CTS_B/CT_INP6/PWM0_X2/PWM1_A1/SmartDMA_PIO14/ADC3_A16, label: Fan_VH, identifier: Fan_VH}
- {pin_num: '38', pin_signal: P3_12/LPUART2_RTS_B/LPUART3_TXD/CT1_MAT2/PWM0_X0/PWM1_A2/SmartDMA_PIO12/ADC3_A14, label: Fan_WH, identifier: Fan_WH}
- {pin_num: '37', pin_signal: P3_13/LPUART2_CTS_B/LPUART3_RXD/CT1_MAT3/PWM0_X1/PWM1_B2/SmartDMA_PIO13/ADC3_A15, label: Fan_WL, identifier: Fan_WL}
- {pin_num: '35', pin_signal: P3_15/LPUART2_TXD/LPUART3_RTS_B/CT_INP7/PWM0_X3/PWM1_B1/SmartDMA_PIO15/ADC3_A17, label: Fan_VL, identifier: Fan_VL}
- {pin_num: '30', pin_signal: P3_31/LPTMR0_ALT2/TRIG_IN10/LPI2C3_SDAS/LPUART4_CTS_B/CT0_MAT3/PWM1_B0/SmartDMA_PIO31/ADC1_A20, label: Fan_UL, identifier: Fan_UL}
- {pin_num: '16', pin_signal: P2_2/TRIG_IN6/LPUART0_RTS_B/LPUART2_TXD/CT_INP12/CT2_MAT2/SmartDMA_PIO26/ADC0_A4/CMP0_IN0/DAC0_OUT, label: Fan_FO, identifier: Fan_FO}
- {pin_num: '34', pin_signal: P3_27/WUU0_IN30/TRIG_OUT7/LPI2C3_SCL/LPUART4_TXD/CT_INP13/CT3_MAT1/PWM1_A3/SmartDMA_PIO27, label: PFC_PWM, identifier: PFC_PWM}
- {pin_num: '18', pin_signal: P2_4/LPUART2_CTS_B/CT_INP14/CT1_MAT0/SmartDMA_PIO28/ADC0_A1/ADC2_A0/CMP2_IN0, label: ENV_TEMP, identifier: PFC_IDC;ENV_TEMP}
- {pin_num: '17', pin_signal: P2_3/WUU0_IN19/TRIG_IN7/LPUART0_CTS_B/LPUART2_RXD/CT_INP13/CT2_MAT3/SmartDMA_PIO27/ADC0_A3/CMP1_IN0/ADC1_A4, label: PIP_TEMP, identifier: UDCB;PIP_TEMP}
- {pin_num: '24', pin_signal: P2_15/WUU0_IN21/TRIG_OUT4/LPSPI1_SDI/LPUART1_RTS_B/CT4_MAT3/CT0_MAT2/SmartDMA_PIO18/OPAMP0_OUT/ADC0_A2, label: Compressor_IDC, identifier: Compressor_IDC;OPAMP_Comp;OPAMP_Comp_OUT;OPAMP0_OUT}
- {pin_num: '53', pin_signal: P0_6/ISPMODE_N/LPI2C0_HREQ/LPSPI0_PCS1/CT_INP2/SmartDMA_PIO2/CMP1_OUT/CLKOUT/ADC0_A15, label: C_IPMT, identifier: C_IPMT}
- {pin_num: '1', pin_signal: P1_7/WUU0_IN9/TRIG_OUT2/LPUART2_CTS_B/CT_INP7/CT4_MAT1/SmartDMA_PIO3/ADC0_A23, label: EXH_TEMP, identifier: EXH_TEMP}
- {pin_num: '19', pin_signal: P2_5/LPUART2_RTS_B/CT_INP15/CT1_MAT1/SmartDMA_PIO29/ADC1_A1/ADC3_A0, label: F_IPMT, identifier: F_IPMT}
- {pin_num: '27', pin_signal: P2_19/TRIG_OUT5/CT3_MAT3/SmartDMA_PIO21/ADC1_A2/OPAMP1_OUT, label: FAN_IW, identifier: FAN_IW;OPAMP1_OUT}
- {pin_num: '20', pin_signal: P2_6/TRIG_OUT4/LPSPI1_PCS1/LPUART4_RXD/CT_INP18/CT1_MAT2/SmartDMA_PIO30/ADC1_A3/ADC2_A2/OPAMP2_OUT, label: FAN_IV, identifier: FAN_IV;OPAMP2_OUT}
- {pin_num: '4', pin_signal: P1_10/LPUART1_RTS_B/LPI2C2_SDAS/CT2_MAT0/SmartDMA_PIO6/LPUART5_TXD/CAN0_TXD/ADC1_A8, label: UDCB, identifier: UDCB}
- {pin_num: '5', pin_signal: P1_11/WUU0_IN11/TRIG_OUT2/LPUART1_CTS_B/LPI2C2_SCLS/CT2_MAT1/SmartDMA_PIO7/LPUART5_RXD/CAN0_RXD/ADC1_A9, label: PFC_I, identifier: PFC_I}
- {pin_num: '6', pin_signal: P1_12/WUU0_IN12/LPI2C1_SDA/LPUART2_RXD/CT2_MAT2/SmartDMA_PIO8/LPUART5_CTS_B/ADC1_A10, label: VAC, identifier: VAC}
- {pin_num: '7', pin_signal: P1_13/TRIG_IN3/LPI2C1_SCL/LPUART2_TXD/CT2_MAT3/SmartDMA_PIO9/LPUART5_RTS_B/ADC1_A11, label: RLY_IN, identifier: RLY_IN}
- {pin_num: '43', pin_signal: P3_7/TRIG_IN2/LPSPI1_PCS2/LPUART3_CTS_B/CT4_MAT3/PWM0_B3/PWM1_B0/SmartDMA_PIO7/ADC3_A13, label: WV4, identifier: wn4;WV4}
- {pin_num: '9', pin_signal: P1_30/TRIG_OUT3/LPI2C0_SDA/CT_INP16/XTAL48M, label: EVALVE0, identifier: EVALVE0}
- {pin_num: '63', pin_signal: P1_5/FREQME_CLK_IN1/LPSPI0_PCS2/LPUART2_TXD/CT1_MAT3/SmartDMA_PIO1/ADC0_A21/CMP1_IN2, label: EVALVE1, identifier: EVALVE1}
- {pin_num: '64', pin_signal: P1_6/TRIG_IN2/LPSPI0_PCS1/LPUART2_RTS_B/CT_INP6/CT4_MAT0/SmartDMA_PIO2/ADC0_A22, label: EVALVE2, identifier: EVALVE2}
- {pin_num: '33', pin_signal: P3_28/WUU0_IN26/TRIG_IN11/LPI2C3_SDA/LPUART4_RXD/CT_INP12/CT3_MAT2/PWM1_B3/SmartDMA_PIO28, label: EVALVE3, identifier: EVALVE3}
- {pin_num: '62', pin_signal: P1_4/WUU0_IN8/FREQME_CLK_IN0/LPSPI0_PCS3/LPUART2_RXD/CT1_MAT2/SmartDMA_PIO0/ADC0_A20/CMP0_IN2, label: PFC_OC, identifier: PFC_OC}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

#include "fsl_common.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "fsl_inputmux.h"
#include "pin_mux.h"

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitBootPins
 * Description   : Calls initialization functions.
 *
 * END ****************************************************************************************************************/
void BOARD_InitBootPins(void)
{
    BOARD_InitFreeMASTER_UARTPins();
    BOARD_InitLEDsPins();
    BOARD_InitUARTPins();
    BOARD_InitPWM0Pins();
    BOARD_InitPWM1Pins();
    BOARD_InitADC0Pins();
    BOARD_InitADC1Pins();
    BOARD_InitGPIOPins();
    BOARD_InitCMP0Pins();
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitFreeMASTER_UARTPins:
- options: {callFromInitBoot: 'true', coreID: cm33_core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '3', peripheral: LPUART1, signal: TX, pin_signal: P1_9/FREQME_CLK_IN1/LPUART1_TXD/LPI2C2_SCL/CT_INP9/CT0_MAT3/SmartDMA_PIO5, drive_strength: low}
  - {pin_num: '2', peripheral: LPUART1, signal: RX, pin_signal: P1_8/WUU0_IN10/FREQME_CLK_IN0/LPUART1_RXD/LPI2C2_SDA/CT_INP8/CT0_MAT2/SmartDMA_PIO4}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitFreeMASTER_UARTPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitFreeMASTER_UARTPins(void)
{
    /* PORT1: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GatePORT1);
    /* LPUART1 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kLPUART1_RST_SHIFT_RSTn);
    /* PORT1 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kPORT1_RST_SHIFT_RSTn);

    /* PORT1_8 (pin 2) is configured as LPUART1_RXD */
    PORT_SetPinMux(PORT1, 8U, kPORT_MuxAlt2);

    PORT1->PCR[8] = ((PORT1->PCR[8] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_IBE_MASK)))

                     /* Input Buffer Enable: Enables. */
                     | PORT_PCR_IBE(PCR_IBE_ibe1));

    /* PORT1_9 (pin 3) is configured as LPUART1_TXD */
    PORT_SetPinMux(PORT1, 9U, kPORT_MuxAlt2);

    PORT1->PCR[9] = ((PORT1->PCR[9] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_DSE_MASK | PORT_PCR_IBE_MASK)))

                     /* Drive Strength Enable: Low. */
                     | PORT_PCR_DSE(PCR_DSE_dse0)

                     /* Input Buffer Enable: Enables. */
                     | PORT_PCR_IBE(PCR_IBE_ibe1));
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitLEDsPins:
- options: {callFromInitBoot: 'true', coreID: cm33_core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '44', peripheral: GPIO3, signal: 'GPIO, 6', pin_signal: P3_6/CLKOUT/LPSPI1_PCS3/LPUART3_RTS_B/CT4_MAT2/PWM0_A3/PWM1_A0/SmartDMA_PIO6/FREQME_CLK_OUT1/ADC3_A12,
    direction: OUTPUT}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitLEDsPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitLEDsPins(void)
{
    /* GPIO3: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GateGPIO3);
    /* PORT3: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GatePORT3);
    /* GPIO3 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kGPIO3_RST_SHIFT_RSTn);
    /* PORT3 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kPORT3_RST_SHIFT_RSTn);

    gpio_pin_config_t LED_GREEN_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO3_6 (pin 44)  */
    GPIO_PinInit(BOARD_INITLEDSPINS_LED_GREEN_GPIO, BOARD_INITLEDSPINS_LED_GREEN_PIN, &LED_GREEN_config);

    /* PORT3_6 (pin 44) is configured as P3_6 */
    PORT_SetPinMux(BOARD_INITLEDSPINS_LED_GREEN_PORT, BOARD_INITLEDSPINS_LED_GREEN_PIN, kPORT_MuxAlt0);

    PORT3->PCR[6] = ((PORT3->PCR[6] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_IBE_MASK)))

                     /* Input Buffer Enable: Enables. */
                     | PORT_PCR_IBE(PCR_IBE_ibe1));
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitUARTPins:
- options: {callFromInitBoot: 'true', coreID: cm33_core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '51', peripheral: LPUART0, signal: RX, pin_signal: P0_2/TDO/SWO/LPUART0_RXD/LPSPI0_SCK/CT0_MAT0/UTICK_CAP0/ADC2_A8}
  - {pin_num: '52', peripheral: LPUART0, signal: TX, pin_signal: P0_3/TDI/LPUART0_TXD/LPSPI0_SDO/CT0_MAT1/UTICK_CAP1/CMP0_OUT/CMP1_IN1/ADC0_A14}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitUARTPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitUARTPins(void)
{
    /* PORT0: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GatePORT0);
    /* LPUART0 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kLPUART0_RST_SHIFT_RSTn);
    /* PORT0 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kPORT0_RST_SHIFT_RSTn);

    /* PORT0_2 (pin 51) is configured as LPUART0_RXD */
    PORT_SetPinMux(PORT0, 2U, kPORT_MuxAlt2);

    PORT0->PCR[2] = ((PORT0->PCR[2] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_IBE_MASK)))

                     /* Input Buffer Enable: Enables. */
                     | PORT_PCR_IBE(PCR_IBE_ibe1));

    /* PORT0_3 (pin 52) is configured as LPUART0_TXD */
    PORT_SetPinMux(PORT0, 3U, kPORT_MuxAlt2);

    PORT0->PCR[3] = ((PORT0->PCR[3] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_IBE_MASK)))

                     /* Input Buffer Enable: Enables. */
                     | PORT_PCR_IBE(PCR_IBE_ibe1));
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitPWM0Pins:
- options: {callFromInitBoot: 'true', coreID: cm33_core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '42', peripheral: FlexPWM0, signal: 'A, 1', pin_signal: P3_8/WUU0_IN23/TRIG_IN3/LPSPI1_SDO/LPUART1_RXD/CT_INP4/PWM0_A1/LPUART5_CTS_B/SmartDMA_PIO8/CLKOUT,
    direction: OUTPUT}
  - {pin_num: '40', peripheral: FlexPWM0, signal: 'A, 2', pin_signal: P3_10/TRIG_IN5/LPSPI1_SCK/LPUART1_RTS_B/CT1_MAT0/PWM0_A2/LPUART5_TXD/SmartDMA_PIO10, direction: OUTPUT}
  - {pin_num: '45', peripheral: FlexPWM0, signal: 'B, 0', pin_signal: P3_1/TRIG_IN1/LPUART3_TXD/CT_INP17/PWM0_B0/PWM1_X1/SmartDMA_PIO1/FREQME_CLK_OUT0, direction: OUTPUT}
  - {pin_num: '41', peripheral: FlexPWM0, signal: 'B, 1', pin_signal: P3_9/TRIG_IN4/LPSPI1_SDI/LPUART1_TXD/CT_INP5/PWM0_B1/LPUART5_RTS_B/SmartDMA_PIO9, direction: OUTPUT}
  - {pin_num: '39', peripheral: FlexPWM0, signal: 'B, 2', pin_signal: P3_11/WUU0_IN24/TRIG_IN6/LPSPI1_PCS0/LPUART1_CTS_B/CT1_MAT1/PWM0_B2/LPUART5_RXD/SmartDMA_PIO11,
    direction: OUTPUT}
  - {pin_num: '21', peripheral: FlexPWM0, signal: 'PWM_FAULT_TRG_CH, 0', pin_signal: P2_7/TRIG_IN5/LPUART4_TXD/CT_INP19/CT1_MAT3/SmartDMA_PIO31/VREFI/ADC1_A7/ADC3_A7/ADC0_A7/ADC2_A7}
  - {pin_num: '46', peripheral: FlexPWM0, signal: 'A, 0', pin_signal: P3_0/WUU0_IN22/TRIG_IN0/LPUART3_RXD/CT_INP16/PWM0_A0/PWM1_X0/SmartDMA_PIO0, identifier: Compressor_UH,
    direction: OUTPUT}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitPWM0Pins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitPWM0Pins(void)
{
    /* INPUTMUX0: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GateINPUTMUX0);
    /* PORT2: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GatePORT2);
    /* PORT3: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GatePORT3);
    /* INPUTMUX0 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kINPUTMUX0_RST_SHIFT_RSTn);
    /* FLEXPWM0 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kFLEXPWM0_RST_SHIFT_RSTn);
    /* PORT2 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kPORT2_RST_SHIFT_RSTn);
    /* PORT3 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kPORT3_RST_SHIFT_RSTn);
    /*  TrigIn5 connect to FlexPwm0Fault 0 */
    INPUTMUX_AttachSignal(INPUTMUX0, 0U, kINPUTMUX_TrigIn5ToFlexPwm0Fault);

    /* PORT2_7 (pin 21) is configured as TRIG_IN5 */
    PORT_SetPinMux(BOARD_INITPWM0PINS_Compressor_FO_PORT, BOARD_INITPWM0PINS_Compressor_FO_PIN, kPORT_MuxAlt1);

    PORT2->PCR[7] = ((PORT2->PCR[7] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_IBE_MASK)))

                     /* Input Buffer Enable: Enables. */
                     | PORT_PCR_IBE(PCR_IBE_ibe1));

    /* PORT3_0 (pin 46) is configured as PWM0_A0 */
    PORT_SetPinMux(BOARD_INITPWM0PINS_Compressor_UH_PORT, BOARD_INITPWM0PINS_Compressor_UH_PIN, kPORT_MuxAlt5);

    PORT3->PCR[0] = ((PORT3->PCR[0] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_IBE_MASK)))

                     /* Input Buffer Enable: Enables. */
                     | PORT_PCR_IBE(PCR_IBE_ibe1));

    /* PORT3_1 (pin 45) is configured as PWM0_B0 */
    PORT_SetPinMux(BOARD_INITPWM0PINS_Compressor_UL_PORT, BOARD_INITPWM0PINS_Compressor_UL_PIN, kPORT_MuxAlt5);

    PORT3->PCR[1] = ((PORT3->PCR[1] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_IBE_MASK)))

                     /* Input Buffer Enable: Enables. */
                     | PORT_PCR_IBE(PCR_IBE_ibe1));

    /* PORT3_10 (pin 40) is configured as PWM0_A2 */
    PORT_SetPinMux(BOARD_INITPWM0PINS_Compressor_WH_PORT, BOARD_INITPWM0PINS_Compressor_WH_PIN, kPORT_MuxAlt5);

    PORT3->PCR[10] = ((PORT3->PCR[10] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_IBE_MASK)))

                      /* Input Buffer Enable: Enables. */
                      | PORT_PCR_IBE(PCR_IBE_ibe1));

    /* PORT3_11 (pin 39) is configured as PWM0_B2 */
    PORT_SetPinMux(BOARD_INITPWM0PINS_Compressor_WL_PORT, BOARD_INITPWM0PINS_Compressor_WL_PIN, kPORT_MuxAlt5);

    PORT3->PCR[11] = ((PORT3->PCR[11] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_IBE_MASK)))

                      /* Input Buffer Enable: Enables. */
                      | PORT_PCR_IBE(PCR_IBE_ibe1));

    /* PORT3_8 (pin 42) is configured as PWM0_A1 */
    PORT_SetPinMux(BOARD_INITPWM0PINS_Compressor_VH_PORT, BOARD_INITPWM0PINS_Compressor_VH_PIN, kPORT_MuxAlt5);

    PORT3->PCR[8] = ((PORT3->PCR[8] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_IBE_MASK)))

                     /* Input Buffer Enable: Enables. */
                     | PORT_PCR_IBE(PCR_IBE_ibe1));

    /* PORT3_9 (pin 41) is configured as PWM0_B1 */
    PORT_SetPinMux(BOARD_INITPWM0PINS_Compressor_VL_PORT, BOARD_INITPWM0PINS_Compressor_VL_PIN, kPORT_MuxAlt5);

    PORT3->PCR[9] = ((PORT3->PCR[9] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_IBE_MASK)))

                     /* Input Buffer Enable: Enables. */
                     | PORT_PCR_IBE(PCR_IBE_ibe1));
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitPWM1Pins:
- options: {callFromInitBoot: 'true', coreID: cm33_core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '31', peripheral: FlexPWM1, signal: 'A, 0', pin_signal: P3_30/TRIG_OUT6/LPI2C3_SCLS/LPUART4_RTS_B/CT0_MAT2/PWM1_A0/SmartDMA_PIO30/ADC1_A21, direction: OUTPUT}
  - {pin_num: '36', peripheral: FlexPWM1, signal: 'A, 1', pin_signal: P3_14/WUU0_IN25/LPUART2_RXD/LPUART3_CTS_B/CT_INP6/PWM0_X2/PWM1_A1/SmartDMA_PIO14/ADC3_A16, direction: OUTPUT}
  - {pin_num: '38', peripheral: FlexPWM1, signal: 'A, 2', pin_signal: P3_12/LPUART2_RTS_B/LPUART3_TXD/CT1_MAT2/PWM0_X0/PWM1_A2/SmartDMA_PIO12/ADC3_A14, direction: OUTPUT}
  - {pin_num: '30', peripheral: FlexPWM1, signal: 'B, 0', pin_signal: P3_31/LPTMR0_ALT2/TRIG_IN10/LPI2C3_SDAS/LPUART4_CTS_B/CT0_MAT3/PWM1_B0/SmartDMA_PIO31/ADC1_A20,
    direction: OUTPUT}
  - {pin_num: '35', peripheral: FlexPWM1, signal: 'B, 1', pin_signal: P3_15/LPUART2_TXD/LPUART3_RTS_B/CT_INP7/PWM0_X3/PWM1_B1/SmartDMA_PIO15/ADC3_A17, direction: OUTPUT}
  - {pin_num: '37', peripheral: FlexPWM1, signal: 'B, 2', pin_signal: P3_13/LPUART2_CTS_B/LPUART3_RXD/CT1_MAT3/PWM0_X1/PWM1_B2/SmartDMA_PIO13/ADC3_A15, direction: OUTPUT}
  - {pin_num: '16', peripheral: FlexPWM1, signal: 'PWM_FAULT_TRG_CH, 0', pin_signal: P2_2/TRIG_IN6/LPUART0_RTS_B/LPUART2_TXD/CT_INP12/CT2_MAT2/SmartDMA_PIO26/ADC0_A4/CMP0_IN0/DAC0_OUT}
  - {pin_num: '34', peripheral: FlexPWM1, signal: 'A, 3', pin_signal: P3_27/WUU0_IN30/TRIG_OUT7/LPI2C3_SCL/LPUART4_TXD/CT_INP13/CT3_MAT1/PWM1_A3/SmartDMA_PIO27, direction: OUTPUT}
  - {peripheral: FlexPWM1, signal: PWM_EXTFORCE_TRG, pin_signal: pwm0_sm3_mux_trig0}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitPWM1Pins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitPWM1Pins(void)
{
    /* INPUTMUX0: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GateINPUTMUX0);
    /* PORT2: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GatePORT2);
    /* PORT3: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GatePORT3);
    /* INPUTMUX0 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kINPUTMUX0_RST_SHIFT_RSTn);
    /* FLEXPWM1 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kFLEXPWM1_RST_SHIFT_RSTn);
    /* PORT2 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kPORT2_RST_SHIFT_RSTn);
    /* PORT3 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kPORT3_RST_SHIFT_RSTn);
    /*  TrigIn6 connect to FlexPwm1Fault 0 */
    INPUTMUX_AttachSignal(INPUTMUX0, 0U, kINPUTMUX_TrigIn6ToFlexPwm1Fault);
    /*  Pwm0Sm3OutTrig0 connect to FlexPwm1Force 0 */
    INPUTMUX_AttachSignal(INPUTMUX0, 0U, kINPUTMUX_Pwm0Sm3OutTrig0ToFlexPwm1Force);

    PORT2->PCR[2] = ((PORT2->PCR[2] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_MUX_MASK | PORT_PCR_IBE_MASK)))

                     /* Pin Multiplex Control: PORT2_2 (pin 16) is configured as TRIG_IN6. */
                     | PORT_PCR_MUX(PORT2_PCR2_MUX_mux01)

                     /* Input Buffer Enable: Enables. */
                     | PORT_PCR_IBE(PCR_IBE_ibe1));

    /* PORT3_12 (pin 38) is configured as PWM1_A2 */
    PORT_SetPinMux(BOARD_INITPWM1PINS_Fan_WH_PORT, BOARD_INITPWM1PINS_Fan_WH_PIN, kPORT_MuxAlt7);

    PORT3->PCR[12] = ((PORT3->PCR[12] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_IBE_MASK)))

                      /* Input Buffer Enable: Enables. */
                      | PORT_PCR_IBE(PCR_IBE_ibe1));

    /* PORT3_13 (pin 37) is configured as PWM1_B2 */
    PORT_SetPinMux(BOARD_INITPWM1PINS_Fan_WL_PORT, BOARD_INITPWM1PINS_Fan_WL_PIN, kPORT_MuxAlt7);

    PORT3->PCR[13] = ((PORT3->PCR[13] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_IBE_MASK)))

                      /* Input Buffer Enable: Enables. */
                      | PORT_PCR_IBE(PCR_IBE_ibe1));

    /* PORT3_14 (pin 36) is configured as PWM1_A1 */
    PORT_SetPinMux(BOARD_INITPWM1PINS_Fan_VH_PORT, BOARD_INITPWM1PINS_Fan_VH_PIN, kPORT_MuxAlt7);

    PORT3->PCR[14] = ((PORT3->PCR[14] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_IBE_MASK)))

                      /* Input Buffer Enable: Enables. */
                      | PORT_PCR_IBE(PCR_IBE_ibe1));

    /* PORT3_15 (pin 35) is configured as PWM1_B1 */
    PORT_SetPinMux(BOARD_INITPWM1PINS_Fan_VL_PORT, BOARD_INITPWM1PINS_Fan_VL_PIN, kPORT_MuxAlt7);

    PORT3->PCR[15] = ((PORT3->PCR[15] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_IBE_MASK)))

                      /* Input Buffer Enable: Enables. */
                      | PORT_PCR_IBE(PCR_IBE_ibe1));

    /* PORT3_27 (pin 34) is configured as PWM1_A3 */
    PORT_SetPinMux(BOARD_INITPWM1PINS_PFC_PWM_PORT, BOARD_INITPWM1PINS_PFC_PWM_PIN, kPORT_MuxAlt7);

    PORT3->PCR[27] = ((PORT3->PCR[27] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_IBE_MASK)))

                      /* Input Buffer Enable: Enables. */
                      | PORT_PCR_IBE(PCR_IBE_ibe1));

    /* PORT3_30 (pin 31) is configured as PWM1_A0 */
    PORT_SetPinMux(BOARD_INITPWM1PINS_Fan_UH_PORT, BOARD_INITPWM1PINS_Fan_UH_PIN, kPORT_MuxAlt7);

    PORT3->PCR[30] = ((PORT3->PCR[30] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_IBE_MASK)))

                      /* Input Buffer Enable: Enables. */
                      | PORT_PCR_IBE(PCR_IBE_ibe1));

    /* PORT3_31 (pin 30) is configured as PWM1_B0 */
    PORT_SetPinMux(BOARD_INITPWM1PINS_Fan_UL_PORT, BOARD_INITPWM1PINS_Fan_UL_PIN, kPORT_MuxAlt7);

    PORT3->PCR[31] = ((PORT3->PCR[31] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_IBE_MASK)))

                      /* Input Buffer Enable: Enables. */
                      | PORT_PCR_IBE(PCR_IBE_ibe1));
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitADC0Pins:
- options: {callFromInitBoot: 'true', coreID: cm33_core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '24', peripheral: ADC0, signal: 'A, 2', pin_signal: P2_15/WUU0_IN21/TRIG_OUT4/LPSPI1_SDI/LPUART1_RTS_B/CT4_MAT3/CT0_MAT2/SmartDMA_PIO18/OPAMP0_OUT/ADC0_A2,
    identifier: Compressor_IDC}
  - {pin_num: '24', peripheral: OPAMP0, signal: OUT, pin_signal: P2_15/WUU0_IN21/TRIG_OUT4/LPSPI1_SDI/LPUART1_RTS_B/CT4_MAT3/CT0_MAT2/SmartDMA_PIO18/OPAMP0_OUT/ADC0_A2,
    identifier: OPAMP0_OUT}
  - {pin_num: '18', peripheral: ADC0, signal: 'A, 1', pin_signal: P2_4/LPUART2_CTS_B/CT_INP14/CT1_MAT0/SmartDMA_PIO28/ADC0_A1/ADC2_A0/CMP2_IN0, identifier: ENV_TEMP}
  - {pin_num: '17', peripheral: ADC0, signal: 'A, 3', pin_signal: P2_3/WUU0_IN19/TRIG_IN7/LPUART0_CTS_B/LPUART2_RXD/CT_INP13/CT2_MAT3/SmartDMA_PIO27/ADC0_A3/CMP1_IN0/ADC1_A4,
    identifier: PIP_TEMP}
  - {pin_num: '53', peripheral: ADC0, signal: 'A, 15', pin_signal: P0_6/ISPMODE_N/LPI2C0_HREQ/LPSPI0_PCS1/CT_INP2/SmartDMA_PIO2/CMP1_OUT/CLKOUT/ADC0_A15}
  - {pin_num: '1', peripheral: ADC0, signal: 'A, 23', pin_signal: P1_7/WUU0_IN9/TRIG_OUT2/LPUART2_CTS_B/CT_INP7/CT4_MAT1/SmartDMA_PIO3/ADC0_A23}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitADC0Pins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitADC0Pins(void)
{
    /* PORT0: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GatePORT0);
    /* PORT1: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GatePORT1);
    /* PORT2: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GatePORT2);
    /* ADC0 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kADC0_RST_SHIFT_RSTn);
    /* PORT0 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kPORT0_RST_SHIFT_RSTn);
    /* PORT1 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kPORT1_RST_SHIFT_RSTn);
    /* OPAMP0 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kOPAMP0_RST_SHIFT_RSTn);
    /* PORT2 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kPORT2_RST_SHIFT_RSTn);

    /* PORT0_6 (pin 53) is configured as ADC0_A15 */
    PORT_SetPinMux(BOARD_INITADC0PINS_C_IPMT_PORT, BOARD_INITADC0PINS_C_IPMT_PIN, kPORT_MuxAlt0);

    PORT0->PCR[6] = ((PORT0->PCR[6] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_IBE_MASK)))

                     /* Input Buffer Enable: Disables. */
                     | PORT_PCR_IBE(PCR_IBE_ibe0));

    /* PORT1_7 (pin 1) is configured as ADC0_A23 */
    PORT_SetPinMux(BOARD_INITADC0PINS_EXH_TEMP_PORT, BOARD_INITADC0PINS_EXH_TEMP_PIN, kPORT_MuxAlt0);

    PORT1->PCR[7] = ((PORT1->PCR[7] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_IBE_MASK)))

                     /* Input Buffer Enable: Disables. */
                     | PORT_PCR_IBE(PCR_IBE_ibe0));

    /* PORT2_15 (pin 24) is configured as ADC0_A2, OPAMP0_OUT */
    PORT_SetPinMux(BOARD_INITADC0PINS_Compressor_IDC_PORT, BOARD_INITADC0PINS_Compressor_IDC_PIN, kPORT_MuxAlt0);

    PORT2->PCR[15] = ((PORT2->PCR[15] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_IBE_MASK)))

                      /* Input Buffer Enable: Disables. */
                      | PORT_PCR_IBE(PCR_IBE_ibe0));

    /* PORT2_3 (pin 17) is configured as ADC0_A3 */
    PORT_SetPinMux(BOARD_INITADC0PINS_PIP_TEMP_PORT, BOARD_INITADC0PINS_PIP_TEMP_PIN, kPORT_MuxAlt0);

    PORT2->PCR[3] = ((PORT2->PCR[3] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_IBE_MASK)))

                     /* Input Buffer Enable: Disables. */
                     | PORT_PCR_IBE(PCR_IBE_ibe0));

    PORT2->PCR[4] = ((PORT2->PCR[4] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_MUX_MASK | PORT_PCR_IBE_MASK)))

                     /* Pin Multiplex Control: PORT2_4 (pin 18) is configured as ADC0_A1. */
                     | PORT_PCR_MUX(PORT2_PCR4_MUX_mux00)

                     /* Input Buffer Enable: Disables. */
                     | PORT_PCR_IBE(PCR_IBE_ibe0));
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitADC1Pins:
- options: {callFromInitBoot: 'true', coreID: cm33_core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '19', peripheral: ADC1, signal: 'A, 1', pin_signal: P2_5/LPUART2_RTS_B/CT_INP15/CT1_MAT1/SmartDMA_PIO29/ADC1_A1/ADC3_A0}
  - {pin_num: '27', peripheral: ADC1, signal: 'A, 2', pin_signal: P2_19/TRIG_OUT5/CT3_MAT3/SmartDMA_PIO21/ADC1_A2/OPAMP1_OUT, identifier: FAN_IW}
  - {pin_num: '27', peripheral: OPAMP1, signal: OUT, pin_signal: P2_19/TRIG_OUT5/CT3_MAT3/SmartDMA_PIO21/ADC1_A2/OPAMP1_OUT, identifier: OPAMP1_OUT}
  - {pin_num: '20', peripheral: ADC1, signal: 'A, 3', pin_signal: P2_6/TRIG_OUT4/LPSPI1_PCS1/LPUART4_RXD/CT_INP18/CT1_MAT2/SmartDMA_PIO30/ADC1_A3/ADC2_A2/OPAMP2_OUT,
    identifier: FAN_IV}
  - {pin_num: '20', peripheral: OPAMP2, signal: OUT, pin_signal: P2_6/TRIG_OUT4/LPSPI1_PCS1/LPUART4_RXD/CT_INP18/CT1_MAT2/SmartDMA_PIO30/ADC1_A3/ADC2_A2/OPAMP2_OUT,
    identifier: OPAMP2_OUT}
  - {pin_num: '4', peripheral: ADC1, signal: 'A, 8', pin_signal: P1_10/LPUART1_RTS_B/LPI2C2_SDAS/CT2_MAT0/SmartDMA_PIO6/LPUART5_TXD/CAN0_TXD/ADC1_A8}
  - {pin_num: '5', peripheral: ADC1, signal: 'A, 9', pin_signal: P1_11/WUU0_IN11/TRIG_OUT2/LPUART1_CTS_B/LPI2C2_SCLS/CT2_MAT1/SmartDMA_PIO7/LPUART5_RXD/CAN0_RXD/ADC1_A9}
  - {pin_num: '6', peripheral: ADC1, signal: 'A, 10', pin_signal: P1_12/WUU0_IN12/LPI2C1_SDA/LPUART2_RXD/CT2_MAT2/SmartDMA_PIO8/LPUART5_CTS_B/ADC1_A10}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitADC1Pins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitADC1Pins(void)
{
    /* PORT1: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GatePORT1);
    /* PORT2: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GatePORT2);
    /* ADC1 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kADC1_RST_SHIFT_RSTn);
    /* PORT1 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kPORT1_RST_SHIFT_RSTn);
    /* PORT2 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kPORT2_RST_SHIFT_RSTn);

    /* PORT1_10 (pin 4) is configured as ADC1_A8 */
    PORT_SetPinMux(BOARD_INITADC1PINS_UDCB_PORT, BOARD_INITADC1PINS_UDCB_PIN, kPORT_MuxAlt0);

    PORT1->PCR[10] = ((PORT1->PCR[10] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_IBE_MASK)))

                      /* Input Buffer Enable: Disables. */
                      | PORT_PCR_IBE(PCR_IBE_ibe0));

    /* PORT1_11 (pin 5) is configured as ADC1_A9 */
    PORT_SetPinMux(BOARD_INITADC1PINS_PFC_I_PORT, BOARD_INITADC1PINS_PFC_I_PIN, kPORT_MuxAlt0);

    PORT1->PCR[11] = ((PORT1->PCR[11] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_IBE_MASK)))

                      /* Input Buffer Enable: Disables. */
                      | PORT_PCR_IBE(PCR_IBE_ibe0));

    /* PORT1_12 (pin 6) is configured as ADC1_A10 */
    PORT_SetPinMux(BOARD_INITADC1PINS_VAC_PORT, BOARD_INITADC1PINS_VAC_PIN, kPORT_MuxAlt0);

    PORT1->PCR[12] = ((PORT1->PCR[12] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_IBE_MASK)))

                      /* Input Buffer Enable: Disables. */
                      | PORT_PCR_IBE(PCR_IBE_ibe0));

    PORT2->PCR[19] = ((PORT2->PCR[19] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_MUX_MASK | PORT_PCR_IBE_MASK)))

                      /* Pin Multiplex Control: PORT2_19 (pin 27) is configured as ADC1_A2, OPAMP1_OUT. */
                      | PORT_PCR_MUX(PORT2_PCR19_MUX_mux00)

                      /* Input Buffer Enable: Disables. */
                      | PORT_PCR_IBE(PCR_IBE_ibe0));

    PORT2->PCR[5] = ((PORT2->PCR[5] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_MUX_MASK | PORT_PCR_IBE_MASK)))

                     /* Pin Multiplex Control: PORT2_5 (pin 19) is configured as ADC1_A1. */
                     | PORT_PCR_MUX(PORT2_PCR5_MUX_mux00)

                     /* Input Buffer Enable: Disables. */
                     | PORT_PCR_IBE(PCR_IBE_ibe0));

    PORT2->PCR[6] = ((PORT2->PCR[6] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_MUX_MASK | PORT_PCR_IBE_MASK)))

                     /* Pin Multiplex Control: PORT2_6 (pin 20) is configured as ADC1_A3, OPAMP2_OUT. */
                     | PORT_PCR_MUX(PORT2_PCR6_MUX_mux00)

                     /* Input Buffer Enable: Disables. */
                     | PORT_PCR_IBE(PCR_IBE_ibe0));
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitGPIOPins:
- options: {callFromInitBoot: 'true', coreID: cm33_core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '7', peripheral: GPIO1, signal: 'GPIO, 13', pin_signal: P1_13/TRIG_IN3/LPI2C1_SCL/LPUART2_TXD/CT2_MAT3/SmartDMA_PIO9/LPUART5_RTS_B/ADC1_A11, direction: OUTPUT}
  - {pin_num: '43', peripheral: GPIO3, signal: 'GPIO, 7', pin_signal: P3_7/TRIG_IN2/LPSPI1_PCS2/LPUART3_CTS_B/CT4_MAT3/PWM0_B3/PWM1_B0/SmartDMA_PIO7/ADC3_A13, identifier: WV4,
    direction: OUTPUT}
  - {pin_num: '9', peripheral: GPIO1, signal: 'GPIO, 30', pin_signal: P1_30/TRIG_OUT3/LPI2C0_SDA/CT_INP16/XTAL48M, direction: OUTPUT}
  - {pin_num: '63', peripheral: GPIO1, signal: 'GPIO, 5', pin_signal: P1_5/FREQME_CLK_IN1/LPSPI0_PCS2/LPUART2_TXD/CT1_MAT3/SmartDMA_PIO1/ADC0_A21/CMP1_IN2, direction: OUTPUT}
  - {pin_num: '64', peripheral: GPIO1, signal: 'GPIO, 6', pin_signal: P1_6/TRIG_IN2/LPSPI0_PCS1/LPUART2_RTS_B/CT_INP6/CT4_MAT0/SmartDMA_PIO2/ADC0_A22, direction: OUTPUT}
  - {pin_num: '33', peripheral: GPIO3, signal: 'GPIO, 28', pin_signal: P3_28/WUU0_IN26/TRIG_IN11/LPI2C3_SDA/LPUART4_RXD/CT_INP12/CT3_MAT2/PWM1_B3/SmartDMA_PIO28,
    direction: OUTPUT}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitGPIOPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitGPIOPins(void)
{
    /* GPIO1: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GateGPIO1);
    /* GPIO3: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GateGPIO3);
    /* PORT1: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GatePORT1);
    /* PORT3: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GatePORT3);
    /* GPIO1 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kGPIO1_RST_SHIFT_RSTn);
    /* GPIO3 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kGPIO3_RST_SHIFT_RSTn);
    /* PORT1 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kPORT1_RST_SHIFT_RSTn);
    /* PORT3 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kPORT3_RST_SHIFT_RSTn);

    gpio_pin_config_t EVALVE1_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO1_5 (pin 63)  */
    GPIO_PinInit(BOARD_INITGPIOPINS_EVALVE1_GPIO, BOARD_INITGPIOPINS_EVALVE1_PIN, &EVALVE1_config);

    gpio_pin_config_t EVALVE2_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO1_6 (pin 64)  */
    GPIO_PinInit(BOARD_INITGPIOPINS_EVALVE2_GPIO, BOARD_INITGPIOPINS_EVALVE2_PIN, &EVALVE2_config);

    gpio_pin_config_t RLY_IN_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO1_13 (pin 7)  */
    GPIO_PinInit(BOARD_INITGPIOPINS_RLY_IN_GPIO, BOARD_INITGPIOPINS_RLY_IN_PIN, &RLY_IN_config);

    gpio_pin_config_t EVALVE0_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO1_30 (pin 9)  */
    GPIO_PinInit(BOARD_INITGPIOPINS_EVALVE0_GPIO, BOARD_INITGPIOPINS_EVALVE0_PIN, &EVALVE0_config);

    gpio_pin_config_t WV4_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO3_7 (pin 43)  */
    GPIO_PinInit(BOARD_INITGPIOPINS_WV4_GPIO, BOARD_INITGPIOPINS_WV4_PIN, &WV4_config);

    gpio_pin_config_t EVALVE3_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO3_28 (pin 33)  */
    GPIO_PinInit(BOARD_INITGPIOPINS_EVALVE3_GPIO, BOARD_INITGPIOPINS_EVALVE3_PIN, &EVALVE3_config);

    /* PORT1_13 (pin 7) is configured as P1_13 */
    PORT_SetPinMux(BOARD_INITGPIOPINS_RLY_IN_PORT, BOARD_INITGPIOPINS_RLY_IN_PIN, kPORT_MuxAlt0);

    PORT1->PCR[13] = ((PORT1->PCR[13] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_IBE_MASK)))

                      /* Input Buffer Enable: Enables. */
                      | PORT_PCR_IBE(PCR_IBE_ibe1));

    /* PORT1_30 (pin 9) is configured as P1_30 */
    PORT_SetPinMux(BOARD_INITGPIOPINS_EVALVE0_PORT, BOARD_INITGPIOPINS_EVALVE0_PIN, kPORT_MuxAlt0);

    PORT1->PCR[30] = ((PORT1->PCR[30] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_IBE_MASK)))

                      /* Input Buffer Enable: Enables. */
                      | PORT_PCR_IBE(PCR_IBE_ibe1));

    /* PORT1_5 (pin 63) is configured as P1_5 */
    PORT_SetPinMux(BOARD_INITGPIOPINS_EVALVE1_PORT, BOARD_INITGPIOPINS_EVALVE1_PIN, kPORT_MuxAlt0);

    PORT1->PCR[5] = ((PORT1->PCR[5] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_IBE_MASK)))

                     /* Input Buffer Enable: Enables. */
                     | PORT_PCR_IBE(PCR_IBE_ibe1));

    /* PORT1_6 (pin 64) is configured as P1_6 */
    PORT_SetPinMux(BOARD_INITGPIOPINS_EVALVE2_PORT, BOARD_INITGPIOPINS_EVALVE2_PIN, kPORT_MuxAlt0);

    PORT1->PCR[6] = ((PORT1->PCR[6] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_IBE_MASK)))

                     /* Input Buffer Enable: Enables. */
                     | PORT_PCR_IBE(PCR_IBE_ibe1));

    /* PORT3_28 (pin 33) is configured as P3_28 */
    PORT_SetPinMux(BOARD_INITGPIOPINS_EVALVE3_PORT, BOARD_INITGPIOPINS_EVALVE3_PIN, kPORT_MuxAlt0);

    PORT3->PCR[28] = ((PORT3->PCR[28] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_IBE_MASK)))

                      /* Input Buffer Enable: Enables. */
                      | PORT_PCR_IBE(PCR_IBE_ibe1));

    /* PORT3_7 (pin 43) is configured as P3_7 */
    PORT_SetPinMux(BOARD_INITGPIOPINS_WV4_PORT, BOARD_INITGPIOPINS_WV4_PIN, kPORT_MuxAlt0);

    PORT3->PCR[7] = ((PORT3->PCR[7] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_IBE_MASK)))

                     /* Input Buffer Enable: Enables. */
                     | PORT_PCR_IBE(PCR_IBE_ibe1));
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitCMP0Pins:
- options: {callFromInitBoot: 'true', coreID: cm33_core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '62', peripheral: CMP0, signal: 'IN, 2N', pin_signal: P1_4/WUU0_IN8/FREQME_CLK_IN0/LPSPI0_PCS3/LPUART2_RXD/CT1_MAT2/SmartDMA_PIO0/ADC0_A20/CMP0_IN2}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitCMP0Pins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitCMP0Pins(void)
{
    /* PORT1: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GatePORT1);
    /* PORT1 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kPORT1_RST_SHIFT_RSTn);

    /* PORT1_4 (pin 62) is configured as CMP0_IN2 */
    PORT_SetPinMux(BOARD_INITCMP0PINS_PFC_OC_PORT, BOARD_INITCMP0PINS_PFC_OC_PIN, kPORT_MuxAlt0);

    PORT1->PCR[4] = ((PORT1->PCR[4] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_IBE_MASK)))

                     /* Input Buffer Enable: Disables. */
                     | PORT_PCR_IBE(PCR_IBE_ibe0));
}
/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
