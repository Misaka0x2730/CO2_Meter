#include "Display.h"

static u8_t backlightValue = 10;

void Display_Init()
{
	GPIO_xInit(PIN_LCD_B4, GPIO_Speed_400KHz, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL);
	GPIO_xInit(PIN_LCD_B5, GPIO_Speed_400KHz, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL);
	GPIO_xInit(PIN_LCD_B6, GPIO_Speed_400KHz, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL);
	GPIO_xInit(PIN_LCD_B7, GPIO_Speed_400KHz, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL);
	GPIO_xInit(PIN_LCD_E, GPIO_Speed_400KHz, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL);
	GPIO_xInit(PIN_LCD_RS, GPIO_Speed_400KHz, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL);
	GPIO_xInit(PIN_LCD_BL, GPIO_Speed_400KHz, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL);

	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	TIM_TimeBaseStructInit(&TIM_InitStructure);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStructure.TIM_Prescaler = 80 - 1;
	TIM_InitStructure.TIM_Period = 400 - 1;
	TIM_TimeBaseInit(TIM2, &TIM_InitStructure);

	TIM_CCxCmd(TIM2, TIM_Channel_1, TIM_CCx_Enable);
	TIM_SetCompare1(TIM2, 10);

	TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

	TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	TIM_Cmd(TIM2, ENABLE);
	NVIC_EnableIRQ(TIM2_IRQn);
	GPIO_xSetBit(PIN_LCD_BL);

	delay_ms(100);
	Display_WriteCmd(0x02);
	delay_ms(5);
	Display_WriteCmd(0x2C);
	Display_WriteCmd(0x0C);
	Display_WriteCmd(0x01);
	delay_ms(2);
	Display_WriteCmd(0x06);
	delay_ms(40);
}

void Display_WriteSymbol(unsigned char data)
{
	GPIOB->BSRRH = 0xF000;
	GPIOB->BSRRL = ((data & 0xF0)>>4)<<12;

	GPIO_xSetBit(PIN_LCD_E);
	delay_us(2);
	GPIO_xResetBit(PIN_LCD_E);

	GPIOB->BSRRH = 0xF000;
	GPIOB->BSRRL = (data & 0x0F)<<12;

	GPIO_xSetBit(PIN_LCD_E);
	delay_us(2);
	GPIO_xResetBit(PIN_LCD_E);
	delay_us(40);
}

void Display_WriteString(unsigned char *str)
{
	while(*str)
		Display_WriteSymbol(*str++);
}

void Display_WriteCmd(unsigned char com)
{
	GPIO_xResetBit(PIN_LCD_RS);
	Display_WriteSymbol(com);
	GPIO_xSetBit(PIN_LCD_RS);
}

void Display_Clear()
{
	Display_WriteCmd(0x01);
	delay_ms(2);
}

void Display_SetStr(unsigned char str)
{
	switch(str)
	{
		case 1: {Display_WriteCmd(0x80); break;}
		case 2: {Display_WriteCmd(0xC0); break;}
		default: break;
	};
}

void Display_IncBackLight()
{
	if(backlightValue < 99)
	{
		TIM_SetCompare1(TIM2, ++backlightValue);
	}
}

void Display_DecBackLight()
{
	if(backlightValue > 10)
	{
		TIM_SetCompare1(TIM2, --backlightValue);
	}
}

u8_t Display_GetBackLightValue()
{
	return backlightValue;
}

void Display_DisplaySensorsData()
{
	s16_t temperature = HDC1080_GetTemperature();
	u16_t humidity = HDC1080_GetHumidity();
	u16_t eco2 = CCS811_GetECO2();
	u16_t backlight = Display_GetBackLightValue();
	u8_t spaceCount = 0;

	Display_Clear();
	Display_SetStr(1);

	Display_WriteString("T:");
	if(temperature < 0)
	{
		Display_WriteSymbol('-');
		temperature *= -1;
	}
	else
		Display_WriteSymbol('+');

	if(temperature > 100)
		Display_WriteSymbol(temperature/100 + 0x30);
	else
		++spaceCount;
	Display_WriteSymbol((temperature%100)/10 + 0x30);
	Display_WriteSymbol(0xDF);
	Display_WriteSymbol('C');

	Display_WriteString("   RH:");
	for(u8_t i = 0; i < spaceCount; ++i)
		Display_WriteSymbol(' ');
	Display_WriteSymbol(humidity/100 + 0x30);
	Display_WriteSymbol((humidity%100)/10 + 0x30);
	Display_WriteSymbol('%');

	spaceCount = 0;
	Display_SetStr(2);
	Display_WriteString("C:");
	if(eco2 >= 10000)
		Display_WriteSymbol(eco2/10000 + 0x30);
	else
		++spaceCount;
	if(eco2 >= 1000)
		Display_WriteSymbol((eco2%10000)/1000 + 0x30);
	else
		++spaceCount;
	if(eco2 >= 100)
		Display_WriteSymbol((eco2%1000)/100 + 0x30);
	else
		++spaceCount;
	if(eco2 >= 10)
		Display_WriteSymbol((eco2%100)/10 + 0x30);
	Display_WriteSymbol(eco2%10 + 0x30);
	Display_WriteString("ppm ");

	for(u8_t i = 0; i < spaceCount; ++i)
		Display_WriteSymbol(' ');

	Display_WriteString("L:");
	if(backlight >= 10)
		Display_WriteSymbol((backlight%100)/10 + 0x30);
	Display_WriteSymbol(backlight%10 + 0x30);
	Display_WriteSymbol('%');
}
