#ifndef DISPLAY_INC_DISPLAY_H_
#define DISPLAY_INC_DISPLAY_H_

#include "stm32l1xx_conf.h"
#include "gpio.h"
#include "delay.h"
#include "sensors.h"

void Display_Init();
void Display_WriteSymbol(unsigned char data);
void Display_WriteString(unsigned char *str);
void Display_WriteCmd(unsigned char com);
void Display_Clear();
void Display_SetStr(unsigned char str);
void Display_IncBackLight();
void Display_DecBackLight();
u8_t Display_GetBackLightValue();
void Display_DisplaySensorsData();
#endif /* DISPLAY_INC_DISPLAY_H_ */
