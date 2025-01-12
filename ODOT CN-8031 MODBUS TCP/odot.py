#!/usr/bin/python3
# -*- coding: utf-8 -*-

"""
Программа чтения / записи станции удаленного ввода вывода для ПЛК.
Программа протестированная, работоспособная.
Аппаратная конфигурация удаленного ввода / вывода используемая для данной программы:
Так как это пример то работает он в терминале.
MODBUS TCP SERVER ADAPTER ODOT CN-8031 1шт.
16DI 24VDC SINK ODOT CT-121F 1шт.
16DO 24VDC SOURCE ODOT CT-222F 1шт.
8AI 4-20mA 15bit ODOT CT-3238 1шт.
4AO 4-20mA 16bit ODOT CT-4234 1шт.
TERMINAL MODULE ODOT CT-5801 1шт.
https://www.odotautomation.com/remote-io-system/
python v3.9
IDLE v3.9
"""

import socket
import struct

def MODBUS_TCP_client_read_holding_register_uint16(IP_address = '127.0.0.1', TCP_port = 502, MODBUS_address = 1, Register_address = 0):
    """MODBUS TCP MASTER READ REGISTER FROM PLC"""
    try:
        Client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        Client_socket.connect((IP_address, TCP_port))
        Tx_Transaction_ID = 1
        Tx_Protocol_ID = 0
        Tx_Message_length = 6
        Tx_MODBUS_address = MODBUS_address
        Tx_MODBUS_function = 3
        Tx_Register_address = Register_address
        Tx_Register_count = 1
        Tx_ADU = struct.pack(">HHHBBHH",Tx_Transaction_ID,Tx_Protocol_ID,Tx_Message_length,Tx_MODBUS_address,Tx_MODBUS_function,Tx_Register_address,Tx_Register_count)
        Client_socket.send(Tx_ADU)
        Rx_ADU = Client_socket.recv(1500)
        (Rx_Transaction_ID,Rx_Protocol_ID,Rx_Message_length,Rx_MODBUS_address,Rx_MODBUS_function,Rx_Byte_count,Rx_Register_value) = struct.unpack(">HHHBBBH",Rx_ADU)
        Client_socket.close()
        Client_socket.__del__()
        return Rx_Register_value
    except:
        print("ERROR: MODBUS_TCP_client_read_holding_register_uint16()")
        Client_socket.close()
        Client_socket.__del__()
        return 0

def MODBUS_TCP_client_write_multiple_holding_register_uint16(IP_address = '127.0.0.1', TCP_port = 502, MODBUS_address = 1, Register_address = 0, Register_value = 0):
    """MODBUS TCP MASTER WRITE REGISTER TO PLC"""
    try:
        Client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        Client_socket.connect((IP_address, TCP_port))
        Tx_Transaction_ID   = 0
        Tx_Protocol_ID      = 0
        Tx_Message_length   = 9
        Tx_MODBUS_address   = MODBUS_address
        Tx_MODBUS_function  = 16 #Write multiple holding registers
        Tx_Register_address = int(Register_address) & 0xFFFF #Limit address
        Tx_Register_count   = 1
        Tx_Byte_count       = 2
        Tx_Register_value   = int(Register_value) & 0xFFFF #Limit value
        Tx_ADU = struct.pack(">HHHBBHHBH",Tx_Transaction_ID,Tx_Protocol_ID,Tx_Message_length,Tx_MODBUS_address,Tx_MODBUS_function,Tx_Register_address,Tx_Register_count,Tx_Byte_count,Tx_Register_value)
        Client_socket.send(Tx_ADU)
        Rx_ADU = Client_socket.recv(1500)
        (Rx_Transaction_ID,Rx_Protocol_ID,Rx_Message_length,Rx_MODBUS_address,Rx_MODBUS_function,Rx_Register_address,Rx_Register_count) = struct.unpack(">HHHBBHH",Rx_ADU)
        Client_socket.close()
        Client_socket.__del__()
        return
    except:
        print("ERROR: MODBUS_TCP_client_write_multiple_holding_register_uint16()")
        Client_socket.close()
        Client_socket.__del__()
    return

#READ FROM ODOT CN-8031 \ 16DI CT-121F
DI_WORD = MODBUS_TCP_client_read_holding_register_uint16(IP_address = '192.168.1.100', Register_address = 20480)
print("DI_WORD:", bin(DI_WORD)) #0...65535

#WRITE TO ODOT CN-8031 \ 16DO CT-222F
DO_WORD = 1 #0...65535
print("DO_WORD:", bin(DO_WORD))
MODBUS_TCP_client_write_multiple_holding_register_uint16(IP_address = '192.168.1.100', Register_address = 12288, Register_value = DO_WORD)

#READ FROM ODOT CN-8031 \ 8AI CT-3238
AI0 = MODBUS_TCP_client_read_holding_register_uint16(IP_address = '192.168.1.100', Register_address = 16384)
print("AI0:", AI0) #0...27648 -> 4...20mA

#WRITE TO ODOT CN-8031 \ 4AO CT-4234
AO0 = 13824 #0...27648 -> 4...20mA
print("AO0:", AO0)
MODBUS_TCP_client_write_multiple_holding_register_uint16(IP_address = '192.168.1.100', Register_address = 0, Register_value = AO0)

input("press any key to exit")

# @COPYLEFT ALL WRONGS RESERVED :)
# Author: VA
# Contacts: DIY.PLC.314@gmail.com
# Date start LIB_PLC: 2014
# License: GNU GPL-2.0-or-later
# https://www.gnu.org/licenses/old-licenses/gpl-2.0.html
# https://www.youtube.com/watch?v=n1F_MfLRlX0
# https://www.youtube.com/@DIY_PLC
# https://github.com/DIYPLC/LIB_PLC
# https://oshwlab.com/diy.plc.314/PLC_HW1_SW1
# https://3dtoday.ru/3d-models/mechanical-parts/body/korpus-na-din-reiku
# https://t.me/DIY_PLC
