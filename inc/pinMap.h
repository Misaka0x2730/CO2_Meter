#ifndef INC_PINMAP_H_
#define INC_PINMAP_H_

/*
 * Format:
 * High: port number
 * 0 - A
 * 1 - B
 * 2 - C
 * 3 - D
 * 4 - E
 * Low: pin number
 * 0  - 0
 * 1  - 1
 * 2  - 2
 * 3  - 3
 * 4  - 4
 * 5  - 5
 * 6  - 6
 * 7  - 7
 * 8  - 8
 * 9  - 9
 * 10 - A
 * 11 - B
 * 12 - C
 * 13 - D
 * 14 - E
 * 15 - F
*/

#define PIN_BUTTON1			0x01
#define PIN_BUTTON2			0x02
#define PIN_BUTTON3			0x14

#define PIN_I2C_SCL			0x18
#define PIN_I2C_SDA			0x19
#define PIN_SENSOR_WAKEUP	0x17
#define PIN_SENSOR_INT		0x16
#define PIN_SENSOR_RST		0x15

#define PIN_LCD_RS			0x1A
#define PIN_LCD_E			0x1B
#define PIN_LCD_B4			0x1C
#define PIN_LCD_B5			0x1D
#define PIN_LCD_B6			0x1E
#define PIN_LCD_B7			0x1F
#define PIN_LCD_BL			0x08

#define PIN_RS485_TX		0x09
#define PIN_RS485_RX		0x0A
#define PIN_RS485_MODE		0x0B

#endif /* INC_PINMAP_H_ */
