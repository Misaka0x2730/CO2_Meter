#ifndef __MODBUS_PARAM_H
#define __MODBUS_PARAM_H

#include "stm32l152.h"
#include "types.h"

#define MODBUS_TCP 								((u16_t)0)
#define MODBUS_RTU 								((u16_t)1)

#define MODBUS_DEVICE_ON						((u8_t)0xFF)

#define MODBUS_TCP_PORT							((u16_t)502)

#define MODBUS_MBAP_LENGTH 						((u8_t)7)
#define MODBUS_ERROR_ADU_LENGTH 				((u8_t)5)
#define MODBUS_ERROR_DATA_LENGTH 				((u8_t)2)
#define MODBUS_RTU_ADU_MAX_SIZE 				((u16_t)256)
#define MODBUS_RTU_MIN_DATA_SIZE				((u16_t)4) //slave address + cmd + crc-16
#define MODBUS_TCP_MAX_ADU_SIZE 				((u16_t)260)
#define MODBUS_MAX_ADU_SIZE 					(MODBUS_TCP_MAX_ADU_SIZE)
#define MODBUS_MAX_DATA_SIZE 					((u16_t)253)

#define	MODBUS_READ_COILS 						((u8_t)0x01)
#define	MODBUS_READ_DISCRETE_INPUTS 			((u8_t)0x02)
#define	MODBUS_SET_MULTIPLE_COILS 				((u8_t)0x0F)
#define	MODBUS_READ_INPUT_REGISTERS 			((u8_t)0x04)
#define	MODBUS_READ_HOLDING_REGISTERS 			((u8_t)0x03)
#define MODBUS_SET_ONE_COIL						((u8_t)0x05)
#define MODBUS_WRITE_ONE_HOLDING_REGISTER 		((u8_t)0x06)
#define	MODBUS_WRITE_MULTIPLE_HOLDING_REGISTERS ((u8_t)0x10)
#define MODBUS_READ_DEVICE_INFO					((u8_t)0x11)
#define MODBUS_LOOP_TEST						((u8_t)0x46)
#define MODBUS_GET_ID							((u8_t)0x45)
#define MODBUS_READ_DIP_SWITCH_STATE			((u8_t)0x44)

#define MODBUS_IS_CMD(CMD)						((CMD == MODBUS_READ_COILS) || (CMD == MODBUS_READ_DISCRETE_INPUTS) \
												|| (CMD == MODBUS_SET_MULTIPLE_COILS) || (CMD == MODBUS_READ_INPUT_REGISTERS)\
												|| (CMD == MODBUS_READ_HOLDING_REGISTERS) || (CMD == MODBUS_SET_ONE_COIL)\
												|| (CMD == MODBUS_WRITE_ONE_HOLDING_REGISTER) || (CMD == MODBUS_WRITE_MULTIPLE_HOLDING_REGISTERS)\
												|| (CMD == MODBUS_READ_DEVICE_INFO) || (CMD == MODBUS_LOOP_TEST)\
												|| (CMD == MODBUS_GET_ID) || (CMD == MODBUS_READ_DIP_SWITCH_STATE))

#define MODBUS_ERROR_OK 						((u8_t)0x00)
#define	MODBUS_ILLEGAL_FUNCTION 				((u8_t)0x01)
#define	MODBUS_ILLEGAL_DATA_ADDRESS 			((u8_t)0x02)
#define	MODBUS_ILLEGAL_DATA_VALUE 				((u8_t)0x03)
#define MODBUS_UNDEFINED_ERROR 					((u8_t)0x04)
#define MODBUS_ERROR_CRC	   					((u8_t)0x05)
#define MODBUS_ERROR_MASK 						((u8_t)0x80)

#define COILS_MAX_AMOUNT_WRITE 					((u16_t)0x07B0)
#define COILS_MAX_AMOUNT_READ 					((u16_t)0x07D0)

#define MODBUS_RTU_SOFT_ADDRESS_MODE			((u8_t)0x00)
#define MODBUS_RTU_HARD_ADDRESS_MODE			((u8_t)0x01)

#define INPUT_AMOUNT							((u8_t)3)
#define INPUT_TEMPERATURE						((u8_t)0)
#define INPUT_RH								((u8_t)1)
#define INPUT_ECO2								((u8_t)2)

struct ModbusPdu
{
	u8_t  slaveAddress;
	u8_t  cmd;
	u16_t address;
	u8_t  numberOfBytes;
	u16_t numberOfOutputs;
	u8_t  data[MODBUS_MAX_DATA_SIZE];
	u8_t  length;
};

struct ModbusAdu
{
	u16_t length;
	u8_t bytes[MODBUS_MAX_ADU_SIZE];
};

u8_t Modbus_Parsing(u16_t protocol, u16_t slaveAddress, struct ModbusAdu* adu); //Reference in modbus.c
void Modbus_InitAdu(struct ModbusAdu* adu);
#endif //__MODBUS_PARAM_H
