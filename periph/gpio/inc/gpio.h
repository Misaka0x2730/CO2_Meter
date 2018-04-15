#ifndef PERIPH_GPIO_INC_GPIO_H_
#define PERIPH_GPIO_INC_GPIO_H_

#include "types.h"
#include "stm32l1xx_conf.h"

typedef u8_t GPIO_PIN_TYPE;

void GPIO_xInit(GPIO_PIN_TYPE pin, GPIOSpeed_TypeDef GPIO_Speed, GPIOMode_TypeDef GPIO_Mode, GPIOOType_TypeDef GPIO_OType, GPIOPuPd_TypeDef GPIO_PuPd);
void GPIO_xSetBit(GPIO_PIN_TYPE pin);
void GPIO_xResetBit(GPIO_PIN_TYPE pin);
void GPIO_xToggleBit(GPIO_PIN_TYPE pin);
u8_t GPIO_xReadBit(GPIO_PIN_TYPE pin);
void GPIO_xPinAFConfig(GPIO_PIN_TYPE pin, uint8_t GPIO_AF);

#endif /* PERIPH_GPIO_INC_GPIO_H_ */
