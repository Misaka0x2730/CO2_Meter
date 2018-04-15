#include "gpio.h"

GPIO_TypeDef* allGPIOPorts[5] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE};

void GPIO_xInit(GPIO_PIN_TYPE pin, GPIOSpeed_TypeDef GPIO_Speed, GPIOMode_TypeDef GPIO_Mode, GPIOOType_TypeDef GPIO_OType, GPIOPuPd_TypeDef GPIO_PuPd)
{
	RCC_AHBPeriphClockCmd((RCC_AHBPeriph_GPIOA << ((pin>>4) & 0x0F)), ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_0 << (pin & 0x0F));
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode;
	GPIO_InitStructure.GPIO_OType = GPIO_OType;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd;
	GPIO_Init(allGPIOPorts[(pin>>4) & 0x0F], &GPIO_InitStructure);
}

void GPIO_xSetBit(GPIO_PIN_TYPE pin)
{
	allGPIOPorts[(pin>>4) & 0x0F]->BSRRL = (GPIO_Pin_0 << (pin & 0x0F));
}

void GPIO_xResetBit(GPIO_PIN_TYPE pin)
{
	allGPIOPorts[(pin>>4) & 0x0F]->BSRRH = (GPIO_Pin_0 << (pin & 0x0F));
}

void GPIO_xToggleBit(GPIO_PIN_TYPE pin)
{
	if(allGPIOPorts[(pin>>4) & 0x0F]->IDR & (GPIO_Pin_0 << (pin & 0x0F)))
		allGPIOPorts[(pin>>4) & 0x0F]->BSRRH = (GPIO_Pin_0 << (pin & 0x0F));
	else
		allGPIOPorts[(pin>>4) & 0x0F]->BSRRL = (GPIO_Pin_0 << (pin & 0x0F));
}

u8_t GPIO_xReadBit(GPIO_PIN_TYPE pin)
{
	if(allGPIOPorts[(pin>>4) & 0x0F]->IDR & (GPIO_Pin_0 << (pin & 0x0F)))
		return Bit_SET;
	else
		return Bit_RESET;
}

void GPIO_xPinAFConfig(GPIO_PIN_TYPE pin, uint8_t GPIO_AF)
{
	GPIO_PinAFConfig(allGPIOPorts[(pin>>4) & 0x0F], (pin & 0x0F), GPIO_AF);
}
