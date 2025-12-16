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
#define TOPBAND_RX_BUFFER_SIZE 30

#define SW_VERSION_MAJOR	20
#define SW_VERSION_MINOR	50
#define DEVICE_ID			1354
//#define UART_IDLE_LINE_THRESHOLD 10 // Threshold for idle line detection
#define START_BYTE 0xAA
#define STOP_BYTE 0xFF
#define MODBUS_START_BYTE 0xba
//#define MEGMEET_TX_BUFFER_SIZE 25
//#define MEGMEET_RX_BUFFER_SIZE 1


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

typedef enum
{
	MODE_STOP,
	MODE_TINY,
	MODE_MUTE,
	MODE_LOW,
	MODE_MEDIUM,
	MODE_HIGH,
}megmeet_fan_level_t;

typedef enum
{
 	STOP_MODE,
	COOL_MODE,
	DRY_MODE,
	HEAT_MODE,
	FAN_MODE,
}megmeet_running_mode_t;

typedef enum
{
	 four_way_valve,
     outdoor_fan_type,
	 //run_mode,
	 stop_mode,
	 dry_mode,
	 cool_mode,
	 heat_mode,

} running_state_modes_t;

typedef enum
{
	good_running_ODU,
	IDU_EE_fault,
	indoor_fan_fault,
	zeroCrossing_detection_fault,
	indoorEvap_tempSensor_fault,
	refrigant_protection_fault,
	wifi_module_fault,
	voice_module_fault,
	dust_sensor_fault,
	humidity_sensor_fault,

}IDU_error_t;



typedef enum
{
	low,
	medium,
	high,

}fan_Speed_level_t;

typedef enum
{
   CA,
   C1,
   C2,
   C3,
   C4,
   C5,
}byte19_5in1_t;

typedef struct
{
	    uint8_t startBit;
	    uint8_t infoCode;
	    uint8_t dataLength;
	    uint8_t machineType;
	    uint8_t superSilent;
	    uint8_t eco;
	    megmeet_running_mode_t runningMode;
	    float setTemp;
	    float roomTemp;
	    float roomTempDec;
	    float evapTemp;
	    megmeet_fan_level_t fanLevel;
	    uint16_t fanRPM;
	    uint8_t fastCooling;
	    uint8_t constantMode;
	    IDU_error_t errorCode;
	    uint8_t humidityValue;
	    uint8_t humidityFunction;
	    uint8_t humidityZone1;
	    uint8_t humidityZone2;
	    byte19_5in1_t byte19_5IN1;
	    uint8_t eevStepHigh;
	    uint8_t eevStepLow;
	    uint8_t verifyOK;

}IDU_Frame_t;



typedef enum
{
   	good_running_IDU,
	ODU_EE_fault,
	reserve,
	IDU_and_ODU_comm_fault,
	Compressor_starting_abnormal,
	compressor_out_of_Steo_fault,
	Discharge_temperature_sensor_fault,
	outdoor_condensor_temp_sensor_fault,
	outdoor_ambient_temp_sensor_fault,


}byte7_fault_t;

typedef struct
{
    uint8_t head_code;                 // Byte 0 (0xAF)
    uint8_t info_code;                 // Byte 1
    uint8_t data_length;               // Byte 2 (always 19)
    uint8_t machine_type;              // Byte 3

    running_state_modes_t running_state;             // Byte 4 (bit-packed)
    uint8_t comp_target_freq;          // Byte 5
    uint8_t comp_running_freq;         // Byte 6

    byte7_fault_t outdoor_err_prot;          // Byte 7 (bit-packed)

    uint8_t outdoor_cond_temp;         // Byte 8
    uint8_t outdoor_amb_temp;          // Byte 9
    uint8_t discharge_temp;            // Byte 10
    uint8_t suction_temp;              // Byte 11
    uint8_t comp_phase_current;        // Byte 12
    uint8_t ac_current;                // Byte 13
    uint8_t dc_current;                // Byte 14
    uint8_t eev_step;                  // Byte 15
    uint8_t fan_speed;                 // Byte 16

    uint8_t reserve_17;                // Byte 17 (0)
    uint8_t ipm_temp;                  // Byte 18
    uint8_t ac_voltage;                // Byte 19

    uint8_t freq_limit_flags;          // Byte 20 (bit-packed)
    uint8_t reserve_21;                // Byte 21 (0)

    uint8_t verify_value;              // Byte 22 (checksum rule)
    uint8_t end_code;                  // Byte 23 (0x88)
} ODU_TxFrame_t;



