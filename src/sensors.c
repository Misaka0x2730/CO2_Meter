#include "sensors.h"

static s16_t temperature = 0;
static u16_t humidity = 0;
static u16_t eco2 = 0;

static u8_t updateFlag = 0;

void Sensors_I2C_Init()
{
	I2C_SoftwareResetCmd(I2C1, ENABLE);
	I2C_SoftwareResetCmd(I2C1, DISABLE);

	GPIO_xInit(PIN_I2C_SCL, GPIO_Speed_40MHz, GPIO_Mode_AF, GPIO_OType_OD, GPIO_PuPd_UP);
	GPIO_xInit(PIN_I2C_SDA, GPIO_Speed_40MHz, GPIO_Mode_AF, GPIO_OType_OD, GPIO_PuPd_UP);

	I2C_InitTypeDef I2C_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	GPIO_xPinAFConfig(PIN_I2C_SCL, GPIO_AF_I2C1);
	GPIO_xPinAFConfig(PIN_I2C_SDA, GPIO_AF_I2C1);

	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 50000;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1, ENABLE);
}

void Sensors_Init()
{
	delay_ms(50);

	Sensors_I2C_Init();

	GPIO_xInit(PIN_SENSOR_RST, GPIO_Speed_400KHz, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL);
	GPIO_xInit(PIN_SENSOR_WAKEUP, GPIO_Speed_400KHz, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL);

	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	TIM_TimeBaseStructInit(&TIM_InitStructure);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStructure.TIM_Prescaler = 32000 - 1;			//32MHz/32000 = 1kHz
	TIM_InitStructure.TIM_Period = 2000 - 1;				//1kHz/2000 = 0.5Hz - 1 прерывание раз в 10 секунд
	TIM_TimeBaseInit(TIM3, &TIM_InitStructure);

	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	NVIC_EnableIRQ(TIM3_IRQn);

	HDC1080_SWReset();
	HDC1080_Init();

	CCS811_Init();
}

void HDC1080_SWReset()
{
	I2C_Start(I2C1, I2C_Direction_Transmitter, HDC1080_ADDR);
	I2C_WriteData(I2C1, HDC1080_CONFIGURATION_ADDR);
	I2C_WriteData(I2C1, 0x80);
	I2C_WriteData(I2C1, 0x00);
	I2C_Stop(I2C1);
}

void HDC1080_Init()
{
	I2C_Start(I2C1, I2C_Direction_Transmitter, HDC1080_ADDR);
	I2C_WriteData(I2C1, HDC1080_CONFIGURATION_ADDR);
	I2C_WriteData(I2C1, 0x00);
	I2C_WriteData(I2C1, 0x00);
	I2C_Stop(I2C1);
}
u16_t HDC1080_GetHumidity()
{
	return humidity;
}

void HDC1080_MeasureHumidity()
{
	u32_t data = 0;

	I2C_AcknowledgeConfig(I2C1, ENABLE);
	I2C_Start(I2C1, I2C_Direction_Transmitter, HDC1080_ADDR);
	I2C_WriteData(I2C1, HDC1080_HUMIDITY_ADDR);
	delay_ms(20);
	I2C_Start(I2C1, I2C_Direction_Receiver, HDC1080_ADDR);
	data = (I2C_ReadData(I2C1)<<8);
	I2C_AcknowledgeConfig(I2C1, DISABLE);
	data += I2C_ReadData(I2C1);
	I2C_Stop(I2C1);

	data *= 100;
	data *= 10;
	data >>= 16;
	humidity = (data & 0xFFFF);
}

s16_t HDC1080_GetTemperature()
{
	return temperature;
}

void HDC1080_MeasureTemperature()
{
	u32_t data = 0;

	I2C_AcknowledgeConfig(I2C1, ENABLE);
	I2C_Start(I2C1, I2C_Direction_Transmitter, HDC1080_ADDR);
	I2C_WriteData(I2C1, HDC1080_TEMPERATURE_ADDR);
	I2C_Stop(I2C1);
	delay_ms(20);
	I2C_Start(I2C1, I2C_Direction_Receiver, HDC1080_ADDR);
	data = (I2C_ReadData(I2C1)<<8);
	I2C_AcknowledgeConfig(I2C1, DISABLE);
	data += I2C_ReadData(I2C1);
	I2C_Stop(I2C1);

	data *= 165;
	data *= 10;
	data >>= 16;
	temperature = (data & 0xFFFF);
	temperature -= 400;
}

