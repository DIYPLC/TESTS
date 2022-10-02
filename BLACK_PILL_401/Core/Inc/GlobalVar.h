#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

struct GlobalVar
{
  //Системные переменные ПЛК
  uint32_t Uptime_ms; //Время работы [мс].
  uint32_t Ts_ms    ; //Время предидущего скана [мс].
  uint32_t Ts_max_ms; //Максимальное время скана [мс].
  float    Ts       ; //Время предидущего скана [с].
  bool     Reset    ; //Флаг первого скана.
  uint32_t ErrorCode; //Код ошибки.
  uint16_t MW[64]   ; //MODBUS HOLDING REGISTERS



};

#ifdef __cplusplus
}
#endif

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
// License: GNU GPL v2
