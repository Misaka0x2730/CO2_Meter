#include "stm32l1xx_conf.h"
#include "gpio.h"
#include "delay.h"
#include "Display.h"
#include "sensors.h"
#include "modbus.h"

extern struct ModbusRTU_State rs485State;

void Sys_Init()
{
  __IO uint32_t StartUpCounter = 0, HSIStatus = 0;

  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
  /* Enable HSE */
  RCC->CR |= ((uint32_t)RCC_CR_HSION);

  /* Wait till HSI is ready and if Time out is reached exit */
  do
  {
	HSIStatus = RCC->CR & RCC_CR_HSIRDY;
	StartUpCounter++;
  } while((HSIStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSIRDY) != RESET)
  {
	HSIStatus = (uint32_t)0x01;
  }
  else
  {
	HSIStatus = (uint32_t)0x00;
  }

  if (HSIStatus == (uint32_t)0x01)
  {
	/* Enable 64-bit access */
	FLASH->ACR |= FLASH_ACR_ACC64;

	/* Enable Prefetch Buffer */
	FLASH->ACR |= FLASH_ACR_PRFTEN;

	/* Flash 1 wait state */
	FLASH->ACR |= FLASH_ACR_LATENCY;

	/* Power enable */
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

	/* Select the Voltage Range 1 (1.8 V) */
	PWR->CR = PWR_CR_VOS_0;

	/* Wait Until the Voltage Regulator is ready */
	while((PWR->CSR & PWR_CSR_VOSF) != RESET)
	{
	}

	/* HCLK = SYSCLK /1*/
	RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

	/* PCLK2 = HCLK /1*/
	RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

	/* PCLK1 = HCLK /1*/
	RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV1;

	/*  PLL configuration */
	RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMUL |
										RCC_CFGR_PLLDIV));
	RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSI | RCC_CFGR_PLLMUL6 | RCC_CFGR_PLLDIV3);

	/* Enable PLL */
	RCC->CR |= RCC_CR_PLLON;

	/* Wait till PLL is ready */
	while((RCC->CR & RCC_CR_PLLRDY) == 0)
	{
	}

	/* Select PLL as system clock source */
	RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
	RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

	/* Wait till PLL is used as system clock source */
	while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL)
	{
	}
  }
  else
  {
	/* If HSE fails to start-up, the application will have wrong clock
	   configuration. User can add here some code to deal with this error */
  }
}

void main()
{
	Sys_Init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	Display_Init();
	GPIO_xInit(PIN_BUTTON1, GPIO_Speed_400KHz, GPIO_Mode_IN, GPIO_OType_PP, GPIO_PuPd_UP);
	GPIO_xInit(PIN_BUTTON2, GPIO_Speed_400KHz, GPIO_Mode_IN, GPIO_OType_PP, GPIO_PuPd_UP);
	Sensors_Init();
	Sensors_MeasureAll();
	Display_DisplaySensorsData();
	Modbus_Init();
	__enable_irq();

	while(1)
	{
		if(GPIO_xReadBit(PIN_BUTTON1) == Bit_RESET)
		{
			Display_IncBackLight();
			Display_DisplaySensorsData();
		}
		if(GPIO_xReadBit(PIN_BUTTON2) == Bit_RESET)
		{
			Display_DecBackLight();
			Display_DisplaySensorsData();
		}
		delay_ms(100);
		if(Sensors_MeasureAllIfReady())
			Display_DisplaySensorsData();
		if(Sensors_GetUpdateFlag())
		{
			Sensors_ClearUpdateFlag();
			Display_DisplaySensorsData();
		}
	}
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
		GPIO_xResetBit(PIN_LCD_BL);
	}
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		GPIO_xSetBit(PIN_LCD_BL);
	}
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		Sensors_SetUpdateFlag();
	}
}

void HardFault_Handler(void)
{
  /* Go into an infinite loop. */
  while (1);
}

/* Modbus RTU Interrupt  */

