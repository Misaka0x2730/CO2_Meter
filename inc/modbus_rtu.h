#ifndef __MODBUS_RTU_H
#define __MODBUS_RTU_H

#include "stm32l152.h"
#include "stm32l1xx_conf.h"
#include "modbus_struct.h"
#include "gpio.h"
#include "pinMap.h"
#include <stddef.h>

/*
* Format:
* High: port number
* 0 - A
* 1 - B
* 2 - C
* 3 - D
* 4 - E
* Low: pin number
* [0 - 15] - [0-F]
*/

#define MODBUS_RTU_OK							((u8_t)0)
#define MODBUS_RTU_ERROR						((u8_t)1)

#define MODBUS_RTU_SELECT_RECEIVER 				do {USART_ITConfig(MODBUS_RTU_UART, USART_IT_RXNE, ENABLE);\
													USART_ITConfig(MODBUS_RTU_UART, USART_IT_TC, DISABLE);\
													GPIO_xResetBit(PIN_RS485_MODE);} while(0)

#define MODBUS_RTU_SELECT_TRANSMITTER			do {USART_ClearITPendingBit(MODBUS_RTU_UART, USART_IT_TC); \
													USART_ITConfig(MODBUS_RTU_UART, USART_IT_TC, ENABLE); \
													GPIO_xSetBit(PIN_RS485_MODE);} while(0)

#define MODBUS_RTU_UART							USART1
#define MODBUS_RTU_UART_RCC						RCC_APB2Periph_USART1
#define MODBUS_RTU_UART_RCC_ENABLE				do{RCC_APB2PeriphClockCmd(MODBUS_RTU_UART_RCC,ENABLE);}while(0)
#define MODBUS_RTU_UART_IRQn					USART1_IRQn

#define MODBUS_RTU_TIM							TIM6
#define MODBUS_RTU_TIM_RCC						RCC_APB1Periph_TIM6
#define MODBUS_RTU_TIM_RCC_ENABLE 				do{RCC_APB1PeriphClockCmd(MODBUS_RTU_TIM_RCC, ENABLE);}while(0)
#define MODBUS_RTU_TIM_PERIOD					(10-1) //us
#define MODBUS_RTU_TIM_IRQn						TIM6_IRQn

#define MODBUS_RTU_DMA							DMA1
#define MODBUS_RTU_DMA_RCC						RCC_AHBPeriph_DMA1
#define MODBUS_RTU_DMA_RCC_ENABLE				do{RCC_AHBPeriphClockCmd(MODBUS_RTU_DMA_RCC, ENABLE);}while(0)
#define MODBUS_RTU_DMA_CHANNEL					DMA1_Channel4
#define MODBUS_RTU_DMA_IT_FLAG					DMA1_IT_TC4


#define MODBUS_RTU_TIMEOUT_9600_10BIT			((u16_t)106)

#define MODBUS_RTU_SEND_TIMEOUT					((u32_t)300000)

#define MODBUS_RTU_DEFAULT_ADDRESS 				((u8_t)0x48)
#define MODBUS_RTU_BROADCAST_ADDRESS 			((u8_t)0x00)
#define MODBUS_RTU_MIN_ADDRESS 					((u8_t)0x01)
#define MODBUS_RTU_MAX_ADDRESS					((u8_t)0xF7)//247

#define MODBUS_RTU_ON 							((u8_t)1)
#define MODBUS_RTU_MAX_DATA_SIZE 				((u16_t)(MODBUS_RTU_ADU_MAX_SIZE - 3)) //MODBUS_RTU_ADU_MAX_SIZE - address(1 byte) - CRC(2 bytes)

#define MODBUS_RTU_RECEIVE 						((u8_t)1)
#define MODBUS_RTU_TRANCEIVE 					((u8_t)2)
#define MODBUS_RTU_PROCESSING 					((u8_t)3)

void ModbusRTU_State_Init();
void ModbusRTU_Init_GPIO();
void ModbusRTU_Init_USART();
void ModbusRTU_Init_Timer();
void ModbusRTU_Init_DMA();
void ModbusRTU_Init_NVIC();

void ModbusRTU_Start();
void ModbusRTU_Stop();
void ModbusRTU_Init();
u8_t ModbusRTU_ReInit();
u8_t ModbusRTU_CheckCorrectData();
u8_t ModbusRTU_CheckCRC16(struct ModbusAdu* data);
u8_t ModbusRTU_SendResponse(struct ModbusPdu* pdu, u8_t errorCode);
void ModbusRTU_GetDeviceInfo(struct ModbusPdu* pdu);

struct ModbusRTU_State
{
	u8_t state;
	u32_t rxDelay;
	u32_t rxTimer;
	u32_t txDelay;
	u32_t txTimer;
	u16_t slaveAddress;
	struct ModbusAdu rxData;
	struct ModbusAdu txData;
};

#endif //__MODBUS_RTU_H
