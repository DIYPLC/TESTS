// Черновик протокола связи PLC + HMI.
// NET_PLC Протокол обмена данными с ПЛК.
// Модель обмена данными ведущий / ведомый.
// По мотивам MODBUS, PROFIBUS DP.
// Физический уровень UART, RS485, Ethernet, LoRa.
// Формат кадра UART по умолчанию - 9600 8E1 (всего 11бит)
// Фиксированный размер кадра для совместимости с библиокекой HAL от STM32.
// Передаем массив байт а потом интерпретируем их как любой необходимый тип данных.
// Передача пакета старшим байтом вперед.
// Дата последней ревизии 24-dec-2024

#include <stdint.h>
#include <stdbool.h>
#include <iso646.h>
#include "NET_PLC.h"

#if (NET_PLC_EN > 0)

#define DEBUG_MODE (1) // Отключить шифрование для отладки.
#define SD3_PROFIBUS_DP (0xA2) // Стартовый байт.
#define ED_PROFIBUS_DP (0x16) // Стоповый байт.
#define BROADCAST_SLAVE_ADR (0xFF)// Адрес 255 зарезервирован для групповой чаписи без ответа.
#define FUN_READ_8_BYTES (1) // Код функции- Прочитать 64 байта из ПЛК.
#define FUN_WRITE_8_BYTES (2) // Код функции- Записать 64 байта в ПЛК.
#define FUN_READ_WRITE_8_BYTES (3) // Код функции- Записать 64 байта в ПЛК и считать 64 байта из ПЛК.
#define FUN_START_PLC (10) // Код функции- Останов ПЛК.
#define FUN_STOP_PLC (11) // Код функции- Останов ПЛК.
#define UDP_PORT_NUMBER (5002)
#define TCP_PORT_NUMBER (5002)

#if DEBUG_MODE > 0
#define KEY_RX_BYTE00 (0x00)
#define KEY_RX_BYTE01 (0x00)
#define KEY_RX_BYTE02 (0x00)
#define KEY_RX_BYTE03 (0x00)
#define KEY_RX_BYTE04 (0x00)
#define KEY_RX_BYTE05 (0x00)
#define KEY_RX_BYTE06 (0x00)
#define KEY_RX_BYTE07 (0x00)
#define KEY_RX_BYTE08 (0x00)
#define KEY_RX_BYTE09 (0x00)
#define KEY_RX_BYTE10 (0x00)
#define KEY_TX_BYTE00 (0x00)
#define KEY_TX_BYTE01 (0x00)
#define KEY_TX_BYTE02 (0x00)
#define KEY_TX_BYTE03 (0x00)
#define KEY_TX_BYTE04 (0x00)
#define KEY_TX_BYTE05 (0x00)
#define KEY_TX_BYTE06 (0x00)
#define KEY_TX_BYTE07 (0x00)
#define KEY_TX_BYTE08 (0x00)
#define KEY_TX_BYTE09 (0x00)
#define KEY_TX_BYTE10 (0x00)
#else
#define KEY_RX_BYTE00 (0x4a)
#define KEY_RX_BYTE01 (0xef)
#define KEY_RX_BYTE02 (0xbc)
#define KEY_RX_BYTE03 (0x82)
#define KEY_RX_BYTE04 (0x12)
#define KEY_RX_BYTE05 (0x99)
#define KEY_RX_BYTE06 (0x70)
#define KEY_RX_BYTE07 (0xbe)
#define KEY_RX_BYTE08 (0xfe)
#define KEY_RX_BYTE09 (0xda)
#define KEY_RX_BYTE10 (0xed)
#define KEY_TX_BYTE00 (0x77)
#define KEY_TX_BYTE01 (0x23)
#define KEY_TX_BYTE02 (0x55)
#define KEY_TX_BYTE03 (0x04)
#define KEY_TX_BYTE04 (0x7c)
#define KEY_TX_BYTE05 (0xaa)
#define KEY_TX_BYTE06 (0x20)
#define KEY_TX_BYTE07 (0xee)
#define KEY_TX_BYTE08 (0x94)
#define KEY_TX_BYTE09 (0xf8)
#define KEY_TX_BYTE10 (0x1a)
#endif

