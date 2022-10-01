//MODBUS ASCII SLAVE RS485
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct DbMODBUS
{
  uint8_t  Message[32];
  uint16_t SizeMessage;
  uint8_t  State;
};

enum MODBUS_STATES
{
  MODBUS_STATE_RESET = 0,
  MODBUS_STATE_RX_ADU = 1,
  MODBUS_STATE_ADU_TO_PDU = 2,
  MODBUS_STATE_PROCESSING_PDU = 3,
  MODBUS_STATE_PDU_TO_ADU = 4,
  MODBUS_STATE_TX_ADU = 5
};

void Fb_MODBUS_ASCII_SLAVE_ADU_TO_PDU(struct DbMODBUS *p);
void Fb_MODBUS_SLAVE_PROCESSING_PDU(struct DbMODBUS *p, struct GlobalVar *ptr);
void Fb_MODBUS_ASCII_SLAVE_PDU_TO_ADU(struct DbMODBUS *p);

#ifdef __cplusplus
}
#endif
