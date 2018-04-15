#include "i2c.h"

void I2C_CheckEventTimeout(I2C_TypeDef* I2Cx, u32_t flag)
{
	while(!I2C_CheckEvent(I2Cx, flag));
}

void I2C_Start(I2C_TypeDef* I2Cx, u8_t direction, u8_t address)
{
	I2C_GenerateSTART(I2Cx, ENABLE);
	I2C_CheckEventTimeout(I2Cx, I2C_EVENT_MASTER_MODE_SELECT);

	I2C_Send7bitAddress(I2Cx, address, direction);
	if(direction == I2C_Direction_Transmitter)
		I2C_CheckEventTimeout(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);

	if(direction == I2C_Direction_Receiver)
		I2C_CheckEventTimeout(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
}

void I2C_WriteData(I2C_TypeDef* I2Cx,u8_t data)
{
    I2C_SendData(I2Cx, data);
    I2C_CheckEventTimeout(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
}

u8_t I2C_ReadData(I2C_TypeDef* I2Cx)
{
	u8_t data = 0;
	I2C_CheckEventTimeout(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED);
    data = I2C_ReceiveData(I2Cx);
    return data;
}

void I2C_Stop(I2C_TypeDef* I2Cx)
{
	I2C_GenerateSTOP(I2Cx, ENABLE);
	delay_ms(20);
}
