#include <stdint.h>
#include <stdbool.h>
#include <iso646.h>
#include "RTC_DS3231.h"
#include "main.h"

#define I2C_ADR_MAIN 0x68 // 0b1101000
#define I2C_ADR_READ  ((uint16_t)(I2C_ADR_MAIN << 1)) // 0xD1 0b11010000
#define I2C_ADR_WRITE ((uint16_t)(I2C_ADR_MAIN << 1)) // 0xD0 0b11010001 WTF TODO
#define I2C_SIZE_READ_BYTES ((uint16_t) 19)
#define I2C_SIZE_WRITE_BYTES ((uint16_t) 1)
#define I2C_BUFFER_SIZE_BYTES 22
#define I2C_TIMEOUT_MS ((uint32_t) 50)

#define DS3231_SEC_REG        0x00
#define DS3231_MIN_REG        0x01
#define DS3231_HOUR_REG       0x02
#define DS3231_WDAY_REG       0x03
#define DS3231_MDAY_REG       0x04
#define DS3231_MONTH_REG      0x05
#define DS3231_YEAR_REG       0x06
#define DS3231_AL1SEC_REG     0x07
#define DS3231_AL1MIN_REG     0x08
#define DS3231_AL1HOUR_REG    0x09
#define DS3231_AL1WDAY_REG    0x0A
#define DS3231_AL2MIN_REG     0x0B
#define DS3231_AL2HOUR_REG    0x0C
#define DS3231_AL2WDAY_REG    0x0D
#define DS3231_CONTROL_REG          0x0E
#define DS3231_STATUS_REG           0x0F
#define DS3231_AGING_OFFSET_REG     0x10
#define DS3231_TMP_UP_REG           0x11
#define DS3231_TMP_LOW_REG          0x12

#define Seconds p->Seconds
#define Minutes p->Minutes
#define Hour    p->Hour
#define Weekday p->Weekday
#define Date    p->Date
#define Month   p->Month
#define Year    p->Year
#define AgingOffset   p->AgingOffset
#define TemperatureCelsius p->TemperatureCelsius
#define RtcError p->RtcError
#define I2C_BUFFER ((uint8_t*) &i2c_buffer)

extern I2C_HandleTypeDef I2C_HANDLE;

static uint8_t BCD_TO_UINT(uint8_t in) {
	return ((in >> 4) * 10 + (0x0F & in));
}
static uint8_t UINT_TO_BCD(uint8_t in) {
	return (((in / 10) << 4) | (in % 10));
}

void FbRtcRead(struct DbRtc *p) {

	// Записываем начальный адрес регистра для чтения 0
	uint8_t i2c_buffer[I2C_BUFFER_SIZE_BYTES];
	i2c_buffer[0] = 0;
	HAL_StatusTypeDef status;
	status = HAL_I2C_Master_Transmit(&I2C_HANDLE, I2C_ADR_WRITE, I2C_BUFFER,
	I2C_SIZE_WRITE_BYTES, I2C_TIMEOUT_MS);
	RtcError = (status != HAL_OK);

	// Читаем 19 байт с адреса регистра 0
	// TODO работает нестабильно требует иногда перезагрузки, возможно из за дребезга проводов.
	status = HAL_I2C_Master_Receive(&I2C_HANDLE, I2C_ADR_READ, I2C_BUFFER,
	I2C_SIZE_READ_BYTES, I2C_TIMEOUT_MS);
	RtcError = (status != HAL_OK);

	if (RtcError) {
		Seconds = 0;
		Minutes = 0;
		Hour = 0;
		Weekday = 0;
		Date = 0;
		Month = 0;
		Year = 0;
		AgingOffset = 0; // TODO
		TemperatureCelsius = 0.0;
	} else {
		Seconds = BCD_TO_UINT(i2c_buffer[DS3231_SEC_REG] & ~0b10000000); // 0...59 s
		Minutes = BCD_TO_UINT(i2c_buffer[DS3231_MIN_REG] & ~0b10000000); // 0...59 m
		Hour = BCD_TO_UINT(i2c_buffer[DS3231_HOUR_REG] & ~0b11000000); // 0...23 h Ignore 24 Hour bit
		Weekday = BCD_TO_UINT(i2c_buffer[DS3231_WDAY_REG] & ~0b11111000); // 1...7 wd
		Date = BCD_TO_UINT(i2c_buffer[DS3231_MDAY_REG] & ~0b11000000); // 1...31 d
		Month = BCD_TO_UINT(i2c_buffer[ DS3231_MONTH_REG] & ~0b11100000); // 1...12 m
		Year = BCD_TO_UINT(i2c_buffer[DS3231_YEAR_REG]) + 2000; // 2000...2099 e

		AgingOffset = i2c_buffer[DS3231_AGING_OFFSET_REG]; // TODO

		uint8_t temperature_up = i2c_buffer[DS3231_TMP_UP_REG]; //Two's complement form
		uint8_t temperature_lo = i2c_buffer[DS3231_TMP_LOW_REG] & ~0b00111111; //Fractional part

		if (temperature_up & 0b10000000) //check if -ve number
				{
			temperature_up ^= 0b11111111;
			temperature_up += 0x1;
			TemperatureCelsius = temperature_up
					+ ((temperature_lo >> 6) * 0.25);
			TemperatureCelsius = TemperatureCelsius * -1;
		} else {
			TemperatureCelsius = temperature_up
					+ ((temperature_lo >> 6) * 0.25);
		}
	}

	HAL_Delay(4); // TODO
	return;
}

