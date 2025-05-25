#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>

// Tinh gia tri TWBR de dat toc do I2C 100 kHz voi F_CPU = 8 MHz va prescaler = 1
// Cong thuc: SCL frequency = F_CPU / (16 + 2 * TWBR * 4^TWPS)
// Voi F_CPU = 8 MHz, SCL = 100 kHz, TWPS = 0 (prescaler = 1):
// 100,000 = 8,000,000 / (16 + 2 * TWBR * 1)
// TWBR = (8,000,000 / 100,000 - 16) / 2 = 32
#define BITRATE_VALUE 32

// Khai bao cac ham I2C
void I2C_Init(void);
uint8_t I2C_Start(char write_address);
uint8_t I2C_Repeated_Start(char read_address);
void I2C_Stop(void);
uint8_t I2C_Write(char data);
int I2C_Read_ACK(void);
int I2C_Read_Nack(void);

#endif /* I2C_H_ */