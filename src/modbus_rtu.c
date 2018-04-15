#include "modbus_rtu.h"

struct ModbusRTU_State rs485State;

const u8_t modbusCRC16Hi[] =
{
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
};

const u8_t modbusCRC16Lo[] =
{
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
};

static u8_t ModbusRTU_Send(struct ModbusAdu* data);

void ModbusRTU_State_Init()
{
	rs485State.state 			= MODBUS_RTU_RECEIVE;
	rs485State.rxData.length 	= 0;
	rs485State.txData.length 	= 0;
	rs485State.rxDelay 			= 0;
	rs485State.rxTimer 			= 0;
	rs485State.txTimer 			= 0;
	rs485State.txDelay 			= MODBUS_RTU_SEND_TIMEOUT;
	rs485State.slaveAddress 	= MODBUS_RTU_DEFAULT_ADDRESS;
}

void ModbusRTU_Init_GPIO()
{
	GPIO_xInit(PIN_RS485_TX, GPIO_Speed_10MHz, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_NOPULL);
	GPIO_xInit(PIN_RS485_RX, GPIO_Speed_10MHz, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_NOPULL);
	GPIO_xInit(PIN_RS485_MODE, GPIO_Speed_10MHz, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL);

	GPIO_xPinAFConfig(PIN_RS485_TX, GPIO_AF_USART1);
	GPIO_xPinAFConfig(PIN_RS485_RX, GPIO_AF_USART1);

	MODBUS_RTU_SELECT_RECEIVER;
}

void ModbusRTU_Init_USART()
{
	rs485State.slaveAddress = MODBUS_RTU_DEFAULT_ADDRESS;

	USART_DeInit(MODBUS_RTU_UART);
	MODBUS_RTU_UART_RCC_ENABLE;

#ifdef MODBUS_RTU_GPIO_REMAP
	GPIO_PinRemapConfig(MODBUS_RTU_GPIO_REMAP, ENABLE);
#endif

	USART_InitTypeDef USART_InitStructure;
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_Init(MODBUS_RTU_UART, &USART_InitStructure);
	rs485State.rxDelay = MODBUS_RTU_TIMEOUT_9600_10BIT*4;
}

void ModbusRTU_Init_Timer()
{
	TIM_TimeBaseInitTypeDef TIM_InitStructure;

	TIM_DeInit(MODBUS_RTU_TIM);
	MODBUS_RTU_TIM_RCC_ENABLE;
	TIM_TimeBaseStructInit(&TIM_InitStructure);
	TIM_InitStructure.TIM_Prescaler = ((u16_t)(F_APB1/1000000))*2-1;
	TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStructure.TIM_Period = MODBUS_RTU_TIM_PERIOD;
	TIM_TimeBaseInit(MODBUS_RTU_TIM, &TIM_InitStructure);
	TIM_ClearITPendingBit(MODBUS_RTU_TIM, TIM_IT_Update);
	TIM_ITConfig(MODBUS_RTU_TIM,TIM_IT_Update,ENABLE);
}

void ModbusRTU_Init_DMA()
{
	MODBUS_RTU_DMA_RCC_ENABLE;
	DMA_DeInit(MODBUS_RTU_DMA_CHANNEL);

	DMA_InitTypeDef DMA_InitStructure;

	DMA_StructInit(&DMA_InitStructure);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32_t)&(MODBUS_RTU_UART->DR);
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_BufferSize = 1;
	DMA_Init(MODBUS_RTU_DMA_CHANNEL, &DMA_InitStructure);
}

void ModbusRTU_Init_NVIC()
{
	NVIC_InitTypeDef  NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = MODBUS_RTU_UART_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = MODBUS_RTU_TIM_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/*NVIC_InitStructure.NVIC_IRQChannel = MODBUS_RTU_LED_BAUD_TIM_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = MODBUS_RTU_MODBUS_PARSE_TIM_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);*/
}

void ModbusRTU_Init()
{
	ModbusRTU_State_Init();
	ModbusRTU_Init_GPIO();
	ModbusRTU_Init_USART();
	ModbusRTU_Init_Timer();
	ModbusRTU_Init_DMA();
	ModbusRTU_Init_NVIC();

	ModbusRTU_Start();
}

