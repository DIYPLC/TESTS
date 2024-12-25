// Черновик протокола связи PLC + HMI.
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#define NET_PLC_EN (1)
#if (NET_PLC_EN > 0)

#define SIZE_INPUT_DATA_BYTES (8) // Размер данных для записи в ПЛК
#define SIZE_OUTPUT_DATA_BYTES (10) // Размер данных для чтения из ПЛК (обычно их больше)
#define SIZE_MSG_BYTES (SIZE_OUTPUT_DATA_BYTES+7)
struct DbNet {
	// Входные переменные, сохраняемые.
	uint8_t RegInput[SIZE_INPUT_DATA_BYTES];
	uint8_t Adress_slave;
	uint32_t Timeout_ms;
	uint32_t Ts_ms; //Шаг дискретизации по времени [мс].
	bool Reset; //Сброс при перезагрузке.
	// Выходные переменные, сохраняемые.
	uint8_t RegOutput[SIZE_OUTPUT_DATA_BYTES];
	uint16_t ErrorCode;
	uint32_t ErrorCounter;
	bool ErrorFlag;
	// Входные / выходные переменные, сохраняемые.
	uint8_t Convertor_start_byte;
	uint8_t Convertor[8];
	uint16_t Convertor_ui16;
	uint32_t Convertor_ui32;
	uint64_t Convertor_ui64;
	int16_t Convertor_i16;
	int32_t Convertor_i32;
	int64_t Convertor_i64;
	float Convertor_f32;
	double Convertor_f64;
	// Внутренние переменные, сохраняемые.
	uint8_t Message[SIZE_MSG_BYTES]; // Телеграмме по байтам.
	uint32_t TimerError_ms;
	uint32_t Timer1; //Внутренний таймер [мс].
	uint8_t State1; //Внутренний граф состояния.
};

void FbNet_init(struct DbNet *p); // setup()
void FbNet_call(struct DbNet *p); // loop()
void FbNet_call_it_rx_end(struct DbNet *p); // HAL_UART_Receive( &huart3, &DbNet1.Message, (SIZE_INPUT_DATA_BYTES+7), 1000);
void FbNet_call_it_tx_end(struct DbNet *p); // HAL_UART_Transmit( &huart3, &DbNet1.Message, (SIZE_OUTPUT_DATA_BYTES+7), 1000);

// Конвертор массива байт в необходимый тип данных и обратно.
void FbNet_ui8_x2_to_ui16(struct DbNet *p);
void FbNet_ui8_x4_to_ui32(struct DbNet *p);
void FbNet_ui8_x8_to_ui64(struct DbNet *p);
void FbNet_ui16_to_ui8_x2(struct DbNet *p);
void FbNet_ui32_to_ui8_x4(struct DbNet *p);
void FbNet_ui64_to_ui8_x8(struct DbNet *p);

void FbNet_ui8_x2_to_i16(struct DbNet *p);
void FbNet_ui8_x4_to_i32(struct DbNet *p);
void FbNet_ui8_x8_to_i64(struct DbNet *p);
void FbNet_i16_to_ui8_x2(struct DbNet *p);
void FbNet_i32_to_ui8_x4(struct DbNet *p);
void FbNet_i64_to_ui8_x8(struct DbNet *p);

void FbNet_ui8_x4_to_f32(struct DbNet *p);
void FbNet_ui8_x8_to_f64(struct DbNet *p);
void FbNet_f32_to_uint8_x4(struct DbNet *p);
void FbNet_f64_to_uint8_x8(struct DbNet *p);

#endif

#ifdef __cplusplus
}
#endif

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
