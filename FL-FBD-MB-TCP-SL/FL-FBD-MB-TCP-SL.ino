//MODBUS TCP SLAVE
//FLProg + Arduino Uno + W5100
//Ненадежно сбоит если передернуть сетевой кабель.
#include <Ethernet.h>
#include <SPI.h>
int _modbusSlaveDataTable_4[3];
int _modbusSlaveAddresTable_4[3] = {0, 1, 2};
byte _modbusSlaveBufferSize = 0;
byte _modbusSlaveBuffer[64];
const unsigned char _modbusSlave_fctsupported[] = {3, 6, 16};
EthernetServer _modbusSlaveTCPServer(502);
EthernetClient _modbusSlaveTCPClient;
byte _modbusSlaveMBAPBuffer[6];
byte ethernet_mac [] = {0x78, 0xAC, 0xC0, 0x87, 0x42, 0xC0};
IPAddress ethernet_ip(192, 168, 13, 55);
byte ethernet_dns [] = {192, 168, 13, 1};
byte ethernet_gateway [] = {192, 168, 13, 1};
byte ethernet_subnet [] = {255, 255, 255, 0};
int _tempVariable_int;
void setup()
{
    Ethernet.begin(ethernet_mac, ethernet_ip, ethernet_dns, ethernet_gateway, ethernet_subnet);
    delay(1000);
    _modbusSlaveTCPServer.begin();
}
void loop()
{
    _modbusSlavePoll();
    //Плата:1
    _tempVariable_int = ((_modbusSlaveDataTable_4[0]))+((_modbusSlaveDataTable_4[1]));
    _modbusSlaveDataTable_4[2] = _tempVariable_int;
}
bool _isTimer(unsigned long startTime, unsigned long period)
{
    unsigned long currentTime;
    currentTime = millis();
    if (currentTime>= startTime) 
    {
        return (currentTime>=(startTime + period));
    }
     else 
    {
        return (currentTime >=(4294967295-startTime+period));
    }
}
byte _modbusSlavePoll()
{
    if(!_modbusSlaveTCPClient.connected()) 
    {
        _modbusSlaveTCPClient = _modbusSlaveTCPServer.available();
    }
    if (_modbusSlaveTCPClient) 
    {
         _modbusGetSlaveRxBuffer();
    }
     else
    {
        return 0;
    }
    if(_modbusSlaveBufferSize ==0) 
    {
        return 0;
    }
    if ((_modbusSlaveBuffer[0] != 1) && (_modbusSlaveBuffer[0] != 0)) return 0;
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
        if(!(checkModbusAddres((word(_modbusSlaveBuffer[2], _modbusSlaveBuffer[3])),4)))
        {
            return 2;
        }
        break;
        case 3 :
        case 16 :
        if(!(checkModbusRange((word(_modbusSlaveBuffer[2], _modbusSlaveBuffer[3])), (word(_modbusSlaveBuffer[4], _modbusSlaveBuffer[5])),4)))
        {
            return 2;
        }
        break;
    }
    return 0; // OK, no exception code thrown
}
bool checkModbusAddres(int addr, byte table)
{
    return (!((modbusSlaveIndexForAddres(addr,table)) == -1));
}
int modbusSlaveIndexForAddres(int addr, byte table)
{
    int tableSize = 0;
    switch (table) 
    {
        case 4:
        tableSize = 3;
        break;
    }
    for (byte i = 0; i < tableSize; i++) 
    {
        if((modbusSlaveAddresFromIndex(i,table)) == addr)
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
    for (int i=0; i < addrNumber; i++) 
    {
        if(!(checkModbusAddres((startAddr+i),table)))
        {
            return false;
        }
    }
    return true;
}
void  _modbusSlaveBuildException(byte exception) 
{
    byte func = _modbusSlaveBuffer[1];
    _modbusSlaveBuffer[0] = 1;
    _modbusSlaveBuffer[1] = func + 0x80;
    _modbusSlaveBuffer[ 2 ] = exception;
    _modbusSlaveBufferSize = 3;
}
void _modbusSlaveSendTxBuffer()
{
    if(_modbusSlaveBuffer[0] == 0) 
    {
        _modbusSlaveTCPClient.stop();
        return;
    }
    String stringBuffer = "";
    _modbusSlaveMBAPBuffer[4] = highByte(_modbusSlaveBufferSize);
    _modbusSlaveMBAPBuffer[5] = lowByte(_modbusSlaveBufferSize);
    for (int i=0; i <6; i++)
    {
        stringBuffer.concat(char(_modbusSlaveMBAPBuffer[i]));
    }
    for (int i=0; i <_modbusSlaveBufferSize; i++)
    {
        stringBuffer.concat(char(_modbusSlaveBuffer[i]));
    }
    _modbusSlaveTCPClient.print(stringBuffer);
    _modbusSlaveBufferSize = 0;
}
byte _modbusGetSlaveRxBuffer()
{
    byte currentByte = 0;
    boolean bBuffOverflow = false;
    byte currentByteIndex = 0;
    _modbusSlaveBufferSize = 0;
    while (_modbusSlaveTCPClient.available()) 
    {
        currentByte = _modbusSlaveTCPClient.read();
        if (currentByteIndex < 6)
        	
        {
            _modbusSlaveMBAPBuffer[currentByteIndex] = currentByte;
        }
        else
        {
            _modbusSlaveBuffer[ _modbusSlaveBufferSize ] = currentByte;
            _modbusSlaveBufferSize ++;
            if (_modbusSlaveBufferSize >= 64) bBuffOverflow = true;
        }
        	currentByteIndex++;
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
    _modbusSlaveDataTable_4[index] =word(_modbusSlaveBuffer[4], _modbusSlaveBuffer[5]);
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
        index = modbusSlaveIndexForAddres((startAddr+i), 4);
        _modbusSlaveDataTable_4[index] =word(_modbusSlaveBuffer[ 7 + i * 2 ],  _modbusSlaveBuffer[8 + i * 2 ]);
    }
    _modbusSlaveSendTxBuffer();
    return _modbusSlaveBufferSize +2;
}
