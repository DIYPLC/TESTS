//TOF - Таймер с задержкой на выключение.
//        DbTimerTof
//    +----------------+
//    |   FbTimerTof   |
// ->-|In           Out|->-
//   -|TimeDelayOff    |
//   -|Ts_ms           |
//    +----------------+

class FbTimerTof
{
  public:
    void Run(void);
    void Set_In(uint32_t value);
    void Set_TimeDelayOff(uint32_t value);
    void Set_Ts_ms(uint32_t value);
    bool Get_Out(void);
  private:
    //Входные переменные, сохраняемые.
    bool     In           = false; //Вход таймера.
    uint32_t TimeDelayOff = 5000 ; //Время задержки выключения [мс].
    uint32_t Ts_ms        = 100  ; //Шаг дискретизации по времени [мс].
    //Выходные переменные, сохраняемые.
    bool Out              = false; //Выход таймера.
    //Внутренние переменные, сохраняемые.
    uint32_t Timer1       = 0    ; //Текущее время таймер1 [мс].
};
