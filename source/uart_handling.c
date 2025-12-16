/*
 * uart.c
 *
 *  Created on: 10-Oct-2025
 *      Author: kashish.singh
 */

#include "uart_handling.h"
#include "PFC_statemachine.h"
#include "m1_sm_snsless.h"
#include <string.h>
#include <stdint.h>

#define MEGMEET 0 //Megmeet 1 for megmeet IDU
#define START_BYTE 0xAB
#define SYNC_BYTE 0x01
#define IDU_DATA_LENGTH 0x13



IDU_to_ODU_data_t rx_data;






/* Data test structure */
lpuart_transfer_t xfer;
IDU_Frame_t received_frame;

/* Megmeet Data structure */
lpuart_transfer_t megmeet_send0Xfer;
lpuart_transfer_t megmeet_receive0Xfer;


/* Rx and Tx buffer for Megmeet */
uint8_t g_Megmeettx0Buffer[MEGMEET_TX_BUFFER_SIZE] = { 0 };
uint8_t g_Megmeetrx0Buffer[MEGMEET_RX_BUFFER_SIZE] = { 0 };

/* UART0 Megmeet Flags*/
volatile bool rx0BufferEmpty = true;
volatile bool tx0BufferFull = false;
volatile bool tx0OnGoing = false;
volatile bool rx0OnGoing = false;

/* MEGMEET Rx buffer */
uint8_t g_megmeet_rx_buffer[25] = { 0 };
uint8_t g_megmeet_rx_buffer_index = 0;


uint8_t calculated_checksum = 0, received_checksum =0;


/* TopBand Data structure */
lpuart_transfer_t topband_send0Xfer;
lpuart_transfer_t topband_receive0Xfer;

/* TopBand Rx buffer*/
uint8_t g_topband_rx_buffer[25] = { 0 };
uint8_t g_topband_rx_buffer_index = 0;

/* Rx and Tx buffer for TopBand */
uint8_t g_Topbandtx0Buffer[TOPBAND_TX_BUFFER_SIZE] = {0};
uint8_t g_Topbandrx0Buffer[TOPBAND_RX_BUFFER_SIZE] = {0};


/* UART sm initial state */

uart_state_t uart_state = UART_STATE_TRANSMITTING;

static uint32_t uart_timeout_counter = 0;

/**
 * @brief Data format for Megmeet.
 *
 * This function describes the Megmeet UART Comm data format.
 * 24 data bytes
 * @param tx.
 *
 */

#ifdef MEGMEET
void MegmeetTxBufferInit(uint8_t *tx)
{


	        g_Megmeettx0Buffer[ODU_HEAD_CODE] = 0xAB;
		 	g_Megmeettx0Buffer[ODU_INFO_CODE] = 0x01;
		 	g_Megmeettx0Buffer[ODU_DATA_LENGTH] = 0x13;       // ODU to IDU
		 	g_Megmeettx0Buffer[ODU_MACHINE_TYPE] = 0x32;     // IDU address
		 	g_Megmeettx0Buffer[ODU_RUNNING_STATE] = 0x08;  //g_sM1Drive.sScalarCtrl.fltFreqCmd
//		 	g_Megmeettx0Buffer[ODU_COMPRESSOR_TARGET_FREQUENCY] = 0x00;
//	        g_Megmeettx0Buffer[ODU_COMPRESSOR_RUNNING_FREQUENCY] = 0x00;
//		 	g_Megmeettx0Buffer[ODU_OUTDOOR_ERROR_AND_PROTECTION] = 0x00; // Only lower 3 bits used
//		 	g_Megmeettx0Buffer[ODU_OUTDOOR_CONDENSOR_TEMPERATURE] = 0x59;
//		 	g_Megmeettx0Buffer[ODU_OUTDOOOR_AMBIENT_TEMPERATURE] = 0x59;
//		 	g_Megmeettx0Buffer[ODU_COMPRESSOR_DISCHARGE_TEMPERATURE] = 0x59;
//		 	g_Megmeettx0Buffer[ODU_COMPRESSOR_SUCTION_TEMPERATURE] = 0x59;
//		 	g_Megmeettx0Buffer[ODU_COMPRESSOR_PHASE_CURRENT] = 0x00;
//		 	g_Megmeettx0Buffer[ODU_AC_CURRENT] = 0x00;
//		 	g_Megmeettx0Buffer[ODU_DC_CURRENT] = 0x00;
//		 	g_Megmeettx0Buffer[ODU_EEV_OPEN_STEP] = 0x00;
//		 	g_Megmeettx0Buffer[ODU_OUTDOOR_AC_FAN_LEVEL] = 0x00;
//		 	g_Megmeettx0Buffer[ODU_RESERVE] = 0x00;
//		 	g_Megmeettx0Buffer[ODU_IPM_TEMPERATURE] = 0x59;
//		 	g_Megmeettx0Buffer[ODU_AC_VOLTAGE] = 0x00;
//		 	g_Megmeettx0Buffer[ODU_COMPRESSOR_FREQUENCY_LIMIT] = 0x00;
//		 	g_Megmeettx0Buffer[ODU_RESERVE] = 0x00;
//		 	g_Megmeettx0Buffer[ODU_VERIFY] = 0xF5;
//		 	g_Megmeettx0Buffer[ODU_END_CODE] = 0x55;
}


