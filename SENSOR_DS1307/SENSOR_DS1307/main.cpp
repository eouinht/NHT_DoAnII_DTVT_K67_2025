#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <util/delay.h>
#include "lcd_i2c.h"


#define DS1307_ADDRESS        0x68
#define Device_write_address  0xD0
#define Device_Read_address   0xD1
#define TimeFormat12          0x40
#define AMPM                  0x20
bool isLunarMode = false;
int second, minute, hour, day, date, month, year;

// Convert BCD to Decimal
int bcdToDec(uint8_t bcd) {
	return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

bool IsItPM(int hour_) {
	return (hour_ & AMPM) ? true : false;
}

// Set time (once only)
void RTC_SetTime(char h, char m, char s) {
	I2C_Start(Device_write_address);
	I2C_Write(0x00);     // Start at seconds register
	I2C_Write(s & 0x7F); // Bit 7 = 0 to enable oscillator
	I2C_Write(m);
	I2C_Write(h);
	I2C_Stop();
	_delay_ms(10);
}

// Set date (once only)
void RTC_SetDate(char d, char dt, char mo, char yr) {
	I2C_Start(Device_write_address);
	I2C_Write(0x03); // Start at day register
	I2C_Write(d);
	I2C_Write(dt);
	I2C_Write(mo);
	I2C_Write(yr);
	I2C_Stop();
	_delay_ms(10);
}

// Read time from RTC
void RTC_Read_Clock(char read_clock_address) {
	I2C_Start(Device_write_address);
	I2C_Write(read_clock_address);
	I2C_Repeated_Start(Device_Read_address);
	
	second = I2C_Read_ACK();
	minute = I2C_Read_ACK();
	hour = I2C_Read_Nack();
	I2C_Stop();
}

// Read calendar from RTC
void RTC_Read_Calendar(char read_calendar_address) {
	I2C_Start(Device_write_address);
	I2C_Write(read_calendar_address);
	I2C_Repeated_Start(Device_Read_address);
	
	day = I2C_Read_ACK();
	date = I2C_Read_ACK();
	month = I2C_Read_ACK();
	year = I2C_Read_Nack();
	I2C_Stop();
}

// Count days since January 1st
int daysSinceStart(int d, int m, int y) {
	int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
	if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) {
		daysInMonth[1] = 29;
	}

	int days = 0;
	for (int i = 0; i < m - 1; i++) {
		days += daysInMonth[i];
	}
	days += d;
	return days;
}

// Approximate lunar date conversion (Vietnam, for year 2025 only)
void convertToLunar(int d, int m, int y, int* ld, int* lm, int* ly) {
	const int tet_day = 29;
	const int tet_month = 1;

	// Lunar months for 2025 (approximate), no leap month
	const int lunar_month_days[] = {30, 29, 30, 29, 30, 29, 30, 29, 30, 29, 30, 29};

	int current_day = daysSinceStart(d, m, y);
	int tet_day_of_year = daysSinceStart(tet_day, tet_month, y);
	int delta = current_day - tet_day_of_year;

	if (delta < 0) {
		// Before T?t
		*ld = 0;
		*lm = 0;
		*ly = y;
		return;
	}

	int month = 1;
	for (int i = 0; i < 12; i++) {
		if (delta < lunar_month_days[i]) {
			*ld = delta + 1;
			*lm = month;
			*ly = y;
			return;
		}
		delta -= lunar_month_days[i];
		month++;
	}

	// Overflow (fallback)
	*ld = 1;
	*lm = 1;
	*ly = y + 1;
}

int main(void) {
	char buffer[21];
	const char* days_str[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	int lunar_d, lunar_m, lunar_y;

	I2C_Init();
	lcd_init();

	 //Uncomment if you want to set time once
	RTC_SetDate(0x04, 0x28, 0x05, 0x25); // VD Sunday, 24/05/2025
	RTC_SetTime(0x19, 0x10, 0x00);      // VD 14:56:00

	// === Setup PB1 as input with pull-up ===
	DDRB &= ~(1 << PB1);    // Set PB1 as input
	PORTB |= (1 << PB1);    // Enable pull-up resistor

	while(1) {
		RTC_Read_Clock(0);
		RTC_Read_Calendar(3);

		int d = bcdToDec(date);
		int m = bcdToDec(month);
		int y = bcdToDec(year);

		// === Toggle mode if button is pressed ===
		if (!(PINB & (1 << PB1))) {
			isLunarMode = !isLunarMode;
			_delay_ms(300);  // debounce
		}

		// === Row 0: Time ===
		if (hour & TimeFormat12) {
			sprintf(buffer, "%02x:%02x:%02x", (hour & 0b00011111), minute, second);
			lcd_print_xy(0, 0, buffer);
			if (IsItPM(hour)) lcd_print_xy(0, 9, "PM");
			else              lcd_print_xy(0, 9, "AM");
			} else {
			sprintf(buffer, "%02x:%02x:%02x", (hour & 0b00111111), minute, second);
			lcd_print_xy(0, 0, buffer);
		}

		// === Row 1: Date or Lunar Date ===
		if (isLunarMode) {
			convertToLunar(d, m, y, &lunar_d, &lunar_m, &lunar_y);
			if (lunar_d != 0) {
				sprintf(buffer, "AL: %02d/%02d/%02d %s", lunar_d, lunar_m, lunar_y,days_str[day-1]);
				} else {
				sprintf(buffer, "Truoc Tet          ");
			}
			} else {
			sprintf(buffer, "DL: %02x/%02x/%02x %s", date, month, year, days_str[day - 1]);
		}
		lcd_print_xy(1, 0, buffer);

		_delay_ms(300);
	}
	//INIT();
	//PORT();
	//PB_2_LED();
	return 0;
}
