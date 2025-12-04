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

lpuart_transfer_t xfer;
lpuart_transfer_t send0Xfer;
lpuart_transfer_t receive0Xfer;

/* Rx and Tx buffer */
uint8_t g_tx0Buffer[LPUART0_TX_BUFFER_SIZE] = {0};
uint8_t g_rx0Buffer[LPUART0_RX_BUFFER_SIZE] = {0};

volatile bool rx0BufferEmpty            = true;
volatile bool tx0BufferFull             = false;
volatile bool tx0OnGoing                = false;
volatile bool rx0OnGoing                = false;

uint8_t g_modbus_rx_buffer[25] = {0};
uint8_t g_modbus_rx_buffer_index = 0;

/* UART State Machine Variables */

uart_state_t uart_state = UART_STATE_TRANSMITTING;

static uint32_t uart_timeout_counter = 0;

/*Message format*/
void uartBufferInit(uint8_t *tx)
{
	g_tx0Buffer[BYTE_SYNC1] = 0xAB;
	g_tx0Buffer[BYTE_TOTAL_LENGTH] = 0x01;
	g_tx0Buffer[BYTE_DATA_TYPE] = 0x13;       // ODU to IDU
	g_tx0Buffer[BYTE_TARGET_ADDR] = 0x32;     // IDU address
	g_tx0Buffer[BYTE_PAGE_ADDR] = 0x08;
	g_tx0Buffer[BYTE_ODU_STATUS] = 0x00;
	g_tx0Buffer[BYTE_COMP_FREQ_LOW] = 0x00;
	g_tx0Buffer[BYTE_COMP_FREQ_HIGH] = 0x00; // Only lower 3 bits used
	g_tx0Buffer[BYTE_FAULT_STATUS1] = 0x59;
	g_tx0Buffer[BYTE_FAULT_STATUS2] = 0x59;
	g_tx0Buffer[BYTE_PROTECT_STATUS1] = 0x59;
	g_tx0Buffer[BYTE_PROTECT_STATUS2] = 0x59;
	g_tx0Buffer[BYTE_AMBIENT_TEMP] = 0x00;
	g_tx0Buffer[BYTE_COIL_TEMP] = 0x00;
	g_tx0Buffer[BYTE_OUTLET_TEMP] = 0x00;
	g_tx0Buffer[BYTE_SUCTION_TEMP] = 0x00;
	g_tx0Buffer[BYTE_DISCHARGE_TEMP] = 0x00;
	g_tx0Buffer[BYTE_IPM_TEMP] = 0x00;

	g_tx0Buffer[BYTE_RESERVED_19] = 0x59;
	g_tx0Buffer[BYTE_RESERVED_20] = 0x00;

	g_tx0Buffer[BYTE_FAN_SPEED] = 0x00;
	g_tx0Buffer[BYTE_DC_VOLTAGE] = 0x00;
	g_tx0Buffer[BYTE_COMP_CURRENT] = 0xF5;
	g_tx0Buffer[BYTE_AC_VOLTAGE] = 0x55;
	g_tx0Buffer[BYTE_STOPBYTE] ;
}

void UART0_Callback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData)
{

	int32_t uart_interrupt_flag;
	uart_interrupt_flag = status;
	LPUART_ClearStatusFlags(LPUART0, status);
	userData = userData;

   //When Tx  is complete, move to receive state
	if (kStatus_LPUART_TxIdle == status)
	{

		uart_state = UART_STATE_RECEIVING;
	}

	//if Rx is complete, copy byte to buffer and receive upto 25 bytes
		if (kStatus_LPUART_RxIdle == status)
	{

		uart_timeout_counter = 0; // Reset timeout on received data
		g_modbus_rx_buffer[g_modbus_rx_buffer_index++] = g_rx0Buffer[LPUART0_RX_BUFFER_SIZE - 1]; // Copy the last byte received to the modbus buffer
		if(g_modbus_rx_buffer_index < MODBUS_RX_BUFFER_SIZE)
		{
			LPUART_TransferReceiveNonBlocking((LPUART_Type *)LPUART0, &LPUART0_handle, &receive0Xfer, NULL);
			rx0BufferEmpty = false;
			rx0OnGoing     = false;
		}

		else
		{
			// If the buffer is full, wait for the state machine to process the data and clear the buffer
			rx0BufferEmpty = true; // Set the RX buffer to empty state
			g_modbus_rx_buffer_index =0;


		}
	}
}

void uart0_sm(void)
{
	if(uart_state == UART_STATE_TRANSMITTING)
	{
	    uartBufferInit(g_tx0Buffer);
	g_tx0Buffer[BYTE_STOPBYTE] = STOP_BYTE;
		LPUART_TransferSendNonBlocking((LPUART_Type *)LPUART0, &LPUART0_handle, &send0Xfer);

	}

	if (uart_state == UART_STATE_RECEIVING)
	{
		uart_timeout_counter++;
		rx0OnGoing = true;
		LPUART_TransferReceiveNonBlocking((LPUART_Type *)LPUART0, &LPUART0_handle, &receive0Xfer, NULL);

		//if timeout counter exceeds TIMEOUT_MAX change state to transmitting and reset the index
		if (uart_timeout_counter >= UART_TIMEOUT_MAX)
		{
			g_modbus_rx_buffer_index = 0;
			rx0BufferEmpty = true;
			rx0OnGoing = false;
			uart_state = UART_STATE_TRANSMITTING;
			uart_timeout_counter = 0;
		}
	}
}

void send_start_data()
{
	uint8_t g_tipString[] =
			"Hello A\r\n";
	/* Send g_tipString out. */
	xfer.data     = g_tipString;
	xfer.dataSize = sizeof(g_tipString) - 1;
	tx0OnGoing     = true;
	// LPUART_WriteBlocking((LPUART_Type *)LPUART0, (const uint8_t *)&g_tipString, xfer.dataSize);
	LPUART_TransferSendNonBlocking((LPUART_Type *)LPUART0, &LPUART0_handle, &xfer);
	/* Wait send finished */
	while (tx0OnGoing)
	{

	}
	// dataSend = false;
	/* Start to echo. */
	send0Xfer.data        = g_tx0Buffer;
	send0Xfer.dataSize    = LPUART0_TX_BUFFER_SIZE;
	receive0Xfer.data     = g_rx0Buffer;
	receive0Xfer.dataSize = LPUART0_RX_BUFFER_SIZE;

}


void initialize_uart0_transfers()
{
	receive0Xfer.data     = g_rx0Buffer;
	receive0Xfer.dataSize = LPUART0_RX_BUFFER_SIZE;

	send0Xfer.data        = g_tx0Buffer;
	send0Xfer.dataSize    = LPUART0_TX_BUFFER_SIZE;
}