#define Rx_Start_byte      (p->Message[0])
#define Rx_Message_counter (p->Message[1])
#define Rx_Adress_slave    (p->Message[2])
#define Rx_Function_code   (p->Message[3])
#define Rx_Data_value0     (p->Message[4])
#define Rx_Data_value1     (p->Message[5])
#define Rx_Data_value2     (p->Message[6])
#define Rx_Data_value3     (p->Message[7])
#define Rx_Data_value4     (p->Message[8])
#define Rx_Data_value5     (p->Message[9])
#define Rx_Data_value6     (p->Message[10])
#define Rx_Data_value7     (p->Message[11])
#define Rx_Control_sum_add (p->Message[12])
#define Rx_Control_sum_xor (p->Message[13])
#define Rx_Stop_byte       (p->Message[14])
#define Tx_Start_byte      (p->Message[0])
#define Tx_Message_counter (p->Message[1])
#define Tx_Adress_slave    (p->Message[2])
#define Tx_Function_code   (p->Message[3])
#define Tx_Data_value0     (p->Message[4])
#define Tx_Data_value1     (p->Message[5])
#define Tx_Data_value2     (p->Message[6])
#define Tx_Data_value3     (p->Message[7])
#define Tx_Data_value4     (p->Message[8])
#define Tx_Data_value5     (p->Message[9])
#define Tx_Data_value6     (p->Message[10])
#define Tx_Data_value7     (p->Message[11])
#define Tx_Control_sum_add (p->Message[12])
#define Tx_Control_sum_xor (p->Message[13])
#define Tx_Stop_byte       (p->Message[14])

void FbNet_init(struct DbNet *p) {
	p->State1 = 0;
	p->Adress_slave = 1;
	p->TimerError_ms = 0;
	return;
}

void FbNet_call(struct DbNet *p) {
	if (p->ErrorCode != 0) {
		p->TimerError_ms = p->TimerError_ms + p->Ts_ms;
		if (p->TimerError_ms >= 10 * 1000) {
			p->TimerError_ms = 0;
			p->ErrorCode = 0;
		}
	}
	return;
}

