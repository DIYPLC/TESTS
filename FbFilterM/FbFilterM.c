#include <stdint.h>
#include <stdbool.h>
#include <iso646.h>
#include "FbFilterM.h"

#define In         p->In
#define Size       p->Size
#define Ts_in      p->Ts_in
#define Ts_ms      p->Ts_ms
#define Reset      p->Reset
#define Out        p->Out
#define InputArray p->InputArray
#define TimeArray  p->TimeArray

void FbFilterM(struct DbFilterM *p)
{
  /*
    //Пузырьковая сотрировка бессмысленная и беспощадная.
    //https://ru.wikibooks.org/wiki/%D0%AF%D0%B7%D1%8B%D0%BA_%D0%A1%D0%B8_%D0%B2_%D0%BF%D1%80%D0%B8%D0%BC%D0%B5%D1%80%D0%B0%D1%85/%D0%A1%D0%BE%D1%80%D1%82%D0%B8%D1%80%D0%BE%D0%B2%D0%BA%D0%B0
    for (uint16_t i = 0 ; i < Size - 1; i = i + 1)
    { //Сравниваем два соседних элемента.
      for (uint16_t j = 0 ; j < Size - i - 1 ; j = j + 1)
      {
        if (InputArray[j] > InputArray[j + 1])
        { //Если они идут в неправильном порядке, то меняем их местами.
          float TempValue = InputArray[j];
          InputArray[j] = InputArray[j + 1] ;
          InputArray[j + 1] = TempValue;
          //Сортируем второй массив в соответствии с первым.
          uint16_t TempTime = TimeArray[j];
          TimeArray[j] = TimeArray[j + 1] ;
          TimeArray[j + 1] = TempTime;
        }
      }
    }
  */
  uint16_t i = 0;
  while (i < Size - 1)
  {
    //Сравниваем два соседних элемента.
    uint16_t j = 0;
    while (j < Size - i - 1)
    {
      if (InputArray[j] > InputArray[j + 1])
      { //Если они идут в неправильном порядке, то меняем их местами.
        float TempValue = InputArray[j];
        InputArray[j] = InputArray[j + 1] ;
        InputArray[j + 1] = TempValue;
        //Сортируем второй массив в соответствии с первым.
        uint16_t TempTime = TimeArray[j];
        TimeArray[j] = TimeArray[j + 1] ;
        TimeArray[j + 1] = TempTime;
      }
      j = j + 1;
    }
    i = i + 1;
  }

  return;
}

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
