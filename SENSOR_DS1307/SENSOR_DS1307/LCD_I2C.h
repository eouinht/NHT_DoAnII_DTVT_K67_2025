#ifndef LCD_I2C_H
#define LCD_I2C_H
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

#ifdef __cplusplus
extern "C" {
	#endif

	// =========================
	// CAU HINH DIA CHI I2C CUA LCD
	// =========================

	// Dia chi I2C 7-bit cua LCD (thuong la 0x27 hoac 0x3F)
	// Nen dung I2C scanner de kiem tra dia chi thuc te
	#define LCD_I2C_ADDR 0x27 

	// =========================
	// CAU HINH LCD
	// =========================

	// Cac bit dieu khien LCD
	#define LCD_BACKLIGHT   0x08
	#define LCD_NOBACKLIGHT 0x00

	#define LCD_ENABLE  0x04
	#define LCD_READ    0x02
	#define LCD_WRITE   0x00
	#define LCD_COMMAND 0x00
	#define LCD_DATA    0x01
	#define FRE 8
	
	// =========================
	// CAU HINH TOC DO I2C
	// =========================

	// Thiet lap toc do I2C = 100kHz voi F_CPU = 8MHz, Prescaler = 1
	// Cong thuc:
	// SCL = F_CPU / (16 + 2*TWBR*4^TWPS)
	// => TWBR = (F_CPU / SCL - 16) / 2 = (8000000 / 100000 - 16) / 2 = 32
	#define BITRATE_VALUE 32

	// =========================
	// NGUYEN MAU CAC HAM I2C
	// =========================

	void I2C_Init(void);
	uint8_t I2C_Start(uint8_t write_address);
	uint8_t I2C_Repeated_Start(uint8_t read_address);
	void I2C_Stop(void);
	uint8_t I2C_Write(uint8_t data);
	int I2C_Read_ACK(void);
	int I2C_Read_Nack(void);

	// =========================
	// NGUYEN MAU CAC HAM LCD I2C
	// =========================

	void lcd_send_nibble(uint8_t nibble);
	void lcd_send_byte(uint8_t value, uint8_t mode);
	void lcd_send_command(uint8_t cmd);
	void lcd_send_data(uint8_t data);
	void lcd_init(void);
	void lcd_clear(void);
	void lcd_home(void);
	void lcd_print_xy(uint8_t row, uint8_t col, const char* str);
	//Khai bao nguyen mau ham rieng

	void INIT();
	void PORT();
	void LED7_OUT(unsigned char num);
	void PB_2_LED();
	unsigned char PB_CHECK();

	#ifdef __cplusplus
}
#endif

#endif /* LCD_I2C_H */