void USART1_IRQHandler()
{
	u8_t data = 0;
	u8_t status = 0;

	if (USART_GetITStatus(MODBUS_RTU_UART, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(MODBUS_RTU_UART, USART_IT_RXNE);
		if((MODBUS_RTU_UART->SR & (USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE | USART_FLAG_ORE)) == 0)
		{
			data = USART_ReceiveData(MODBUS_RTU_UART);
			if(rs485State.state == MODBUS_RTU_RECEIVE)
			{
				rs485State.rxTimer = 0;
				if(rs485State.rxData.length < sizeof(rs485State.rxData.bytes))
					rs485State.rxData.bytes[rs485State.rxData.length++] = data;
			}
		}
		else
		{
			status = MODBUS_RTU_UART->SR;
			data =  USART_ReceiveData(MODBUS_RTU_UART);
		}
	}

	if (USART_GetITStatus(MODBUS_RTU_UART, USART_IT_TC) != RESET)
	{
		USART_ClearITPendingBit(MODBUS_RTU_UART, USART_IT_TC);
		if(rs485State.state == MODBUS_RTU_TRANCEIVE)
		{
			rs485State.state = MODBUS_RTU_RECEIVE;
			rs485State.txTimer = 0;

			DMA_ClearITPendingBit(MODBUS_RTU_DMA_IT_FLAG);				//End of DMA transfer
			DMA_Cmd(MODBUS_RTU_DMA_CHANNEL, DISABLE);

			USART_DMACmd(MODBUS_RTU_UART, USART_DMAReq_Tx, DISABLE);
			USART_ITConfig(MODBUS_RTU_UART, USART_IT_TC, DISABLE);
			MODBUS_RTU_SELECT_RECEIVER;
		}
	}

	if (USART_GetITStatus(MODBUS_RTU_UART, USART_IT_ORE) != RESET)
	{
		USART_ClearITPendingBit(MODBUS_RTU_UART, USART_IT_ORE);
		status = MODBUS_RTU_UART->SR;
		data =  USART_ReceiveData(MODBUS_RTU_UART);
	}
}

void TIM6_IRQHandler(void)
{
	if (TIM_GetFlagStatus(MODBUS_RTU_TIM, TIM_FLAG_Update))
	{
		TIM_ClearITPendingBit(MODBUS_RTU_TIM, TIM_IT_Update);
		if((rs485State.state == MODBUS_RTU_RECEIVE) && (rs485State.rxData.length > 0) && (++rs485State.rxTimer > rs485State.rxDelay)) //Receive timeout
		{
			rs485State.rxTimer = 0;
			if((rs485State.rxData.bytes[0] == rs485State.slaveAddress) ||  (rs485State.rxData.bytes[0] == MODBUS_RTU_BROADCAST_ADDRESS))
			{
				//rs485State.state = MODBUS_RTU_PROCESSING;
				USART_ITConfig(MODBUS_RTU_UART, USART_IT_RXNE, DISABLE);
				Modbus_Parsing(MODBUS_RTU, rs485State.slaveAddress, &rs485State.rxData);
				if(rs485State.state != MODBUS_RTU_TRANCEIVE)
				{
					rs485State.state = MODBUS_RTU_RECEIVE;
					rs485State.txData.length = 0;
					rs485State.rxData.length = 0;
					MODBUS_RTU_SELECT_RECEIVER;
				}
			}
			else
				rs485State.rxData.length = 0;
		}
		if((rs485State.state == MODBUS_RTU_TRANCEIVE) && (++rs485State.txTimer > rs485State.txDelay)) //Send timeout
		{
			USART_ClearITPendingBit(MODBUS_RTU_UART, USART_IT_TC);
			USART_ITConfig(MODBUS_RTU_UART, USART_IT_TC, DISABLE);
			USART_DMACmd(MODBUS_RTU_UART, USART_DMAReq_Tx, DISABLE);

			DMA_ClearITPendingBit(MODBUS_RTU_DMA_IT_FLAG);
			DMA_Cmd(MODBUS_RTU_DMA_CHANNEL, DISABLE);

			rs485State.state = MODBUS_RTU_RECEIVE;
			rs485State.txTimer = 0;

			MODBUS_RTU_SELECT_RECEIVER;
		}
	}
	else
		MODBUS_RTU_TIM->SR = 0x0000;
}

void assert_failed(uint8_t* file, uint32_t line)
{
	while(1);
}
