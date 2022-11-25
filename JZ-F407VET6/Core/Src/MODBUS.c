//MODBUS ASCII SLAVE RS485
//MODBUS ADDRESS 1
//9600[Bit/s] 8N1
//UART TO RS485 ADM2486BRVZ
//PIN A10 STM32 RX
//PIN A9  STM32 TX
//PIN A8  STM32 TX_ON
#include <stdint.h>
#include <stdbool.h>
#include <iso646.h>

#include "GlobalVar.h"
#include "MODBUS.h"

#define SLAVE_ADDRESS 1
#define READ_HOLDING_REGISTERS 3

uint8_t TwoASCIItoByte(uint8_t LoASCII, uint8_t HiASCII);
uint8_t Lo4BitInByteToASCII(uint8_t InputByte);
uint8_t Hi4BitInByteToASCII(uint8_t InputByte);
uint8_t uint16_to_uint8_lo(uint16_t InputWord);
uint8_t uint16_to_uint8_hi(uint16_t InputWord);

void Fb_MODBUS_ASCII_SLAVE_ADU_TO_PDU(struct DbMODBUS *p) {
	;
	uint16_t SIZE_PDU = p->SizeMessage;
	uint8_t RX_MODBUS_ADDRESS = TwoASCIItoByte(p->Message[2], p->Message[1]);
	p->Message[0] = TwoASCIItoByte(p->Message[2], p->Message[1]);
	return;
}

void Fb_MODBUS_SLAVE_PROCESSING_PDU(struct DbMODBUS *p, struct GlobalVar *ptr) {
	uint8_t Function_code = READ_HOLDING_REGISTERS;
	uint16_t MODBUS_start_address = 0;
	uint16_t MODBUS_register_count = 1; //1 Holding_register
	uint8_t Byte_count = MODBUS_register_count * 2;
	uint16_t SIZE_PDU = Byte_count + 2; //4 байт в буффере
	p->SizeMessage = SIZE_PDU; //4 байт в буффере
	p->Message[0] = Function_code;
	p->Message[1] = Byte_count;
	uint16_t MODBUS_register_value = ptr->MW[MODBUS_start_address];
	p->Message[2] = uint16_to_uint8_hi(MODBUS_register_value); //DATA
	p->Message[3] = uint16_to_uint8_lo(MODBUS_register_value); //DATA
	MODBUS_register_value = ptr->MW[MODBUS_start_address + 1];
	p->Message[4] = uint16_to_uint8_hi(MODBUS_register_value); //DATA
	p->Message[5] = uint16_to_uint8_lo(MODBUS_register_value); //DATA
	return;
}

uint8_t Calc_LCR(struct DbMODBUS *p) {
	uint16_t i = 0;
	//Рассчет ответной контрольной суммы LCR.
	uint8_t SUM = SLAVE_ADDRESS;
	//Cумма 0...SizeMessage - 1 = 0...3
	for (i = 0; (i <= (p->SizeMessage - 1)); i = i + 1) {
		SUM = (0xFF & (SUM + p->Message[i]));
	}
	//Инверсия, инкремент.
	uint8_t LCR = ((~SUM) + 1) & 0xFF;
	return LCR;
}

void Fb_MODBUS_ASCII_SLAVE_PDU_TO_ADU(struct DbMODBUS *p) {
	uint8_t MODBUS_ASCII_LCR = Calc_LCR(p);

//Message = ADU MODBUS ASCII Tx
//Размер ASCII ADU в байтах. [0...SizeMessage - 1]
	uint16_t SIZE_PDU = p->SizeMessage;
	uint16_t SIZE_ADU = SIZE_PDU * 2 + 7; //15 байт в буффере;
	p->SizeMessage = SIZE_ADU; //15 байт в буффере
//Стоповые байты
	p->Message[SIZE_ADU - 1] = 0x0A; //[14]LF
	p->Message[SIZE_ADU - 2] = 0x0D; //[13]CR
//LCR
	p->Message[SIZE_ADU - 3] = Lo4BitInByteToASCII(MODBUS_ASCII_LCR); //[12]
	p->Message[SIZE_ADU - 4] = Hi4BitInByteToASCII(MODBUS_ASCII_LCR); //[11]
//PDU -> ASCII ADU
//p->Message[10] = Lo4BitInByteToASCII(p->Message[3]); //ASCII PDU
//p->Message[ 9] = Hi4BitInByteToASCII(p->Message[3]); //ASCII PDU
//p->Message[ 8] = Lo4BitInByteToASCII(p->Message[2]); //ASCII PDU
//p->Message[ 7] = Hi4BitInByteToASCII(p->Message[2]); //ASCII PDU
//p->Message[ 6] = Lo4BitInByteToASCII(p->Message[1]); //ASCII PDU
//p->Message[ 5] = Hi4BitInByteToASCII(p->Message[1]); //ASCII PDU
//p->Message[ 4] = Lo4BitInByteToASCII(p->Message[0]); //ASCII PDU
//p->Message[ 3] = Hi4BitInByteToASCII(p->Message[0]); //ASCII PDU
	uint16_t i = SIZE_PDU; //i=4
	while (i != 0) //i = 3,2,1,0
	{
		i = i - 1;
		p->Message[(i * 2 + 4)] = Lo4BitInByteToASCII(p->Message[i]); //ASCII PDU
		p->Message[(i * 2 + 3)] = Hi4BitInByteToASCII(p->Message[i]); //ASCII PDU;
	}
//MODBUS SLAVE ADDRESS
	p->Message[2] = Lo4BitInByteToASCII(SLAVE_ADDRESS);
	p->Message[1] = Hi4BitInByteToASCII(SLAVE_ADDRESS);
//добавляем в начало стартовый байт
	p->Message[0] = 0x3A; //: START
	return;
}

