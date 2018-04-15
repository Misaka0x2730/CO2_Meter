#ifndef __I2C_H
#define __I2C_H

#include "types.h"
#include "stm32l152.h"
#include "delay.h"

void I2C_Start(I2C_TypeDef* I2Cx, u8_t direction, u8_t address);
void I2C_WriteData(I2C_TypeDef* I2Cx, u8_t data);
u8_t I2C_ReadData(I2C_TypeDef* I2Cx);
void I2C_Stop(I2C_TypeDef* I2Cx);
void I2C_CheckEventTimeout(I2C_TypeDef* I2Cx, u32_t flag);
void I2C_CheckFlagTimeout(I2C_TypeDef* I2Cx, u32_t flag);

#endif