void CCS811_Init()
{
	u32_t data = 0;

	GPIO_xResetBit(PIN_SENSOR_RST);
	delay_ms(5);
	GPIO_xSetBit(PIN_SENSOR_RST);
	delay_ms(100);

	GPIO_xResetBit(PIN_SENSOR_WAKEUP);
	delay_ms(5);

	//I2C_AcknowledgeConfig(I2C1, DISABLE);

	I2C_AcknowledgeConfig(I2C1, ENABLE);
	I2C_Start(I2C1, I2C_Direction_Transmitter, CCS811_ADDR);
	I2C_WriteData(I2C1, CCS811_STATUS_ADDR);
	I2C_Start(I2C1, I2C_Direction_Receiver, CCS811_ADDR);
	I2C_AcknowledgeConfig(I2C1, DISABLE);
	data = I2C_ReadData(I2C1);
	I2C_Stop(I2C1);

	delay_ms(5);
	I2C_Start(I2C1, I2C_Direction_Transmitter, CCS811_ADDR);
	I2C_WriteData(I2C1, CCS811_APP_START_ADDR);
	I2C_Stop(I2C1);

	delay_ms(100);
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	I2C_Start(I2C1, I2C_Direction_Transmitter, CCS811_ADDR);
	I2C_WriteData(I2C1, CCS811_STATUS_ADDR);
	I2C_Start(I2C1, I2C_Direction_Receiver, CCS811_ADDR);
	I2C_AcknowledgeConfig(I2C1, DISABLE);
	data = I2C_ReadData(I2C1);
	I2C_Stop(I2C1);

	I2C_AcknowledgeConfig(I2C1, ENABLE);
	I2C_Start(I2C1, I2C_Direction_Transmitter, CCS811_ADDR);
	I2C_WriteData(I2C1, CCS811_MEAS_MODE_ADDR);
	I2C_WriteData(I2C1, 0x18);
	I2C_Stop(I2C1);

	//GPIO_xSetBit(PIN_SENSOR_WAKEUP);
	delay_ms(5);
}

u8_t CCS811_GetDataReady()
{
	u8_t data = 0;
	u8_t error = 0;
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	I2C_Start(I2C1, I2C_Direction_Transmitter, CCS811_ADDR);
	I2C_WriteData(I2C1, CCS811_STATUS_ADDR);
	I2C_Start(I2C1, I2C_Direction_Receiver, CCS811_ADDR);
	I2C_AcknowledgeConfig(I2C1, DISABLE);
	data = I2C_ReadData(I2C1);
	I2C_Stop(I2C1);

	I2C_AcknowledgeConfig(I2C1, ENABLE);
	I2C_Start(I2C1, I2C_Direction_Transmitter, CCS811_ADDR);
	I2C_WriteData(I2C1, CCS811_ERROR_ID_ADDR);
	I2C_Start(I2C1, I2C_Direction_Receiver, CCS811_ADDR);
	I2C_AcknowledgeConfig(I2C1, DISABLE);
	error = I2C_ReadData(I2C1);
	I2C_Stop(I2C1);

	return (data & 0x08);
}

u16_t CCS811_GetECO2()
{
	return eco2;
}

void CCS811_MeasureECO2()
{
	u16_t data = 0;
	u16_t tvoc = 0;
	u16_t temp = 0;
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	I2C_Start(I2C1, I2C_Direction_Transmitter, CCS811_ADDR);
	I2C_WriteData(I2C1, CCS811_ALG_RESULT_DATA_ADDR);
	I2C_Start(I2C1, I2C_Direction_Receiver, CCS811_ADDR);
	data = (I2C_ReadData(I2C1)<<8);
	//I2C_AcknowledgeConfig(I2C1, DISABLE);
	data += I2C_ReadData(I2C1);
	tvoc = (I2C_ReadData(I2C1)<<8);
	//I2C_AcknowledgeConfig(I2C1, DISABLE);
	tvoc += I2C_ReadData(I2C1);
	temp = (I2C_ReadData(I2C1)<<8);
	I2C_AcknowledgeConfig(I2C1, DISABLE);
	temp += I2C_ReadData(I2C1);
	I2C_Stop(I2C1);

	eco2 = data;
}

void Sensors_MeasureAll()
{
	HDC1080_MeasureTemperature();
	HDC1080_MeasureHumidity();
	CCS811_MeasureECO2();
}

u8_t Sensors_MeasureAllIfReady()
{
	if(CCS811_GetDataReady())
	{
		Sensors_MeasureAll();
		return 1;
	}
	return 0;
}

void Sensors_SetUpdateFlag()
{
	if(updateFlag == 0)
		updateFlag = 1;
}

u8_t Sensors_GetUpdateFlag()
{
	return updateFlag;
}

void Sensors_ClearUpdateFlag()
{
	updateFlag = 0;
}
