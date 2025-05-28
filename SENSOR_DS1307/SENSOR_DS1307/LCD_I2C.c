#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include "lcd_i2c.h"

// Dinh nghia ma loi I2C
#define I2C_OK           0
#define I2C_NACK         1
#define I2C_OTHER_ERROR  2
#define I2C_TIMEOUT      3
unsigned char push_button = 0;  
// Bien luu trang thai den nen
static uint8_t backlight_state = LCD_BACKLIGHT;



// =============================
// CAC HAM I2C CO BAN
// =============================

// Khoi tao I2C
void I2C_Init(void) {
	TWSR = 0x00;              // Thanh ghi trang thai dat ve 0 | Prescaler = 1 ( Khong chia tan so ) 
	TWBR = BITRATE_VALUE;     // Bit rate register |Chon toc do tan so Vi du 100 kHz voi F_CPU = 8MHz
	TWCR = (1 << TWEN);       // Thanh ghi dieu khien| Kich hoat I2C
}

// Gui dieu kien START va dia chi thiet bi ghi
uint8_t I2C_Start(uint8_t write_address) {
	uint16_t timeout = 10000;

	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT))) {
		if (--timeout == 0) return I2C_TIMEOUT;
	}

	if ((TWSR & 0xF8) != 0x08) return I2C_OTHER_ERROR;

	TWDR = write_address;
	TWCR = (1 << TWEN) | (1 << TWINT);
	timeout = 10000;
	while (!(TWCR & (1 << TWINT))) {
		if (--timeout == 0) return I2C_TIMEOUT;
	}

	uint8_t status = TWSR & 0xF8;
	if (status == 0x18) return I2C_OK;
	else if (status == 0x20) return I2C_NACK;
	else return I2C_OTHER_ERROR;
}

// Gui Repeated START va dia chi doc
uint8_t I2C_Repeated_Start(uint8_t read_address) {
	uint16_t timeout = 10000;

	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT))) {
		if (--timeout == 0) return I2C_TIMEOUT;
	}

	if ((TWSR & 0xF8) != 0x10) return I2C_OTHER_ERROR;

	TWDR = read_address;
	TWCR = (1 << TWEN) | (1 << TWINT);
	timeout = 10000;
	while (!(TWCR & (1 << TWINT))) {
		if (--timeout == 0) return I2C_TIMEOUT;
	}

	uint8_t status = TWSR & 0xF8;
	if (status == 0x40) return I2C_OK;
	else if (status == 0x48) return I2C_NACK;
	else return I2C_OTHER_ERROR;
}

// Gui STOP de giai phong bus
void I2C_Stop(void) {
	TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);
	uint16_t timeout = 10000;
	while (TWCR & (1 << TWSTO)) {
		if (--timeout == 0) break;
	}
}

// Ghi mot byte
uint8_t I2C_Write(uint8_t data) {
	uint16_t timeout = 10000;
	TWDR = data;
	TWCR = (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT))) {
		if (--timeout == 0) return I2C_TIMEOUT;
	}

	uint8_t status = TWSR & 0xF8;
	if (status == 0x28) return I2C_OK;
	else if (status == 0x30) return I2C_NACK;
	else return I2C_OTHER_ERROR;
}

// Doc ACK
int I2C_Read_ACK(void) {
	uint16_t timeout = 10000;
	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA);
	while (!(TWCR & (1 << TWINT))) {
		if (--timeout == 0) return -1;
	}
	return TWDR;
}

// Doc NACK
int I2C_Read_Nack(void) {
	uint16_t timeout = 10000;
	TWCR = (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT))) {
		if (--timeout == 0) return -1;
	}
	return TWDR;
}

// =============================
// CAC HAM LCD QUA I2C
// =============================

// Ghi mot byte thap cap
void lcd_write_raw(uint8_t data) {
	I2C_Write(data | LCD_ENABLE | backlight_state);
	_delay_us(50);
	I2C_Write((data & ~LCD_ENABLE) | backlight_state);
	_delay_us(100);
}

// Gui nibble
void lcd_send_nibble(uint8_t nibble) {
	I2C_Start(LCD_I2C_ADDR << 1);
	lcd_write_raw(nibble);
	I2C_Stop();
}

// Gui 1 byte du lieu hoac lenh
void lcd_send_byte(uint8_t value, uint8_t mode) {
	uint8_t high = value & 0xF0;
	uint8_t low = (value << 4) & 0xF0;

	I2C_Start(LCD_I2C_ADDR << 1);
	lcd_write_raw(high | mode);
	lcd_write_raw(low | mode);
	I2C_Stop();
}

// Gui lenh
void lcd_send_command(uint8_t cmd) {
	lcd_send_byte(cmd, LCD_COMMAND);
}

// Gui du lieu
void lcd_send_data(uint8_t data) {
	lcd_send_byte(data, LCD_DATA);
}

// Khoi tao LCD
void lcd_init(void) {
	_delay_ms(50);
	lcd_send_nibble(0x30);
	_delay_ms(5);
	lcd_send_nibble(0x30);
	_delay_us(150);
	lcd_send_nibble(0x30);
	_delay_us(150);
	lcd_send_nibble(0x20); // 4-bit mode

	lcd_send_command(0x28); // 2 lines, 5x8 font
	lcd_send_command(0x0C); // Display ON, cursor OFF
	lcd_send_command(0x06); // Entry mode
	lcd_clear();
}