// void MegmeetRxBufferInit(uint8_t *rx)
// {
//
//	 g_Megmeetrx0Buffer[HEAD_CODE] = 0xAB;
//	 	g_Megmeetrx0Buffer[INFO_CODE] = 0x01;
//	 	g_Megmeetrx0Buffer[DATA_LENGTH] = 0x13;       // ODU to IDU
//	 	g_Megmeetrx0Buffer[HEAD] = 0x32;     // IDU address
//	 	g_Megmeetrx0Buffer[MODE_AND_FUNCTION] = 0x08;
//	 	g_Megmeetrx0Buffer[RESERVE1] = 0x00;
//	 	g_Megmeetrx0Buffer[SETTING_TEMP] = 0x00;
//	 	g_Megmeetrx0Buffer[INDOOR_ROOM_TEMP] = 0x00; // Only lower 3 bits used
//	 	g_Megmeetrx0Buffer[INDOOR_ROOM_TEMP_DECIMAL] = 0x59;
//	 	g_Megmeetrx0Buffer[INDOOR_EVAPORATOR_TEMP] = 0x59;
//	 	g_Megmeetrx0Buffer[INDOOR_FAV_LEVEL] = 0x59;
//	 	g_Megmeetrx0Buffer[INDOOR_FAN_RUNNING_SPEED] = 0x59;
//	 	g_Megmeetrx0Buffer[RESERVE2] = 0x00;
//	 	g_Megmeetrx0Buffer[BYTE_13] = 0x00;
//	 	g_Megmeetrx0Buffer[IDU_ERROR] = 0x00;
//	 	g_Megmeetrx0Buffer[HUMIDITY_VALUE] = 0x00;
//	 	g_Megmeetrx0Buffer[HUMIDITY_FUNCTION] = 0x00;
//	 	g_Megmeetrx0Buffer[HUMIDITY_ZONE1] = 0x00;
//	 	g_Megmeetrx0Buffer[HUMIDITY_ZONE2] = 0x59;
//	 	g_Megmeetrx0Buffer[BYTE19_5IN1] = 0x00;
//	 	g_Megmeetrx0Buffer[EEV_OPEN_STEP_HIGH] = 0x00;
//	 	g_Megmeetrx0Buffer[EEV_OPEN_STEP_LOW] = 0x00;
//	 	g_Megmeetrx0Buffer[VERIFY_VALUE] = 0xF5;
//	 	g_Megmeetrx0Buffer[END_CODE] = 0x55;
// }

#endif

/**
 * @brief Data format for TopBand.
 *
 * This function describes the TopBand UART Comm data format.
 * 29 data bytes
 * @param tx.
 *
 */
#ifdef TOPBAND

