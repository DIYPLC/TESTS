//MODBUS RTU SLAVE
//FLProg + Arduino Nano

#define NUMBER_OF_REGISTERS 8 //Количество регистров.
#define MODBUS_SLAVE_ADR 1
#define MW _modbusSlaveDataTable_4

int16_t _modbusSlaveDataTable_4[NUMBER_OF_REGISTERS] = {0}; //Значения в регистрах.
int16_t _modbusSlaveAddresTable_4[NUMBER_OF_REGISTERS] = {0, 1, 2, 3, 4, 5, 6, 7}; //Адрес регистров.
byte _modbusSlaveBufferSize = 0;
byte _modbusSlaveLastRec = 0;
long _modbusSlaveTime;
byte _modbusSlaveBuffer[64];
const unsigned char _modbusSlave_fctsupported[] = {3, 6, 16};
int16_t _tempVariable_int;

void setup()
{
  Serial.begin(9600, SERIAL_8N1);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  MW[0] = -2;
  MW[1] = 6;
}

void loop()
{
  _modbusSlavePoll();
  //Плата:1
  MW[2] = MW[0] + MW[1];
  MW[3] = millis();
  
}

bool _isTimer(unsigned long startTime, unsigned long period)
{
  unsigned long currentTime;
  currentTime = millis();
  if (currentTime >= startTime)
  {
    return (currentTime >= (startTime + period));
  }
  else
  {
    return (currentTime >= (4294967295 - startTime + period));
  }
}

int modbusCalcCRC(byte length, byte bufferArray[])
{
  unsigned int temp, temp2, flag;
  temp = 0xFFFF;
  for (unsigned char i = 0; i < length; i++)
  {
    temp = temp ^ bufferArray[i];
    for (unsigned char j = 1; j <= 8; j++)
    {
      flag = temp & 0x0001;
      temp >>= 1;
      if (flag)   temp ^= 0xA001;
    }
  }
  temp2 = temp >> 8;
  temp = (temp << 8) | temp2;
  temp &= 0xFFFF;
  return temp;
}

byte _modbusSlavePoll()
{
  byte  avalibleBytes = Serial.available();
  if (avalibleBytes == 0) return 0;
  if (avalibleBytes != _modbusSlaveLastRec)
  {
    _modbusSlaveLastRec = avalibleBytes;
    _modbusSlaveTime = millis();
    return 0;
  }
  if (!(_isTimer(_modbusSlaveTime, 5))) return 0;
  _modbusSlaveLastRec = 0;
  byte state = _modbusGetSlaveRxBuffer();
  if (state < 7)
  {
    return state;
  }
  if ((_modbusSlaveBuffer[0] != MODBUS_SLAVE_ADR) and (_modbusSlaveBuffer[0] != 0)) return 0;
  byte exception =  _modbusValidateRequest();
  if (exception > 0)
  {
    if (exception != 255)
    {
      _modbusSlaveBuildException(exception);
      _modbusSlaveSendTxBuffer();
    }
    return exception;
  }
  switch (_modbusSlaveBuffer[1])
  {
    case 3 :
      return process_modbus_FC3(4);
      break;
    case 6 :
      return process_modbus_FC6();
      break;
    case 16 :
      return process_modbus_FC16();
      break;
    default:
      break;
  }
  return 25;
}

byte _modbusValidateRequest()
{
  int msgCRC =
    ((_modbusSlaveBuffer[_modbusSlaveBufferSize - 2] << 8)     | _modbusSlaveBuffer[_modbusSlaveBufferSize - 1]);
  if (modbusCalcCRC(_modbusSlaveBufferSize - 2, _modbusSlaveBuffer) != msgCRC)
  {
    return 255;
  }
  boolean isSupported = false;
  for (uint8_t i = 0; i < sizeof(_modbusSlave_fctsupported); i++)
  {
    if (_modbusSlave_fctsupported[i] == _modbusSlaveBuffer[1])
    {
      isSupported = 1;
      break;
    }
  }
  if (!isSupported)
  {
    return 1;
  }
  int intRegs = 0;
  byte byteRegs;
  switch (_modbusSlaveBuffer[1])
  {
    case 6 :
      if (!(checkModbusAddres((word(_modbusSlaveBuffer[2], _modbusSlaveBuffer[3])), 4)))
      {
        return 2;
      }
      break;
    case 3 :
    case 16 :
      if (!(checkModbusRange((word(_modbusSlaveBuffer[2], _modbusSlaveBuffer[3])), (word(_modbusSlaveBuffer[4], _modbusSlaveBuffer[5])), 4)))
      {
        return 2;
      }
      break;
  }
  return 0; // OK, no exception code thrown
}
bool checkModbusAddres(int addr, byte table)
{
  return (!((modbusSlaveIndexForAddres(addr, table)) == -1));
}