uint8_t uint16_to_uint8_lo(uint16_t InputWord) {
	return ((uint8_t) (InputWord & 0xFF));
}

uint8_t uint16_to_uint8_hi(uint16_t InputWord) {
	return ((uint8_t) ((InputWord & 0xFF00) >> 8));
}

uint8_t TwoASCIItoByte(uint8_t LoASCII, uint8_t HiASCII) {
	uint8_t Lo4Bit = 0; //Младшие 4бита.
	uint8_t Hi4Bit = 0; //Старшие 4бита.
	switch (LoASCII) {
	case 0x30:
		Lo4Bit = 0x00;
		break;
	case 0x31:
		Lo4Bit = 0x01;
		break;
	case 0x32:
		Lo4Bit = 0x02;
		break;
	case 0x33:
		Lo4Bit = 0x03;
		break;
	case 0x34:
		Lo4Bit = 0x04;
		break;
	case 0x35:
		Lo4Bit = 0x05;
		break;
	case 0x36:
		Lo4Bit = 0x06;
		break;
	case 0x37:
		Lo4Bit = 0x07;
		break;
	case 0x38:
		Lo4Bit = 0x08;
		break;
	case 0x39:
		Lo4Bit = 0x09;
		break;
	case 0x41:
		Lo4Bit = 0x0A;
		break;
	case 0x42:
		Lo4Bit = 0x0B;
		break;
	case 0x43:
		Lo4Bit = 0x0C;
		break;
	case 0x44:
		Lo4Bit = 0x0D;
		break;
	case 0x45:
		Lo4Bit = 0x0E;
		break;
	case 0x46:
		Lo4Bit = 0x0F;
		break;
	default:
		Lo4Bit = 0x00;
		break;
	}
	switch (HiASCII) {
	case 0x30:
		Hi4Bit = 0x00;
		break;
	case 0x31:
		Hi4Bit = 0x10;
		break;
	case 0x32:
		Hi4Bit = 0x20;
		break;
	case 0x33:
		Hi4Bit = 0x30;
		break;
	case 0x34:
		Hi4Bit = 0x40;
		break;
	case 0x35:
		Hi4Bit = 0x50;
		break;
	case 0x36:
		Hi4Bit = 0x60;
		break;
	case 0x37:
		Hi4Bit = 0x70;
		break;
	case 0x38:
		Hi4Bit = 0x80;
		break;
	case 0x39:
		Hi4Bit = 0x90;
		break;
	case 0x41:
		Hi4Bit = 0xA0;
		break;
	case 0x42:
		Hi4Bit = 0xB0;
		break;
	case 0x43:
		Hi4Bit = 0xC0;
		break;
	case 0x44:
		Hi4Bit = 0xD0;
		break;
	case 0x45:
		Hi4Bit = 0xE0;
		break;
	case 0x46:
		Hi4Bit = 0xF0;
		break;
	default:
		Hi4Bit = 0x00;
		break;
	}
	return (Lo4Bit | Hi4Bit);
}

uint8_t Lo4BitInByteToASCII(uint8_t InputByte) {
	uint8_t result;
	switch (InputByte & 0x0F) {
	case 0x00:
		result = 0x30;
		break;
	case 0x01:
		result = 0x31;
		break;
	case 0x02:
		result = 0x32;
		break;
	case 0x03:
		result = 0x33;
		break;
	case 0x04:
		result = 0x34;
		break;
	case 0x05:
		result = 0x35;
		break;
	case 0x06:
		result = 0x36;
		break;
	case 0x07:
		result = 0x37;
		break;
	case 0x08:
		result = 0x38;
		break;
	case 0x09:
		result = 0x39;
		break;
	case 0x0A:
		result = 0x41;
		break;
	case 0x0B:
		result = 0x42;
		break;
	case 0x0C:
		result = 0x43;
		break;
	case 0x0D:
		result = 0x44;
		break;
	case 0x0E:
		result = 0x45;
		break;
	case 0x0F:
		result = 0x46;
		break;
	default:
		result = 0x00;
		break;
	}
	return result;
}

uint8_t Hi4BitInByteToASCII(uint8_t InputByte) {
	uint8_t result;
	switch (InputByte & 0xF0) {
	case 0x00:
		result = 0x30;
		break;
	case 0x10:
		result = 0x31;
		break;
	case 0x20:
		result = 0x32;
		break;
	case 0x30:
		result = 0x33;
		break;
	case 0x40:
		result = 0x34;
		break;
	case 0x50:
		result = 0x35;
		break;
	case 0x60:
		result = 0x36;
		break;
	case 0x70:
		result = 0x37;
		break;
	case 0x80:
		result = 0x38;
		break;
	case 0x90:
		result = 0x39;
		break;
	case 0xA0:
		result = 0x41;
		break;
	case 0xB0:
		result = 0x42;
		break;
	case 0xC0:
		result = 0x43;
		break;
	case 0xD0:
		result = 0x44;
		break;
	case 0xE0:
		result = 0x45;
		break;
	case 0xF0:
		result = 0x46;
		break;
	default:
		result = 0x00;
		break;
	}
	return result;
}
