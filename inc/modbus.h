#ifndef __MODBUS_H
#define __MODBUS_H

#include "modbus_struct.h"
#include "modbus_rtu.h"
#include "sensors.h"

void Modbus_Init();
u8_t Modbus_Parsing(u16_t protocol, u16_t slaveAddress, struct ModbusAdu* adu); //Reference in modbus.c
void Modbus_InitAdu(struct ModbusAdu* adu);

#endif //__MODBUS_H