// Xoa man hinh
void lcd_clear(void) {
	lcd_send_command(0x01);
	_delay_ms(2);
}

// Dua ve dau dong
void lcd_home(void) {
	lcd_send_command(0x02);
	_delay_ms(2);
}

// In chuoi tai vi tri
void lcd_print_xy(uint8_t row, uint8_t col, const char* str) {
	if (col > 15) col = 15;

	uint8_t addr;
	switch (row) {
		case 0: addr = 0x80 + col; break;
		case 1: addr = 0xC0 + col; break;
		case 2: addr = 0x94 + col; break;
		case 3: addr = 0xD4 + col; break;
		default: addr = 0x80; break;
	}

	lcd_send_command(addr);

	while (*str) {
		lcd_send_data(*str++);
	}
}


// Cac ham rieng de su dung nut bam va led 7 thanh 
void INIT()
{

	// Khoi tao trang thai output cho cac chan noi toi led don
	DDRD |= 0xFF;
	// Khoi tao trang thai logic 1 cho cac chan noi toi cac LED don
	PORTD |= 0xFF;
	// Khoi tao trang thai output cho cac chan noi toi led 7 thanh
	DDRC = 0xFF;
	// Khoi tao trang thai logic 1 cho cac chan noi toi led 7 thanh
	PORTC |= 0xFF;
	
	DDRA = 0x00;
	PORTA = 0x00;
	// Goi cac ham va khoi tao tham so cho bo ADC

}

void PORT()
{
	/* Khai bao cac bien se dung toi trong ham nay*/
	// Bien led_shift = 255;
	// Gia tri dau tien la 255 = 0xFF = 0b11111111 -> Tat ca ca LED deu tat
	unsigned char led_shift = 255;
	// Bien dem cho LED 7 thanh, gia tri bat dau la
	unsigned char led_7_count = 0;
	// Vong for giip LED sang/tat theo quy luat lap di lap lai
	
	for(;;)
	{
		/*-------------------------------- Dieu khien cac LED don--------------------------- */
		
		// cac LED sang/tat theo 8 bit cua bien led_shift
		PORTD = led_shift;
		// thay doi bien led_shift
		if(led_shift!=0)               //Neu led shift khac 0
		led_shift = led_shift << 1;//Dich trai 1 bit
		else
		led_shift = 255; // Tro lai gia tri 255
		
		/*---------------------------Doan ma dieu khien LED 7 thanh----------------------------------*/
		// Xuat gia tri dem ra LED 7 thanh
		LED7_OUT(led_7_count);
		// Dao trang thai PC3 de nhap nhay dau cham tren LED 7 thanh
		PORTC ^= (1<<PC3);
		// Tang dan gia tri dem
		led_7_count = led_7_count + 1;
		
		// Khi vuot qua 9, gia tri dem duoc reset ve 0
		if(led_7_count > 9)
		led_7_count = 0;
		
		// Ham tre khoang 0.5s
		_delay_ms(1000);
	}
	
}
void LED7_OUT(unsigned char num)
{
	// Khai bao bien temp luu trang thai cura PORTC
	unsigned char temp = PORTC;
	// Chan vi dieu khien ung voi thanh LED
	// a = PC5
	// b - PC4
	// c - PC2
	// d - PC1
	// e - PC0
	// f - PC6
	// g - PC&
	// dot - PC3
	
	// Tat cac doan LED hien dang sang truoc khi sang cac doan LED moi
	temp &= 0B00001000;
	// gan muc logic cho 8 bit cua bien temp ung voi gia tri bien num
	switch(num)
	{
		case 0: temp |= 0B10000000; break;
		case 1: temp |= 0B11100011; break;
		case 2: temp |= 0B01000100; break;
		case 3: temp |= 0B01000001; break;
		case 4: temp |= 0B00100011; break;
		case 5: temp |= 0B00010001; break;
		case 6: temp |= 0B00010000; break;
		case 7: temp |= 0B11000011; break;
		case 8: temp |= 0B00000000; break;
		case 9: temp |= 0B00000001; break;
		
	}
	// Xuat gia tri logic moi ra PORTC de lam led 7 thanh sang
	PORTC = temp;
	
}
void PB_2_LED()
{
	for(;;)
	{
		// Goi ham quet phim
		push_button = PB_CHECK();
		// Hiejn so thu tu phim an ra LED 7 thanh
		LED7_OUT(push_button);
		// Dieu khien hang led don
		switch(push_button)
		{
			// Neu push_button = 1, sang 2 led ngoai cung ben trai
			case 1: PORTD = 0b11111100; break;
			case 2: PORTD = 0b11110011; break;
			case 3: PORTD = 0b11001111; break;
			case 4: PORTD = 0b00111111; break;
			// push_button = 0, tat tat ca ca led
			default: PORTD = 0xFF;
			
		}
	}
}
unsigned char PB_CHECK()
{
	// Kiem tra trang thai logic cua chan 4 PB0-3. Neu khac 1111
	if ((PINB & 0x0F) != 0x0F)
	{
		// Kiem tra PB0. Neu muc  logic 0, ham ket thuc va = 1
		if(!(PINB &(1<<PB0)))
		return 1;
		// Kiem tra PB1. Neu muc logic 0, ham ket thuc va bang = 2
		if(!(PINB &(1<<PB1)))
		return 2;
		if(!(PINB &(1<<PB2)))
		return 3;
		if(!(PINB &(1<<PB3)))
		return 4;
	}
	return push_button;
}