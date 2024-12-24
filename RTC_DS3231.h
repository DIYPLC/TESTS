// Driver RTC DS3231 tested STM32F103C8T6 I2C port 1

// STM32CubeIDE Version: 1.6.1
// STM32CubeF1 Firmware Package V1.8.3

// static struct DbRtc DbRtc1 = { 0 };

// FbRtcRead(&DbRtc1);
// uint8_t Seconds = DbRtc1.Seconds; // Секунда 0...59
// uint8_t Minutes = DbRtc1.Minutes; // Минута 0...59
// uint8_t Hour = DbRtc1.Hour; // Час 0...23
// uint8_t Weekday = DbRtc1.Weekday; // День недели 1...7
// uint8_t Date = DbRtc1.Date; // День 1...31
// uint8_t Month = DbRtc1.Month; // Месяц 1...12
// uint16_t Year = DbRtc1.Year; // Год 2000...2099
// int8_t rtc_aging_offset = DbRtc1.rtc_aging_offset; // Константа для подстройки хода часов.
// float TemperatureCelsius = DbRtc1.TemperatureCelsius; // Temperature deg C

// DbRtc1.Seconds = 33; // Секунда 0...59
// DbRtc1.Minutes = 01; // Минута 0...59
// DbRtc1.Hour = 14; // Час 0...23
// DbRtc1.Weekday = 1; // День недели 1...7
// DbRtc1.Date = 9; // День 1...31
// DbRtc1.Month = 12; // Месяц 1...12
// DbRtc1.Year = 2024; // Год 2000...2099
// DbRtc1.AgingOffset = 0;
// FbRtcWrite(&DbRtc1);

// Last modified date: 21-dec-2024

#define RTC_DS3231_EN (1)

#if (RTC_DS3231_EN > 0)

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#define I2C_HANDLE hi2c1 // Привязка порту I2C номер 1.
//#define I2C_HANDLE hi2c2 // Привязка порту I2C номер 2.

struct DbRtc {
	uint8_t Seconds; // Секунда 0...59
	uint8_t Minutes; // Минута 0...59
	uint8_t Hour; // Час 0...23
	uint8_t Weekday; // День недели 1...7
	uint8_t Date; // День 1...31
	uint8_t Month; // Месяц 1...12
	uint16_t Year; // Год 2000...2099
	int8_t AgingOffset; // Константа для подстройки хода часов.
	float TemperatureCelsius; // Temperature deg C
	bool Error; // Ошибка часов.
	bool Blink; // Мигание по секундам от часов.
};

void FbRtcRead(struct DbRtc *p);
void FbRtcWrite(struct DbRtc *p);

#ifdef __cplusplus
}
#endif

#endif

// @COPYLEFT ALL WRONGS RESERVED :)
// Author: VA
// Contacts: DIY.PLC.314@gmail.com
// Date foundation library LIB_PLC: 2014
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
