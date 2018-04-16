#include "modbus.h"

static u8_t Modbus_SendResponse(struct ModbusPdu* pdu, u8_t errorCode);
static void Modbus_InitPdu(struct ModbusPdu* pdu);
static void Modbus_ParsingPdu(u8_t protocol, struct ModbusPdu* pdu, struct ModbusAdu* adu);

void Modbus_Init()
{
	ModbusRTU_Init();
}

static void Modbus_InitPdu(struct ModbusPdu* pdu)
{
	u16_t i = 0;
	pdu->cmd = 0;
	pdu->slaveAddress = 0;
	pdu->numberOfBytes = 0;
	pdu->numberOfOutputs = 1;
	pdu->length = 0;
	for(i = 0; i < MODBUS_MAX_DATA_SIZE; i++)
		pdu->data[i] = 0;
}

void Modbus_InitAdu(struct ModbusAdu* adu)
{
	u16_t i = 0;
	for(i = 0; i < sizeof(adu->bytes); i++)
		adu->bytes[i] = 0;
	adu->length = 0;
}

static void Modbus_ParsingPdu(u8_t protocol, struct ModbusPdu* pdu, struct ModbusAdu* adu)
{
	u16_t i = 0, j = 0;
	u16_t originState = (protocol == MODBUS_TCP) ? (MODBUS_MBAP_LENGTH) : (1);
	u16_t endState = (protocol == MODBUS_TCP) ? 0 : 2; // Modbus RTU: last 2 bytes - CRC-16
	u16_t maxLength = (protocol == MODBUS_TCP) ? (MODBUS_TCP_MAX_ADU_SIZE) : (MODBUS_RTU_ADU_MAX_SIZE);
	if(protocol == MODBUS_RTU)
		pdu->slaveAddress = adu->bytes[0];

	for(i = 0; ((i < adu->length - originState - endState) && ((i+originState) < maxLength)); i++)
	{
		switch(i)
		{
			case 0:
			{
				pdu->cmd = adu->bytes[i+originState];
				break;
			}
			case 1:
			{
				pdu->address = (adu->bytes[i+originState]<<8);
				break;
			}
			case 2:
			{
				pdu->address += adu->bytes[i+originState];
				break;
			}
			case 3:
			{
				if(pdu->cmd == MODBUS_READ_INPUT_REGISTERS)
					pdu->numberOfOutputs = (adu->bytes[i+originState]<<8);
				break;
			}
			case 4:
			{
				if(pdu->cmd == MODBUS_READ_INPUT_REGISTERS)
					pdu->numberOfOutputs += adu->bytes[i+originState];
				break;
			}
			case 5:
			{
				pdu->data[2] = adu->bytes[i+originState];
				pdu->length = 3;
				break;
			}
			default:
			{
				for(j = i; ((j < adu->length - originState - endState) && ((j+originState) < maxLength) && (pdu->length < MODBUS_MAX_DATA_SIZE)); j++)
					pdu->data[pdu->length++] = adu->bytes[j+originState];
				return;
			}
		}
	}
}

static u8_t Modbus_SendResponse(struct ModbusPdu* pdu, u8_t errorCode)
{
	return ModbusRTU_SendResponse(pdu, errorCode);
}

u8_t Modbus_Parsing(u16_t protocol, u16_t slaveAddress, struct ModbusAdu* adu)
{
	u16_t i = 0;
	struct ModbusPdu pdu;
	u8_t errorCode = MODBUS_ERROR_OK;

	if(protocol == MODBUS_RTU)
	{
		if(adu->length < MODBUS_RTU_MIN_DATA_SIZE)
			return MODBUS_UNDEFINED_ERROR;
	}
	else
		return MODBUS_UNDEFINED_ERROR;

	Modbus_InitPdu(&pdu);
	Modbus_ParsingPdu(protocol, &pdu, adu);

	if(ModbusRTU_CheckCRC16(adu) == MODBUS_ERROR_CRC)
		return MODBUS_ERROR_OK;
	else
	{
		if((pdu.slaveAddress != slaveAddress) && (pdu.slaveAddress != MODBUS_RTU_BROADCAST_ADDRESS))
			return MODBUS_ERROR_OK;
	}

	switch(pdu.cmd)
	{
		case MODBUS_READ_INPUT_REGISTERS:
		{
			if((pdu.numberOfOutputs > 0) && ((pdu.address + pdu.numberOfOutputs) <= INPUT_AMOUNT))
			{
				for(u8_t i = 0; (i < pdu.numberOfOutputs) && (errorCode == MODBUS_ERROR_OK); i++)
				{
					pdu.numberOfBytes += 2;
					switch(pdu.address+i)
					{
						case INPUT_TEMPERATURE:
						{
							s16_t temperature = HDC1080_GetTemperature();
							pdu.data[i*2] = (temperature>>8)&0xFF;
							pdu.data[i*2+1] = temperature&0xFF;
							errorCode = MODBUS_ERROR_OK;
							break;
						}
						case INPUT_RH:
						{
							u16_t rh = HDC1080_GetHumidity();
							pdu.data[i*2] = (rh>>8)&0xFF;
							pdu.data[i*2+1] = rh&0xFF;
							errorCode = MODBUS_ERROR_OK;
							break;
						}
						case INPUT_ECO2:
						{
							u16_t eco2 = CCS811_GetECO2();
							pdu.data[i*2] = (eco2>>8)&0xFF;
							pdu.data[i*2+1] = eco2&0xFF;
							errorCode = MODBUS_ERROR_OK;
							break;
						}
						default:
						{
							errorCode = MODBUS_ILLEGAL_DATA_ADDRESS;
							break;
						}
					}
				}
			}
			else
				errorCode = MODBUS_ILLEGAL_DATA_ADDRESS;
			break;
		}
		default:
		{
			errorCode = MODBUS_ILLEGAL_FUNCTION;
			break;
		}
	}
	u8_t responseStatus = Modbus_SendResponse(&pdu,errorCode);
	return responseStatus;
}
