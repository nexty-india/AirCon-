/*
 * user_uart_handling.h
 *
 *  Created on: 01-Jul-2025
 *      Author: Shashank
 */

#ifndef USER_UART_HANDLING_H_
#define USER_UART_HANDLING_H_

#include "fsl_lpuart.h"
#include "peripherals.h"

#define MODBUS_RX_BUFFER_SIZE 25// Size of the receive buffer

#define SW_VERSION_MAJOR	20
#define SW_VERSION_MINOR	50
#define DEVICE_ID			1354
//#define UART_IDLE_LINE_THRESHOLD 10 // Threshold for idle line detection
#define START_BYTE 0xAA
#define STOP_BYTE 0xFF
#define MODBUS_START_BYTE 0xba
//#define LPUART0_TX_BUFFER_SIZE 25
//#define LPUART0_RX_BUFFER_SIZE 1


typedef enum {
    MODBUS_FRAME_START_BYTE = 0,
    MODBUS_FRAME_READ_WRITE_BYTE,
    MODBUS_FRAME_INDEX_BYTE,
    MODBUS_FRAME_DATA1_BYTE,
    MODBUS_FRAME_DATA0_BYTE,
    MODBUS_FRAME_CHECKSUM_BYTE,
    MODBUS_FRAME_STOP_BYTE,
    MODBUS_FRAME_SIZE,
} modbus_frame_t;

typedef enum {
    MODBUS_FRAME_READ = 0x05, // Read operation
    MODBUS_FRAME_WRITE = 0x06 // Write operation
} modbus_frame_operation_t;

typedef enum {

	    BYTE_SYNC1 = 0x00,
	    BYTE_TOTAL_LENGTH ,
	    BYTE_DATA_TYPE ,
	    BYTE_TARGET_ADDR ,
	    BYTE_PAGE_ADDR ,
	    BYTE_ODU_STATUS,
	    BYTE_COMP_FREQ_LOW,
	    BYTE_COMP_FREQ_HIGH,
	    BYTE_FAULT_STATUS1,
	    BYTE_FAULT_STATUS2,
	    BYTE_PROTECT_STATUS1,
	    BYTE_PROTECT_STATUS2,
	    BYTE_AMBIENT_TEMP,
	    BYTE_COIL_TEMP,
	    BYTE_OUTLET_TEMP,
	    BYTE_SUCTION_TEMP,
	    BYTE_DISCHARGE_TEMP,
	    BYTE_IPM_TEMP,
	    BYTE_RESERVED_19,
	    BYTE_RESERVED_20,
	    BYTE_FAN_SPEED,
	    BYTE_DC_VOLTAGE,
	    BYTE_COMP_CURRENT,
	    BYTE_AC_VOLTAGE,
		BYTE_STOPBYTE

}uart_byte_index_megmeet_t;



typedef enum {

    UART_STATE_IDLE,
    UART_STATE_RECEIVING,
    UART_STATE_TRANSMITTING,

} uart_state_t;


//#define UART_TIMEOUT_MAX 280
#define UART_TIMEOUT_MAX 280

void uart0_sm(void);
void initialize_uart0_transfers(void);
void send_start_data(void);
void transmitData();
bool msElapsed();
// Function to decode received data and perform actions based on it.
// Also handles the sending of data back to the UART.
//bool decode_and_handle_uart_data(uint8_t *recieved_data, uint8_t *transmit_data);

uint8_t calculate_checksum(uint8_t *data);

extern uart_state_t uart_state;
extern  volatile uint16_t uart_delay_counter;   // 1 ms timer counter for delay
extern  volatile bool uart_delay_active;
extern volatile uint8_t uart_idle_line_counter;

#endif /* USER_UART_HANDLING_H_ */
