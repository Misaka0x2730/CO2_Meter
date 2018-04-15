#include "delay.h"

void delay_us(u16_t delay)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
    TIM7->PSC = (F_APB1/1000000)-1;
    TIM7->ARR = delay - 1;
    TIM7->EGR = TIM_EGR_UG;
    TIM7->CR1 = TIM_CR1_CEN|TIM_CR1_OPM;
    TIM7->CNT = 0;
    while (TIM7->CR1 & TIM_CR1_CEN);
}

void delay_ms(u16_t delay)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
    TIM7->PSC = (F_APB1/1000)-1;
    TIM7->ARR = delay - 1;
    TIM7->EGR = TIM_EGR_UG;
    TIM7->CR1 = TIM_CR1_CEN|TIM_CR1_OPM;
    TIM7->CNT = 0;
    while (TIM7->CR1 & TIM_CR1_CEN);
}
