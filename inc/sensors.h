#ifndef INC_SENSORS_H_
#define INC_SENSORS_H_

#include "stm32l1xx_conf.h"
#include "gpio.h"
#include "delay.h"
#include "i2c.h"

#define HDC1080_ADDR	0x80
#define CCS811_ADDR		0xB4

#define HDC1080_TEMPERATURE_ADDR		0x00
#define HDC1080_HUMIDITY_ADDR			0x01
#define HDC1080_CONFIGURATION_ADDR		0x02
#define HDC1080_SERIAL_ID_ADDR			0xFB
#define HDC1080_MANUFACTURER_ID_ADDR	0xFE
#define HDC1080_DEVICE_ID_ADDR			0xFF

#define CCS811_STATUS_ADDR				0x00
#define CCS811_MEAS_MODE_ADDR			0x01
#define CCS811_ALG_RESULT_DATA_ADDR		0x02
#define CCS811_RAW_DATA_ADDR			0x03
#define CCS811_ENV_DATA_ADDR			0x05
#define CCS811_THRESHOLDS_ADDR			0x10
#define CCS811_BASELINE_ADDR			0x11
#define CCS811_HW_ID_ADDR				0x20
#define CCS811_HW_VERSION_ADDR			0x21
#define CCS811_ERROR_ID_ADDR			0xE0
#define CCS811_APP_START_ADDR			0xF4
#define CCS811_SW_RESET_ADDR			0xFF

void Sensors_Init();

void HDC1080_SWReset();
void HDC1080_Init();
u16_t HDC1080_GetHumidity();
s16_t HDC1080_GetTemperature();

void CCS811_Init();
u16_t CCS811_GetECO2();

void Sensors_MeasureAll();
u8_t Sensors_MeasureAllIfReady();

void Sensors_SetUpdateFlag();
u8_t Sensors_GetUpdateFlag();
void Sensors_ClearUpdateFlag();

#endif /* INC_SENSORS_H_ */
