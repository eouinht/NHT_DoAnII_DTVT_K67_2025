#include "I2C.h"

// Khoi tao module TWI (I2C) tren ATmega16
void I2C_Init() {
	// Dat prescaler = 1 (TWPS1:TWPS0 = 00)
	TWSR = 0x00;
	
	// Dat bitrate de dat toc do 100 kHz (da tinh trong I2C.h)
	TWBR = BITRATE_VALUE;
	
	// Kich hoat module TWI
	TWCR = (1 << TWEN);
}

// Gui dieu kien START va dia chi thiet bi (ghi)
uint8_t I2C_Start(char write_address) {
	uint8_t status;

	// Gui dieu kien START
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT))); // Cho den khi START duoc gui

	// Kiem tra trang thai: 0x08 la START condition
	status = TWSR & 0xF8;
	if (status != 0x08) {
		I2C_Stop(); // Giai phong bus neu co loi
		return 0;   // Tra ve 0 neu START that bai
	}

	// Gui dia chi thiet bi (SLA+W)
	TWDR = write_address;
	TWCR = (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT))); // Cho phan hoi tu thiet bi

	// Kiem tra trang thai
	status = TWSR & 0xF8;
	if (status == 0x18) {       // SLA+W truyen thanh cong, nhan duoc ACK
		return 1;
		} else if (status == 0x20) { // SLA+W truyen thanh cong, nhan duoc NACK
		I2C_Stop();
		return 2;
		} else {                     // Loi khac
		I2C_Stop();
		return 3;
	}
}

// Gui dieu kien Repeated START va dia chi thiet bi (doc)
uint8_t I2C_Repeated_Start(char read_address) {
	uint8_t status;

	// Gui dieu kien Repeated START
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT))); // Cho den khi Repeated START duoc gui

	// Kiem tra trang thai: 0x10 la Repeated START condition
	status = TWSR & 0xF8;
	if (status != 0x10) {
		I2C_Stop(); // Giai phong bus neu co loi
		return 0;   // Tra ve 0 neu Repeated START that bai
	}

	// Gui dia chi thiet bi (SLA+R)
	TWDR = read_address;
	TWCR = (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT))); // Cho phan hoi tu thiet bi

	// Kiem tra trang thai
	status = TWSR & 0xF8;
	if (status == 0x40) {       // SLA+R truyen thanh cong, nhan duoc ACK
		return 1;
		} else if (status == 0x48) { // SLA+R truyen thanh cong, nhan duoc NACK
		I2C_Stop();
		return 2;
		} else {                     // Loi khac
		I2C_Stop();
		return 3;
	}
}

// Gui dieu kien STOP de giai phong bus I2C
void I2C_Stop() {
	// Gui dieu kien STOP
	TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);
	
	// Cho den khi STOP hoan tat (thuong khong can thiet tren ATmega16)
	while (TWCR & (1 << TWSTO));
}

// Ghi mot byte du lieu vao thiet bi
uint8_t I2C_Write(char data) {
	uint8_t status;

	// Ghi du lieu vao TWDR
	TWDR = data;
	TWCR = (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT))); // Cho truyen xong

	// Kiem tra trang thai
	status = TWSR & 0xF8;
	if (status == 0x28) {       // Du lieu truyen thanh cong, nhan duoc ACK
		return 0;
		} else if (status == 0x30) { // Du lieu truyen thanh cong, nhan duoc NACK
		I2C_Stop();
		return 1;
		} else {                     // Loi khac
		I2C_Stop();
		return 2;
	}
}

// Doc mot byte tu thiet bi va gui ACK (dung khi con byte tiep theo can doc)
int I2C_Read_ACK() {
	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA); // Kich hoat ACK
	while (!(TWCR & (1 << TWINT))); // Cho doc xong
	return TWDR; // Tra ve byte da doc
}

// Doc mot byte tu thiet bi va khong gui ACK (dung cho byte cuoi cung)
int I2C_Read_Nack() {
	TWCR = (1 << TWEN) | (1 << TWINT); // Khong kich hoat ACK
	while (!(TWCR & (1 << TWINT))); // Cho doc xong
	return TWDR; // Tra ve byte da doc
}