void FbRtcWrite(struct DbRtc *p) {

	uint8_t i2c_buffer[I2C_BUFFER_SIZE_BYTES];
	i2c_buffer[0] = 0; // Пишем данные с адреса 0
	i2c_buffer[DS3231_SEC_REG + 1] = UINT_TO_BCD(Seconds) & ~0b10000000; // 0...59 s
	i2c_buffer[DS3231_MIN_REG + 1] = UINT_TO_BCD(Minutes) & ~0b10000000; // 0...59 m
	i2c_buffer[DS3231_HOUR_REG + 1] = UINT_TO_BCD(Hour) & ~0b11000000; // 0...23 h Ignore 24 Hour bit
	i2c_buffer[DS3231_WDAY_REG + 1] = UINT_TO_BCD(Weekday) & ~0b11111000; // 1...7 wd
	i2c_buffer[DS3231_MDAY_REG + 1] = UINT_TO_BCD(Date) & ~0b11000000; // 1...31 d
	i2c_buffer[DS3231_MONTH_REG + 1] = UINT_TO_BCD(Month) & ~0b11100000; // 1...12 m
	i2c_buffer[DS3231_YEAR_REG + 1] = UINT_TO_BCD(Year - 2000); // 2000...2099 e
	HAL_StatusTypeDef status;
	status = HAL_I2C_Master_Transmit(&I2C_HANDLE, I2C_ADR_WRITE, I2C_BUFFER,
			(uint16_t) 8, I2C_TIMEOUT_MS);
	RtcError = (status != HAL_OK);
	HAL_Delay(4); // TODO

	// TODO Write AgingOffset
	i2c_buffer[0] = DS3231_AGING_OFFSET_REG;
	i2c_buffer[1] = AgingOffset;
	status = HAL_I2C_Master_Transmit(&I2C_HANDLE, I2C_ADR_WRITE, I2C_BUFFER,
			(uint16_t) 2, I2C_TIMEOUT_MS);
	RtcError = (status != HAL_OK);
	HAL_Delay(4);
	return;
}

// https://narodstream.ru/stm-urok-9-hal-shina-i2c-prodolzhaem-rabotu-s-ds3231/
// https://github.com/HexRx/ds3231_driver/blob/master/ds3231.c
// https://github.com/SodaqMoja/Sodaq_DS3231/blob/master/src/Sodaq_DS3231.cpp
// https://github.com/switchdoclabs/RTC_SDL_DS3231.git
// https://habr.com/ru/articles/736482/
// https://rxtx.su/mikrokontrollery/stmicroelectronics/i2c-v-stm32-hal/?ysclid=m4grhvxtyf685040595

/*
 DS3231 + raspberry pi set time algoritm
 apt install i2c-tools
 i2cdetect -y 1
 sudo git clone https://github.com/switchdoclabs/RTC_SDL_DS3231.git
 nano RTCmodule.py
 import SDL_DS3231
 ds3231 = SDL_DS3231.SDL_DS3231(1, 0x68)
 ds3231.write_now() #set ntp time to ds3231
 print(ds3231.read_datetime())
 */

//  +---------+
//  | GNU GPL |
//  +---------+
//  |
//  |
//  .= .-_-. =.
// ((_/)o o(\_))
//  `-'(. .)`-'
//  |/| \_/ |\
//  ( |     | )
//  /"\_____/"\
//  \__)   (__/
// @COPYLEFT ALL WRONGS RESERVED :)
// Author: VA
// Contacts: DIY.PLC.314@gmail.com
// Date: 2014 - 2024
// License: GNU GPL-2.0-or-later
// https://www.gnu.org/licenses/old-licenses/gpl-2.0.html
// https://www.youtube.com/watch?v=n1F_MfLRlX0
//
// See also:
// https://www.youtube.com/@DIY_PLC
// https://github.com/DIYPLC/LIB_PLC
// https://oshwlab.com/diy.plc.314/PLC_HW1_SW1
// https://3dtoday.ru/3d-models/mechanical-parts/body/korpus-na-din-reiku
// https://t.me/DIY_PLC
