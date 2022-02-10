#include <stdint.h>
#include "FbTimerTof.hpp"

void FbTimerTof::Set_In(uint32_t value)
{
  In = value;
}

void FbTimerTof::Set_TimeDelayOff(uint32_t value)
{
  TimeDelayOff = value;
}

void FbTimerTof::Set_Ts_ms(uint32_t value)
{
  Ts_ms = value;
}

bool FbTimerTof::Get_Out(void)
{
  return Out;
}

void FbTimerTof::Run(void)
{
  if (TimeDelayOff > 0) //Таймер включен.
  {
    if (In) //На входе 1 то на выходе тоже 1.
    {
      Out = true;
      Timer1 = 0;
    }
    else //Если на входе 0.
    {
      if (Out) //На выходе 1.
      {
        if (Timer1 >= TimeDelayOff) //Задержка включения закончилась.
        {
          Out = false;
          Timer1 = 0;
        }
        else //Задержка включения активна.
        {
          Out = true;
          Timer1 = Timer1 + Ts_ms;
        };
      }
      else //На выходе 0.
      {
        Out = false;
        Timer1 = 0;
      }
    }
  }
  else //Таймер выключен.
  {
    Out = In;
    Timer1 = 0;
  }
  return;
}

// Применение.
//
//Блок может применятся как фильтр дребезга контактов и как расширитель импульсов.

// Временная характеристика.
//
//  In
//  ^
//  |     TimeDelayOff          TimeDelayOff
//  |       |<--->|               |<--->|
// 1|   *****     |     ***** *** *     |
//  |   *   *     |     *   * * * *     |
// 0*****---*************---***-*************----> t[s]
//
//  Out
//  ^
//  |     TimeDelayOff          TimeDelayOff
//  |       |<--->|               |<--->|
// 1|   ***********     *****************
//  |   *         *     *               *
// 0*****---------*******---------------*****----> t[s]