void FbNet_call_it_rx_end(struct DbNet *p) {
	// Обработка принятого пакета от HMI.
	// Проверка контрольной суммы до дешифрации методом add uint8.
	uint8_t Sum_add = 0;
	Sum_add = (Sum_add + Rx_Message_counter) bitand 0xFF;
	Sum_add = (Sum_add + Rx_Adress_slave) bitand 0xFF;
	Sum_add = (Sum_add + Rx_Function_code) bitand 0xFF;
	Sum_add = (Sum_add + Rx_Data_value0) bitand 0xFF;
	Sum_add = (Sum_add + Rx_Data_value1) bitand 0xFF;
	Sum_add = (Sum_add + Rx_Data_value2) bitand 0xFF;
	Sum_add = (Sum_add + Rx_Data_value3) bitand 0xFF;
	Sum_add = (Sum_add + Rx_Data_value4) bitand 0xFF;
	Sum_add = (Sum_add + Rx_Data_value5) bitand 0xFF;
	Sum_add = (Sum_add + Rx_Data_value6) bitand 0xFF;
	Sum_add = (Sum_add + Rx_Data_value7) bitand 0xFF;
	// Проверка контрольной суммы до дешифрации методом xor uint8.
	uint8_t Sum_xor = 0;
	Sum_xor = (Sum_xor xor Rx_Message_counter) bitand 0xFF;
	Sum_xor = (Sum_xor xor Rx_Adress_slave) bitand 0xFF;
	Sum_xor = (Sum_xor xor Rx_Function_code) bitand 0xFF;
	Sum_xor = (Sum_xor xor Rx_Data_value0) bitand 0xFF;
	Sum_xor = (Sum_xor xor Rx_Data_value1) bitand 0xFF;
	Sum_xor = (Sum_xor xor Rx_Data_value2) bitand 0xFF;
	Sum_xor = (Sum_xor xor Rx_Data_value3) bitand 0xFF;
	Sum_xor = (Sum_xor xor Rx_Data_value4) bitand 0xFF;
	Sum_xor = (Sum_xor xor Rx_Data_value5) bitand 0xFF;
	Sum_xor = (Sum_xor xor Rx_Data_value6) bitand 0xFF;
	Sum_xor = (Sum_xor xor Rx_Data_value7) bitand 0xFF;
	// Дешифровка сообщения.
	Rx_Message_counter = Rx_Message_counter xor KEY_RX_BYTE00;
	Rx_Adress_slave = Rx_Adress_slave xor KEY_RX_BYTE01;
	Rx_Function_code = Rx_Function_code xor KEY_RX_BYTE02;
	Rx_Data_value0 = Rx_Data_value0 xor KEY_RX_BYTE03;
	Rx_Data_value1 = Rx_Data_value1 xor KEY_RX_BYTE04;
	Rx_Data_value2 = Rx_Data_value2 xor KEY_RX_BYTE05;
	Rx_Data_value3 = Rx_Data_value3 xor KEY_RX_BYTE06;
	Rx_Data_value4 = Rx_Data_value4 xor KEY_RX_BYTE07;
	Rx_Data_value5 = Rx_Data_value5 xor KEY_RX_BYTE08;
	Rx_Data_value6 = Rx_Data_value6 xor KEY_RX_BYTE09;
	Rx_Data_value7 = Rx_Data_value7 xor KEY_RX_BYTE10;
	// Проверка сообщения.
	p->ErrorFlag = p->ErrorFlag or (Rx_Start_byte != SD3_PROFIBUS_DP);
	p->ErrorFlag = p->ErrorFlag or (Rx_Function_code != FUN_READ_WRITE_8_BYTES);
	p->ErrorFlag = p->ErrorFlag or (Rx_Control_sum_add != Sum_add);
	p->ErrorFlag = p->ErrorFlag or (Rx_Control_sum_xor != Sum_xor);
	p->ErrorFlag = p->ErrorFlag or (Rx_Stop_byte != ED_PROFIBUS_DP);
	if (p->ErrorFlag) {
		p->ErrorCounter = p->ErrorCounter + 1;
		if (Rx_Start_byte != SD3_PROFIBUS_DP) {
			p->ErrorCode = p->ErrorCode or 0b0000000000000001;
		}
		if (Rx_Function_code != FUN_READ_WRITE_8_BYTES) {
			p->ErrorCode = p->ErrorCode or 0b0000000000000010;
		}
		if (Rx_Control_sum_add != Sum_add) {
			p->ErrorCode = p->ErrorCode or 0b0000000000000100;
		}
		if (Rx_Control_sum_xor != Sum_xor) {
			p->ErrorCode = p->ErrorCode or 0b0000000000001000;
		}
		if (Rx_Stop_byte != ED_PROFIBUS_DP) {
			p->ErrorCode = p->ErrorCode or 0b0000000000010000;
		}
	}
	// Прием данных от HMI.
	if ((not(p->ErrorFlag)) and (Rx_Adress_slave == p->Adress_slave)) {
		p->RegInput[0] = Rx_Data_value0;
		p->RegInput[1] = Rx_Data_value1;
		p->RegInput[2] = Rx_Data_value2;
		p->RegInput[3] = Rx_Data_value3;
		p->RegInput[4] = Rx_Data_value4;
		p->RegInput[5] = Rx_Data_value5;
		p->RegInput[6] = Rx_Data_value6;
		p->RegInput[7] = Rx_Data_value7;
	}
	return;
}