void TopbandBufferInit(uint8_t *tx)
{
    g_Topbandtx0Buffer[BYTE_SYNC_CODE_1] = 0xAA;
    g_Topbandtx0Buffer[BYTE_SYNC_CODE_2] = 0x55;

    g_Topbandtx0Buffer[BYTE_TOTAL_LENGTH] = 0x1E;
    g_Topbandtx0Buffer[BYTE_DATA_TYPE] = 0x00;           // ODU → IDU
    g_Topbandtx0Buffer[BYTE_TARGET_ADDRESS] = 0x01;      // IDU address
    g_Topbandtx0Buffer[BYTE_PAGE_ADDRESS] = 0x00;

    g_Topbandtx0Buffer[BYTE_ODU_STATUS] = 0x00;

    g_Topbandtx0Buffer[BYTE_COMP_FREQ_LOW] = 0x00;
    g_Topbandtx0Buffer[BYTE_COMP_FREQ_HIGH] = 0x00;      // Only lower 3 bits

    g_Topbandtx0Buffer[BYTE_ODU_FAULT_STATUS_1] = 0x00;
    g_Topbandtx0Buffer[BYTE_ODU_FAULT_STATUS_2] = 0x00;

    g_Topbandtx0Buffer[BYTE_ODU_PROTECT_STATUS_1] = 0x00;
    g_Topbandtx0Buffer[BYTE_ODU_PROTECT_STATUS_2] = 0x00;

    g_Topbandtx0Buffer[BYTE_TEMP_OUTDOOR_AMBIENT] = 0x70;
    g_Topbandtx0Buffer[BYTE_TEMP_COIL] = 0x72;
    g_Topbandtx0Buffer[BYTE_TEMP_CONDENSER_OUTLET] = 0x00;
    g_Topbandtx0Buffer[BYTE_TEMP_SUCTION] = 0x3C;
    g_Topbandtx0Buffer[BYTE_TEMP_DISCHARGE] = 0x34;
    g_Topbandtx0Buffer[BYTE_TEMP_IPM] = 0x00;

    g_Topbandtx0Buffer[BYTE_RESERVED_19] = 0x00;
    g_Topbandtx0Buffer[BYTE_RESERVED_20] = 0x00;

    g_Topbandtx0Buffer[BYTE_OUTDOOR_FAN_SPEED] = 0x00;
    g_Topbandtx0Buffer[BYTE_DC_VOLTAGE] = 0x92;

    g_Topbandtx0Buffer[BYTE_COMP_PHASE_CURRENT] = 0x00;
    g_Topbandtx0Buffer[BYTE_INPUT_AC_VOLT] = 0x6A;

    g_Topbandtx0Buffer[BYTE_INPUT_AC_CURRENT] = 0x00;   // If needed
    g_Topbandtx0Buffer[BYTE_EXTENSION_BITS] = 0x11;
    g_Topbandtx0Buffer[BYTE_ODU_EEV_OPENING] = 0x17;

    g_Topbandtx0Buffer[BYTE_RESERVED_28] = 0x00;

    g_Topbandtx0Buffer[BYTE_CHECKSUM] = 0x6B;           // To be calculated later
}

#endif

/**
 *
 * @brief Structure for MEGMEET UART data
 *
 *
 *
 * **/


uint8_t calculate_checksum(uint8_t *data)
{
  uint16_t checksum = 0;
  //  data[MODBUS_FRAME_CHECKSUM_BYTE] = 0;
  for (volatile int i = 0; i < TOPBAND_TX_BUFFER_SIZE - 2; i++)
  {
	checksum += data[i];
  }
  checksum &= 0xFF;
  return (uint8_t)checksum;
}

bool handle_tx_data(uint8_t *transmit_data)
{


}