int modbusSlaveIndexForAddres(int addr, byte table)
{
  int tableSize = 0;
  switch (table)
  {
    case 4:
      tableSize = NUMBER_OF_REGISTERS;
      break;
  }
  for (byte i = 0; i < tableSize; i++)
  {
    if ((modbusSlaveAddresFromIndex(i, table)) == addr)
    {
      return  i;
    }
  }
  return -1;
}

int modbusSlaveAddresFromIndex(byte index, byte table)
{
  switch (table)
  {
    case 4:
      return _modbusSlaveAddresTable_4[index];
      break;
  }
  return -1;
}

bool checkModbusRange(int startAddr, int addrNumber, byte table)
{
  for (int i = 0; i < addrNumber; i++)
  {
    if (!(checkModbusAddres((startAddr + i), table)))
    {
      return false;
    }
  }
  return true;
}

void  _modbusSlaveBuildException(byte exception)
{
  byte func = _modbusSlaveBuffer[1];
  _modbusSlaveBuffer[0] = MODBUS_SLAVE_ADR;
  _modbusSlaveBuffer[1] = func + 0x80;
  _modbusSlaveBuffer[ 2 ] = exception;
  _modbusSlaveBufferSize = 3;
}

void _modbusSlaveSendTxBuffer()
{
  if (_modbusSlaveBuffer[0] == 0)
  {
    return;
  }
  byte i = 0;
  int crc = modbusCalcCRC(_modbusSlaveBufferSize, _modbusSlaveBuffer);
  _modbusSlaveBuffer[ _modbusSlaveBufferSize ] = crc >> 8;
  _modbusSlaveBufferSize++;
  _modbusSlaveBuffer[ _modbusSlaveBufferSize ] = crc & 0x00ff;
  _modbusSlaveBufferSize++;
  UCSR0A = UCSR0A | (1 << TXC0);
  digitalWrite(13, HIGH);
  delay (5);
  Serial.write(_modbusSlaveBuffer, _modbusSlaveBufferSize);
  while (!(UCSR0A & (1 << TXC0)));
  delay (5);
  digitalWrite(13, LOW);
  Serial.flush();
  _modbusSlaveBufferSize = 0;
}

byte _modbusGetSlaveRxBuffer()
{
  boolean bBuffOverflow = false;
  digitalWrite(13, LOW);
  _modbusSlaveBufferSize = 0;
  while (Serial.available())
  {
    _modbusSlaveBuffer[ _modbusSlaveBufferSize ] = Serial.read();
    _modbusSlaveBufferSize ++;
    if (_modbusSlaveBufferSize >= 64) bBuffOverflow = true;
  }
  if (bBuffOverflow)
  {
    return -3;
  }
  return _modbusSlaveBufferSize;
}

byte process_modbus_FC3(byte table)
{
  int startAddr = word(_modbusSlaveBuffer[2], _modbusSlaveBuffer[3]);
  int byteRegsno = word(_modbusSlaveBuffer[4], _modbusSlaveBuffer[5]);
  int i;
  int value;
  byte index;
  _modbusSlaveBuffer[ 2 ]  = byteRegsno * 2;
  _modbusSlaveBufferSize = 3;
  for (i = startAddr; i < startAddr + byteRegsno; i++)
  {
    index = modbusSlaveIndexForAddres(i, table);
    if (table == 4)
    {
      value = _modbusSlaveDataTable_4[index];
    }
    _modbusSlaveBuffer[ _modbusSlaveBufferSize ] = highByte(value);
    _modbusSlaveBufferSize++;
    _modbusSlaveBuffer[ _modbusSlaveBufferSize ] = lowByte(value);
    _modbusSlaveBufferSize++;
  }
  _modbusSlaveSendTxBuffer();
  return _modbusSlaveBufferSize + 2;
}

byte process_modbus_FC6()
{
  int addres = word(_modbusSlaveBuffer[2], _modbusSlaveBuffer[3]);
  int index;
  index = modbusSlaveIndexForAddres(addres, 4);
  _modbusSlaveDataTable_4[index] = word(_modbusSlaveBuffer[4], _modbusSlaveBuffer[5]);
  _modbusSlaveBufferSize = 6;
  _modbusSlaveSendTxBuffer();
  return _modbusSlaveBufferSize + 2;
}

byte process_modbus_FC16()
{
  byte func = _modbusSlaveBuffer[1];
  int startAddr = _modbusSlaveBuffer[2] << 8 | _modbusSlaveBuffer[3];
  int byteRegsno = _modbusSlaveBuffer[4] << 8 | _modbusSlaveBuffer[5];
  int i;
  int index;
  _modbusSlaveBuffer[4]   = 0;
  _modbusSlaveBuffer[5]   = byteRegsno;
  _modbusSlaveBufferSize = 6;
  for (i = 0; i < byteRegsno; i++)
  {
    index = modbusSlaveIndexForAddres((startAddr + i), 4);
    _modbusSlaveDataTable_4[index] = word(_modbusSlaveBuffer[ 7 + i * 2 ],  _modbusSlaveBuffer[8 + i * 2 ]);
  }
  _modbusSlaveSendTxBuffer();
  return _modbusSlaveBufferSize + 2;
}
