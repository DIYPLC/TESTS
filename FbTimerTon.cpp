#include <stdint.h>
#include "FbTimerTon.hpp"

void FbTimerTon::Set_In(uint32_t value)
{
  In = value;
}

void FbTimerTon::Set_TimeDelayOn(uint32_t value)
{
  TimeDelayOn = value;
}

void FbTimerTon::Set_Ts_ms(uint32_t value)
{
  Ts_ms = value;
}

bool FbTimerTon::Get_Out(void)
{
  return Out;
}

void FbTimerTon::Run(void)
{
  if (TimeDelayOn > 0) //Таймер включен.
  {
    if (In) //На входе 1.
    {
      if (Out) //На выходе 1.
      {
        Out = true;
        Timer1 = 0;
      }
      else //На выходе 0.
      {
        if (Timer1 >= TimeDelayOn) //Задержка включения закончилась.
        {
          Out = true;
          Timer1 = 0;
        }
        else //Задержка включения активна.
        {
          Out = false;
          Timer1 = Timer1 + Ts_ms;
        }
      }
    }
    else //Если на входе 0 то на выходе тоже 0.
    {
      Out = false;
      Timer1 = 0;
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
// При поступлении входного сигнала выходной сигнал появляется не сразу, а с задержкой TimeDelayOn.
// Блок может применятся как фильтр дребезга контактов дискретных входов.
// Совместно с инвертором может применяться для генерации импульсов.

// Временная характеристика.
//
//  In
//  ^
//  | TimeDelayOn           TimeDelayOn
//  |   |<--->|               |<--->|
// 1|   ***********     * *   ***********
//  |   *         *     * *   *         *
// 0*****---------*************---------*****----> t[s]
//
//  Out
//  ^
//  | TimeDelayOn           TimeDelayOn
//  |   |<--->|               |<--->|
// 1|   |     *****           |     *****
//  |   |     *   *           |     *   *
// 0***********---*******************---*****----> t[s]