void FbNet_call_it_tx_end(struct DbNet *p) {
	// Формирование ответного сообщения для HMI.
	Tx_Start_byte = SD3_PROFIBUS_DP;
	Tx_Message_counter = Rx_Message_counter;
	Tx_Adress_slave = p->Adress_slave;
	Tx_Function_code = FUN_READ_WRITE_8_BYTES;
	Tx_Data_value0 = p->RegOutput[0];
	Tx_Data_value1 = p->RegOutput[1];
	Tx_Data_value2 = p->RegOutput[2];
	Tx_Data_value3 = p->RegOutput[3];
	Tx_Data_value4 = p->RegOutput[4];
	Tx_Data_value5 = p->RegOutput[5];
	Tx_Data_value6 = p->RegOutput[6];
	Tx_Data_value7 = p->RegOutput[7];
	// Шифрование сообщения.
	Tx_Message_counter = Tx_Message_counter xor KEY_TX_BYTE00;
	Tx_Adress_slave = Tx_Adress_slave xor KEY_TX_BYTE01;
	Tx_Function_code = Tx_Function_code xor KEY_TX_BYTE02;
	Tx_Data_value0 = Tx_Data_value0 xor KEY_TX_BYTE03;
	Tx_Data_value1 = Tx_Data_value1 xor KEY_TX_BYTE04;
	Tx_Data_value2 = Tx_Data_value2 xor KEY_TX_BYTE05;
	Tx_Data_value3 = Tx_Data_value3 xor KEY_TX_BYTE06;
	Tx_Data_value4 = Tx_Data_value4 xor KEY_TX_BYTE07;
	Tx_Data_value5 = Tx_Data_value5 xor KEY_TX_BYTE08;
	Tx_Data_value6 = Tx_Data_value6 xor KEY_TX_BYTE09;
	Tx_Data_value7 = Tx_Data_value7 xor KEY_TX_BYTE10;
	// Проверка контрольной суммы до дешифрации методом add uint8.
	uint8_t Sum_add = 0;
	Sum_add = (Sum_add + Tx_Message_counter) bitand 0xFF;
	Sum_add = (Sum_add + Tx_Adress_slave) bitand 0xFF;
	Sum_add = (Sum_add + Tx_Function_code) bitand 0xFF;
	Sum_add = (Sum_add + Tx_Data_value0) bitand 0xFF;
	Sum_add = (Sum_add + Tx_Data_value1) bitand 0xFF;
	Sum_add = (Sum_add + Tx_Data_value2) bitand 0xFF;
	Sum_add = (Sum_add + Tx_Data_value3) bitand 0xFF;
	Sum_add = (Sum_add + Tx_Data_value4) bitand 0xFF;
	Sum_add = (Sum_add + Tx_Data_value5) bitand 0xFF;
	Sum_add = (Sum_add + Tx_Data_value6) bitand 0xFF;
	Sum_add = (Sum_add + Tx_Data_value7) bitand 0xFF;
	// Проверка контрольной суммы до дешифрации методом xor uint8.
	uint8_t Sum_xor = 0;
	Sum_xor = (Sum_xor xor Tx_Message_counter) bitand 0xFF;
	Sum_xor = (Sum_xor xor Tx_Adress_slave) bitand 0xFF;
	Sum_xor = (Sum_xor xor Tx_Function_code) bitand 0xFF;
	Sum_xor = (Sum_xor xor Tx_Data_value0) bitand 0xFF;
	Sum_xor = (Sum_xor xor Tx_Data_value1) bitand 0xFF;
	Sum_xor = (Sum_xor xor Tx_Data_value2) bitand 0xFF;
	Sum_xor = (Sum_xor xor Tx_Data_value3) bitand 0xFF;
	Sum_xor = (Sum_xor xor Tx_Data_value4) bitand 0xFF;
	Sum_xor = (Sum_xor xor Tx_Data_value5) bitand 0xFF;
	Sum_xor = (Sum_xor xor Tx_Data_value6) bitand 0xFF;
	Sum_xor = (Sum_xor xor Tx_Data_value7) bitand 0xFF;
	Tx_Control_sum_add = Sum_add;
	Tx_Control_sum_xor = Sum_xor;
	Tx_Stop_byte = ED_PROFIBUS_DP;
	return;
}

#endif

