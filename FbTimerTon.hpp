//TON - Таймер с задержкой на включение.
//        DbTimerTon
//    +----------------+
//    |   FbTimerTon   |
// ->-|In           Out|->-
//   -|TimeDelayOn     |
//   -|Ts_ms           |
//    +----------------+

class FbTimerTon
{
  public:
    void Run(void);
    void Set_In(uint32_t value);
    void Set_TimeDelayOn(uint32_t value);
    void Set_Ts_ms(uint32_t value);
    bool Get_Out(void);
  private:
    //Входные переменные, сохраняемые.
    bool     In          = false; //Вход таймера.
    uint32_t TimeDelayOn = 5000 ; //Время задержки включения [мс].
    uint32_t Ts_ms       = 100  ; //Шаг дискретизации по времени [мс].
    //Выходные переменные, сохраняемые.
    bool Out             = false; //Выход таймера.
    //Внутренние переменные, сохраняемые.
    uint32_t Timer1      = 0    ; //Текущее время таймер1 [мс].
};
