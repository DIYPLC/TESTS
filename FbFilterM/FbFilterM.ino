#include "FbFilterM.h"

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  //       DbFilterM
  //    +-------------+
  //    |  FbFilterM  |
  // ->-|In        Out|->-
  //   -|Size         |
  //   -|Ts_in        |
  //   -|Ts_ms        |
  //   -|Reset        |
  //    +-------------+
  static struct DbFilterM DbFilterM1 = {0};

  DbFilterM1.InputArray[0] = 7.0;
  DbFilterM1.InputArray[1] = 6.0;
  DbFilterM1.InputArray[2] = 5.0;
  DbFilterM1.InputArray[3] = 4.0;
  DbFilterM1.InputArray[4] = 3.0;
  DbFilterM1.InputArray[5] = 2.0;
  DbFilterM1.InputArray[6] = 1.0;
  DbFilterM1.InputArray[7] = 0.0;

  DbFilterM1.TimeArray[0] = 0;
  DbFilterM1.TimeArray[1] = 1;
  DbFilterM1.TimeArray[2] = 2;
  DbFilterM1.TimeArray[3] = 3;
  DbFilterM1.TimeArray[4] = 4;
  DbFilterM1.TimeArray[5] = 5;
  DbFilterM1.TimeArray[6] = 6;
  DbFilterM1.TimeArray[7] = 7;

  DbFilterM1.In    = 0.0          ; //Входной сигнал до фильтрации.
  DbFilterM1.Size  = 4            ; //Размер массива для сортировки (Количество выборок).
  DbFilterM1.Ts_in = 2000         ; //Время между выборками и сортировками для входа In [мс].
  DbFilterM1.Ts_ms = 100          ; //Шаг дискретизации по времени [мс].
  DbFilterM1.Reset = false        ; //Сброс при перезагрузке.

  Serial.println("Start");
  Serial.println("Array not sort:");
  for (uint8_t i = 0; i <= 7; i = i + 1)
  {
    Serial.print("i=");
    Serial.print(i);
    Serial.print("    InputArray=");
    Serial.print(DbFilterM1.InputArray[i]);
    Serial.print("    TimeArray=");
    Serial.print(DbFilterM1.TimeArray[i]);
    Serial.println();
  }

  FbFilterM(&DbFilterM1)          ; //Фильтр медианный.
  //              = DbFilterM1.Out; //Выходной сигнал (середина отсортированного массива).

  Serial.println("Array sort:");
  for (uint8_t i = 0; i <= 7; i = i + 1)
  {
    Serial.print("i=");
    Serial.print(i);
    Serial.print("    InputArray=");
    Serial.print(DbFilterM1.InputArray[i]);
    Serial.print("    TimeArray=");
    Serial.print(DbFilterM1.TimeArray[i]);
    Serial.println();
  }

  Serial.println("Stop");
end: goto end;
  //delay(100000);

}