#if (0)
struct Message_big_protocol { // Сообщение запрос / ответ:
	uint8_t Start_byte; // Стартовый байт.
	uint8_t Byte_counter; // Количество байт в соообщении.
	uint8_t Message_counter; // Счетчик сообщений.
	uint8_t Adress_slave; // Адрес устройства получателя.
	uint8_t Adress_master; // Адрес устройства отправителя.
	uint8_t Function_code; // Код функции.
	uint8_t Adress_register_read_hi; // Адрес переменной для чтения.
	uint8_t Adress_register_read_lo; // Адрес переменной для чтения.
	uint8_t Counter_byte_register_read; // Количество байт для чтения.
	uint8_t Adress_register_write_hi; // Адрес переменной для записи.
	uint8_t Adress_register_write_lo; // Адрес переменной для записи.
	uint8_t Counter_byte_register_write; // Количество байт для записи.
	uint8_t Data_value[64]; // Данные для приема / передачи.
	uint8_t Control_sum_add; // Контрольная сумма побайтное add как в PROFIBUS_DP.
	uint8_t Control_sum_xor; // Контрольная сумма побайтное xor.
	uint8_t Stop_byte; // Стоповый байт.
};

union Convertor1 {
	struct Message_big_protocol Msg; // Телеграма с именами полей.
	uint8_t Buf[64 + 15]; // Поступ к той же телеграмме по байтам для UART.
};
#endif

// Разъем DB9 RS485 PROFIBUS DP
// DB9 PLC: Мама
// DB9 Кабель: Папа
// PB9.PIN1:
// PB9.PIN2:
// PB9.PIN3: RS485+ P(B) красный
// PB9.PIN4:
// PB9.PIN5: -0GND от PLC
// PB9.PIN6: +5VDC от PLC
// PB9.PIN7:
// PB9.PIN8: RS485- N(A) желтый
// PB9.PIN9:
// PB9.SHIELD: PE(Заземлить)-оплетку кабеля сюда
// Резисторы в разъеме DB9
// Резистор терминальный 220 Ом
// Резистор подтягивающий к +5VDC 390 Ом
// Резистор подтягивающий к -0GND 390 Ом
// Резисторы в PLC DB9
// Между землей и минусом резистор 1 МОм и конденсатор хз зачем.
// Схема PLC с PROFIBUS DP совместимым разъемом DB9 Мама.
//
//  ║                             XS1
//  ║                        ┌───────────┐
//  ║                        │ DB9 RS485 │
//  ║                    PIN1│           │
//  ║                    ────┤           │
//  ║                    PIN2│           │
//  ║                    ────┤           │
//  ║RS485_DB9_RS485P    PIN3│           │
//  ╟────────────────────────┤RS485P+    │
//  ║                    PIN4│           │
//  ║                    ────┤           │
//  ║RS485_DB9_GND       PIN5│           │
//  ╟────────────────────────┤GND        │
//  ║RS485_DB9_5VCC      PIN6│           │
//  ╟────────────────────────┤5VCC       │
//  ║                    PIN7│           │
//  ║                    ────┤           │
//  ║RS485_DB9_RS485N    PIN8│           │
//  ╟────────────────────────┤RS485N-    │
//  ║                    PIN9│           │
//  ║                    ────┤           │
//  ║RS485_DB9_PE      SHIELD│           │
//  ╟────────────────────────┤           │
//  ║                        │           │
//  ║                        └───────────┘
//  ║

// @COPYLEFT ALL WRONGS RESERVED :)
// Author: VA
// Contacts: DIY.PLC.314@gmail.com
// Date of foundation of the library LIB_PLC: 2014
// License: GNU GPL-2.0-or-later
// https://www.gnu.org/licenses/old-licenses/gpl-2.0.html
// https://www.youtube.com/watch?v=n1F_MfLRlX0
// See also:
// https://www.youtube.com/@DIY_PLC
// https://github.com/DIYPLC/LIB_PLC
// https://oshwlab.com/diy.plc.314/PLC_HW1_SW1
// https://3dtoday.ru/3d-models/mechanical-parts/body/korpus-na-din-reiku
// https://t.me/DIY_PLC