typedef enum
{

	     ODU_HEAD_CODE,
		 ODU_INFO_CODE,
		 ODU_DATA_LENGTH,
		 ODU_MACHINE_TYPE,
		 ODU_RUNNING_STATE,
		 ODU_COMPRESSOR_TARGET_FREQUENCY,
		 ODU_COMPRESSOR_RUNNING_FREQUENCY,
		 ODU_OUTDOOR_ERROR_AND_PROTECTION,
		 ODU_OUTDOOR_CONDENSOR_TEMPERATURE,
		 ODU_OUTDOOOR_AMBIENT_TEMPERATURE,
		 ODU_COMPRESSOR_DISCHARGE_TEMPERATURE,
		 ODU_COMPRESSOR_SUCTION_TEMPERATURE,
		 ODU_COMPRESSOR_PHASE_CURRENT,
		 ODU_AC_CURRENT,
		 ODU_DC_CURRENT,
		 ODU_EEV_OPEN_STEP,
		 ODU_OUTDOOR_AC_FAN_LEVEL,
		 ODU_RESERVE1,
		 ODU_IPM_TEMPERATURE,
		 ODU_AC_VOLTAGE,
		 ODU_COMPRESSOR_FREQUENCY_LIMIT,
		 ODU_RESERVE2,
		 ODU_VERIFY,
		 ODU_END_CODE,

}ODU_to_IDU_data_t;

typedef enum
{
	HEAD_CODE,
	INFO_CODE,
	DATA_LENGTH,
    HEAD,
	MODE_AND_FUNCTION,
	RESERVE1,
	SETTING_TEMP,
	INDOOR_ROOM_TEMP,
	INDOOR_ROOM_TEMP_DECIMAL,
    INDOOR_EVAPORATOR_TEMP,
	INDOOR_FAV_LEVEL,
	INDOOR_FAN_RUNNING_SPEED,
	RESERVE2,
	BYTE_13,
	IDU_ERROR,
	HUMIDITY_VALUE,
	HUMIDITY_FUNCTION,
	HUMIDITY_ZONE1,
	HUMIDITY_ZONE2,
	BYTE19_5IN1,
	EEV_OPEN_STEP_HIGH,
	EEV_OPEN_STEP_LOW,
	VERIFY_VALUE,
	END_CODE,

}IDU_to_ODU_data_t;


//typedef enum
//{
//    BYTE_SYNC_CODE_1 ,   // 0xAA
//    BYTE_SYNC_CODU_AC_CURRENTODE_2 ,   // 0x55
//    BYTE_TOTAL_LENGTH,   // 0x14
//    BYTE_DATA_TYPE ,   // 0x00=ODU↔IDU, 0x28=Fixture↔IDU
//    BYTE_TARGET_ADDRESS,   // IDU address
//    BYTE_PAGE_ADDRESS,   // 0x10
//    BYTE_ODU_STATUS,
//    BYTE_COMP_FREQ_LOW,   // freq*10
//    BYTE_COMP_FREQ_HIGH,   // high 3 bits
//    BYTE_ODU_FAULT_STATUS_1,
//    BYTE_ODU_FAULT_STATUS_2,
//    BYTE_ODU_PROTECT_STATUS_1,
//    BYTE_ODU_PROTECT_STATUS_2,
//    BYTE_TEMP_OUTDOOR_AMBIENT,  // *0.5 - 30 offset
//    BYTE_TEMP_COIL ,
//    BYTE_TEMP_CONDENSER_OUTLET,
//    BYTE_TEMP_SUCTION ,
//    BYTE_TEMP_DISCHARGE,  // *0.5 no offset
//    BYTE_TEMP_IPM ,
//    BYTE_RESERVED_19,
//    BYTE_RESERVED_20,
//    BYTE_OUTDOOR_FAN_SPEED,  // Tap:0–3, DC:RPM/10
//    BYTE_DC_VOLTAGE ,  // V
//    BYTE_COMP_PHASE_CURRENT,  // A*10
//    BYTE_INPUT_AC_VOLT  ,  // V
//    BYTE_INPUT_AC_CURRENT,  // A*10
//    BYTE_EXTENSION_BITS ,
//    BYTE_ODU_EEV_OPENING,  // Opening/2
//    BYTE_RESERVED_28 ,  // always 0x00
//    BYTE_CHECKSUM,
//
//   } odu_topband_frame_byte_t;



typedef enum {

    UART_STATE_IDLE,
    UART_STATE_RECEIVING,
    UART_STATE_TRANSMITTING,

} uart_state_t;


//#define UART_TIMEOUT_MAX 280
#define UART_TIMEOUT_MAX 500

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