void ModbusRTU_Start()
{
	rs485State.rxData.length = 0;
	rs485State.txData.length = 0;
	rs485State.rxTimer = 0;
	rs485State.txTimer = 0;
	rs485State.state = MODBUS_RTU_RECEIVE;
	MODBUS_RTU_SELECT_RECEIVER;

	USART_ClearITPendingBit(MODBUS_RTU_UART, USART_IT_RXNE);
	USART_ClearITPendingBit(MODBUS_RTU_UART, USART_IT_TC);
	USART_Cmd(MODBUS_RTU_UART,ENABLE);

	TIM_ClearITPendingBit(MODBUS_RTU_TIM, TIM_IT_Update);
	//TIM_ClearITPendingBit(MODBUS_RTU_LED_BAUD_TIM, TIM_IT_Update);
	//TIM_ITConfig(MODBUS_RTU_MODBUS_PARSE_TIM,TIM_IT_Update, DISABLE);
	TIM_Cmd(MODBUS_RTU_TIM, ENABLE);
	//TIM_Cmd(MODBUS_RTU_LED_BAUD_TIM,ENABLE);
	//TIM_Cmd(MODBUS_RTU_MODBUS_PARSE_TIM, ENABLE);

	NVIC_EnableIRQ(MODBUS_RTU_UART_IRQn);
	NVIC_EnableIRQ(MODBUS_RTU_TIM_IRQn);
	//NVIC_EnableIRQ(MODBUS_RTU_LED_BAUD_TIM_IRQn);
	//NVIC_EnableIRQ(MODBUS_RTU_MODBUS_PARSE_TIM_IRQn);
}

void ModbusRTU_Stop()
{
	rs485State.state = MODBUS_RTU_RECEIVE;
	MODBUS_RTU_SELECT_RECEIVER;
	USART_Cmd(MODBUS_RTU_UART, DISABLE);
	DMA_Cmd(MODBUS_RTU_DMA_CHANNEL, DISABLE);
	TIM_Cmd(MODBUS_RTU_TIM, DISABLE);
	//TIM_Cmd(MODBUS_RTU_MODBUS_PARSE_TIM, DISABLE);

	NVIC_DisableIRQ(MODBUS_RTU_UART_IRQn);
	NVIC_DisableIRQ(MODBUS_RTU_TIM_IRQn);
	//NVIC_DisableIRQ(MODBUS_RTU_MODBUS_PARSE_TIM_IRQn);
}

u8_t ModbusRTU_Send(struct ModbusAdu* data)
{
	u16_t i = 0;

	if(data != NULL)
	{
		rs485State.state = MODBUS_RTU_TRANCEIVE;
		rs485State.txData.length = 0;
		rs485State.rxData.length = 0;

		for(i = 0; ((i < data->length) && (i < sizeof(rs485State.txData.bytes))); i++)
			rs485State.txData.bytes[rs485State.txData.length++] = data->bytes[i];

		DMA_Cmd(MODBUS_RTU_DMA_CHANNEL, DISABLE);
		DMA_SetCurrDataCounter(MODBUS_RTU_DMA_CHANNEL,rs485State.txData.length);
		MODBUS_RTU_DMA_CHANNEL->CMAR = (u32_t)(&rs485State.txData.bytes);
		MODBUS_RTU_SELECT_TRANSMITTER;

		USART_DMACmd(MODBUS_RTU_UART, USART_DMAReq_Tx, ENABLE);
		DMA_Cmd(MODBUS_RTU_DMA_CHANNEL, ENABLE);
	}
	else
	{
		rs485State.state = MODBUS_RTU_RECEIVE;
		rs485State.txData.length = 0;
		rs485State.rxData.length = 0;
		MODBUS_RTU_SELECT_RECEIVER;
	}
	return MODBUS_ERROR_OK;
}

u8_t ModbusRTU_CheckCorrectData()
{
	return MODBUS_RTU_OK;
}

u8_t ModbusRTU_ReInit()
{
	u8_t result = MODBUS_RTU_OK;
	if(ModbusRTU_CheckCorrectData() != MODBUS_RTU_OK)
		return MODBUS_RTU_ERROR;

	ModbusRTU_State_Init();
	ModbusRTU_Stop();
	ModbusRTU_Init_USART();
	ModbusRTU_Init_Timer();
	ModbusRTU_Start();
	return result;
}

u16_t ModbusRTU_CalculateCRC16(struct ModbusAdu* data)
{
	u8_t CRC16Hi = 0xFF;
	u8_t CRC16Lo = 0xFF;
	u16_t index = 0;
	u16_t i = 0;
	for(i = 0; i < data->length - 2; i++)
	{
		index = CRC16Lo ^ data->bytes[i];
		CRC16Lo = CRC16Hi ^ modbusCRC16Hi[index];
		CRC16Hi = modbusCRC16Lo[index];
	}
	return ((u16_t)((CRC16Hi<<8) | CRC16Lo));
}