bool decode_rx_data(uint8_t *received_data)
{


	if((received_data[HEAD_CODE]== START_BYTE ) && (received_data[INFO_CODE] == SYNC_BYTE ) && (received_data[DATA_LENGTH] == IDU_DATA_LENGTH))
	{
		received_checksum = received_data[VERIFY_VALUE];
		calculated_checksum =  calculate_checksum(received_data);
		//if(calculated_checksum == received_checksum)

			uint8_t byte4 = received_data[MODE_AND_FUNCTION];
			 received_frame.superSilent = byte4 && (1 << 7);
			 received_frame.eco = byte4 && (1 << 6);
			 received_frame.runningMode = byte4 && 0x07;

			 switch(received_frame.runningMode)
			 {
			 case 0:
				 received_frame.runningMode = STOP_MODE;
				  break;

			 case 1:
				 received_frame.runningMode = COOL_MODE;
				break;

			 case 2:
				 received_frame.runningMode = DRY_MODE;
				 break;

			 case 3:
				 received_frame.runningMode = HEAT_MODE;
				 break;

			 case 4:
				 received_frame.runningMode = FAN_MODE;
				 break;

			 }

			 received_frame.setTemp = received_data[SETTING_TEMP] + 64;
			 received_frame.roomTemp = received_data[INDOOR_ROOM_TEMP] + 64;
			 received_frame.roomTempDec = received_data[INDOOR_ROOM_TEMP_DECIMAL]+0.5;
			 received_frame.evapTemp = received_data[INDOOR_EVAPORATOR_TEMP] + 64;
			 uint8_t byte10 = received_data[INDOOR_FAV_LEVEL] && 0x07;

			 switch(byte10)
			 {
			 case 0:
				             received_frame.fanLevel = MODE_STOP;
				             break;
			 case 1:
							 received_frame.fanLevel = MODE_TINY;
							 break;
			 case 2:
							 received_frame.fanLevel = MODE_MUTE;
							 break;
			 case 3:
							 received_frame.fanLevel = MODE_LOW;
							 break;
			 case 4:
							 received_frame.fanLevel = MODE_MEDIUM;
							 break;
			 case 5:
							 received_frame.fanLevel = MODE_HIGH;
							 break;

			 }

			 received_frame.fanRPM = received_data[INDOOR_FAN_RUNNING_SPEED] / 10;
			 received_frame.fastCooling = (received_data[BYTE_13]>>5 ) && 0x01;
			 received_frame.constantMode = (received_data[BYTE_13] >> 6) && 0X01;
			 /*
			  * Error code bits yet to be written
			  *
			  * */
			 received_frame.errorCode = received_data[IDU_ERROR];
			 received_frame.humidityValue = received_data[HUMIDITY_VALUE];
			 received_frame.humidityZone1 = received_data[HUMIDITY_ZONE1];
			 received_frame.humidityZone2 = received_data[HUMIDITY_ZONE2];
			 uint8_t byte19 = received_data[BYTE19_5IN1];

			 switch(byte19)
			 {
			 case 0:
				 received_frame.byte19_5IN1 = CA;
				 break;
			 case 1:
				 received_frame.byte19_5IN1 = C1;
							 break;
			 case 2:
				 received_frame.byte19_5IN1 = C2;
							 break;
			 case 3:
				 received_frame.byte19_5IN1 = C3;
							 break;
			 case 4:
				 received_frame.byte19_5IN1 = C5;
							 break;

			 }

			 received_frame.eevStepHigh  = received_data[EEV_OPEN_STEP_HIGH];
			 received_frame.eevStepLow  = received_data[EEV_OPEN_STEP_LOW];
              return true;

	}
	else
		 return false;

}



void UART0_Callback(LPUART_Type *base, lpuart_handle_t *handle, status_t status,
		void *userData) {

#ifdef MEGMEET
	int32_t uart_interrupt_flag;
	uart_interrupt_flag = status;
	LPUART_ClearStatusFlags(LPUART0, status);
	userData = userData;

	//When Tx  is complete, move to receive state
	if (kStatus_LPUART_TxIdle == status) {

		uart_state = UART_STATE_RECEIVING;
	}

	//if Rx is complete, copy byte to buffer and receive upto 25 bytes
	if (kStatus_LPUART_RxIdle == status) {

		uart_timeout_counter = 0; // Reset timeout on received data


		g_megmeet_rx_buffer[g_megmeet_rx_buffer_index++] =	g_Megmeetrx0Buffer[MEGMEET_RX_BUFFER_SIZE - 1]; // Copy the last byte received to the modbus buffer
		if (g_megmeet_rx_buffer_index < MODBUS_RX_BUFFER_SIZE) {
			LPUART_TransferReceiveNonBlocking((LPUART_Type*) LPUART0,
					&LPUART0_handle, &megmeet_receive0Xfer, NULL);
			rx0BufferEmpty = false;
			rx0OnGoing = false;
		}

		else {
			// If the buffer is full, wait for the state machine to process the data and clear the buffer
			decode_rx_data(g_megmeet_rx_buffer);
			rx0BufferEmpty = true; // Set the RX buffer to empty state
			g_megmeet_rx_buffer_index = 0;

		}
	}
#endif

#ifdef TOPBAND

	if (kStatus_LPUART_TxIdle == status)
	{
			uart_state = UART_STATE_RECEIVING;
	}

	if (kStatus_LPUART_RxIdle == status)
	{

			uart_timeout_counter = 0; // Reset timeout on received data
			g_topband_rx_buffer[g_topband_rx_buffer_index++] = g_Topbandrx0Buffer[TOPBAND_RX_BUFFER_SIZE - 1];

			if (g_topband_rx_buffer_index < TOPBAND_RX_BUFFER_SIZE)
			{
				LPUART_TransferReceiveNonBlocking((LPUART_Type*) LPUART0,&LPUART0_handle, &topband_receive0Xfer, NULL);

			}

			else
			{
				// If the buffer is full, wait for the state machine to process the data and clear the buffer
				rx0BufferEmpty = true; // Set the RX buffer to empty state
				g_topband_rx_buffer_index = 0;

			}
		}


#endif

}