u8_t ModbusRTU_CheckCRC16(struct ModbusAdu* data)
{
	if(data->length > 2)
	{
		u16_t crc = ((data->bytes[data->length-1]<<8) + (data->bytes[data->length-2]));
		u16_t calculateCRC = ModbusRTU_CalculateCRC16(data);
		if(crc == calculateCRC)
			return MODBUS_ERROR_OK;
		else
			return MODBUS_ERROR_CRC;
	}
	else
		return MODBUS_ERROR_CRC;
}

u8_t ModbusRTU_SendResponse(struct ModbusPdu* pdu, u8_t errorCode)
{
	u16_t i = 0;
	u16_t crc16 = 0;
	struct ModbusAdu adu;
	Modbus_InitAdu(&adu);

	adu.bytes[0] = pdu->slaveAddress;
	if(pdu->slaveAddress == MODBUS_RTU_BROADCAST_ADDRESS)
		return ModbusRTU_Send(NULL);
	else
	{
		if((errorCode == MODBUS_ERROR_OK) && (MODBUS_IS_CMD(pdu->cmd)))
		{
			adu.bytes[1] = pdu->cmd;
			adu.length = 4;
			if((pdu->cmd == MODBUS_READ_HOLDING_REGISTERS)||(pdu->cmd == MODBUS_READ_COILS)||(pdu->cmd == MODBUS_READ_INPUT_REGISTERS)||(pdu->cmd == MODBUS_READ_DISCRETE_INPUTS))
			{
				adu.length = (1+1+1+pdu->numberOfBytes+2);//slave address+cmd+byte count+data[byte count]+crc-16
				adu.bytes[2] = pdu->numberOfBytes;
				for(i = 0; ((i < pdu->numberOfBytes) && (i < MODBUS_MAX_DATA_SIZE)); i++)
					adu.bytes[3+i] = pdu->data[i];
			}
			if((pdu->cmd == MODBUS_SET_ONE_COIL)||(pdu->cmd == MODBUS_WRITE_ONE_HOLDING_REGISTER))
			{
				adu.length = 1+1+2+2+2;//slave address+cmd+address+value+crc-16
				adu.bytes[2] = (pdu->address>>8)&0xFF;
				adu.bytes[3] = pdu->address & 0xFF;
				adu.bytes[4] = pdu->data[0];
				adu.bytes[5] = pdu->data[1];
			}
			if((pdu->cmd == MODBUS_SET_MULTIPLE_COILS)||(pdu->cmd == MODBUS_WRITE_MULTIPLE_HOLDING_REGISTERS))
			{
				adu.length = 1+1+2+2+2; //slave address+cmd+address+number of outputs+crc-16
				adu.bytes[2] = (pdu->address>>8)&0xFF;
				adu.bytes[3] = pdu->address & 0xFF;
				adu.bytes[4] = (pdu->numberOfOutputs>>8)&0xFF;
				adu.bytes[5] = pdu->numberOfOutputs & 0xFF;
			}
			if(pdu->cmd == MODBUS_READ_DEVICE_INFO)
			{
				adu.length = 1+1+1+pdu->data[0]+2;//slave address+cmd+byte count+data+crc-16
				for(i = 0; (i < (pdu->data[0] + 1)) && (i < (MODBUS_MAX_ADU_SIZE - 4)); i++)
					adu.bytes[2+i] = pdu->data[i];
			}
			if(pdu->cmd == MODBUS_LOOP_TEST)
			{
				adu.length = 1+1+1+2;//slave address+cmd+state+crc-16
				adu.bytes[2] = pdu->data[0];
			}
			if(pdu->cmd == MODBUS_READ_DIP_SWITCH_STATE)
			{
				adu.length = 1+1+2+2;//slave address+cmd+state+crc-16
				adu.bytes[2] = pdu->data[0];
				adu.bytes[3] = pdu->data[1];
			}
		}
		else
		{
			if(errorCode == MODBUS_ERROR_OK)
				errorCode = MODBUS_ILLEGAL_FUNCTION;

			adu.length = MODBUS_ERROR_ADU_LENGTH;
			adu.bytes[1] = pdu->cmd|MODBUS_ERROR_MASK;
			adu.bytes[2] = errorCode;
		}
		crc16 = ModbusRTU_CalculateCRC16(&adu);
		adu.bytes[adu.length-2] = (crc16 & 0xFF);
		adu.bytes[adu.length-1] = ((crc16>>8) & 0xFF);

		return ModbusRTU_Send(&adu);
	}
}