void uart0_sm(void) {

#ifdef MEGMEET
	if (uart_state == UART_STATE_TRANSMITTING) {
		MegmeetTxBufferInit(g_Megmeettx0Buffer);
		//g_Megmeettx0Buffer[BYTE_STOPBYTE] = STOP_BYTE;
		LPUART_TransferSendNonBlocking((LPUART_Type*) LPUART0, &LPUART0_handle,&megmeet_send0Xfer);

	}

	if (uart_state == UART_STATE_RECEIVING) {

		uart_timeout_counter++;
		rx0OnGoing = true;
		LPUART_TransferReceiveNonBlocking((LPUART_Type*) LPUART0,&LPUART0_handle, &megmeet_receive0Xfer, NULL);

		//if timeout counter exceeds TIMEOUT_MAX change state to transmitting and reset the index
		if (uart_timeout_counter >= UART_TIMEOUT_MAX) {
			g_megmeet_rx_buffer_index = 0;
			rx0BufferEmpty = true;
			rx0OnGoing = false;
			uart_state = UART_STATE_TRANSMITTING;
			uart_timeout_counter = 0;
		}
	}

#endif

#ifdef TOPBAND

	if(uart_state == UART_STATE_TRANSMITTING)
	{
		TopbandBufferInit(g_Topbandtx0Buffer);
		LPUART_TransferSendNonBlocking((LPUART_Type*) LPUART0, &LPUART0_handle,&topband_send0Xfer);

	}

	if(uart_state == UART_STATE_RECEIVING)
	{
		uart_timeout_counter++;

		LPUART_TransferReceiveNonBlocking((LPUART_Type*) LPUART0,&LPUART0_handle, &topband_receive0Xfer, NULL);

		if (uart_timeout_counter >= UART_TIMEOUT_MAX)
		{
			        g_topband_rx_buffer_index = 0;
					rx0BufferEmpty = true;
					rx0OnGoing = false;
					uart_state = UART_STATE_TRANSMITTING;
					uart_timeout_counter = 0;

		}
	}

#endif

}

void send_start_data() {
	uint8_t g_tipString[] = "Hello A\r\n";
	/* Send g_tipString out. */
	xfer.data = g_tipString;
	xfer.dataSize = sizeof(g_tipString) - 1;
	tx0OnGoing = true;
	// LPUART_WriteBlocking((LPUART_Type *)LPUART0, (const uint8_t *)&g_tipString, xfer.dataSize);
	LPUART_TransferSendNonBlocking((LPUART_Type*) LPUART0, &LPUART0_handle,
			&xfer);
	/* Wait send finished */
	while (tx0OnGoing) {

	}
	// dataSend = false;
	/* Start to echo. */
	megmeet_send0Xfer.data = g_Megmeettx0Buffer;
	megmeet_send0Xfer.dataSize = MEGMEET_TX_BUFFER_SIZE;
	megmeet_receive0Xfer.data = g_Megmeetrx0Buffer;
	megmeet_receive0Xfer.dataSize = MEGMEET_RX_BUFFER_SIZE;

}

void initialize_uart0_transfers() {
	megmeet_receive0Xfer.data = g_Megmeetrx0Buffer;
	megmeet_receive0Xfer.dataSize = MEGMEET_RX_BUFFER_SIZE;

	megmeet_send0Xfer.data = g_Megmeettx0Buffer;
	megmeet_send0Xfer.dataSize = MEGMEET_TX_BUFFER_SIZE;

	topband_receive0Xfer.data = g_Topbandrx0Buffer;
	topband_receive0Xfer.dataSize =TOPBAND_TX_BUFFER_SIZE;

	topband_send0Xfer.data = g_Topbandtx0Buffer;
	topband_send0Xfer.dataSize = TOPBAND_RX_BUFFER_SIZE;
}
