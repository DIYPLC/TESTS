#!/usr/bin/python3
# -*- coding: utf-8 -*-
# 26 dec 2024
# Windows 10 pro x64
# Python 3.12.2
# IDLE 3.12.z

"""
NET_PLC MASTER FOR RS232/RS485/UART
DRAFT !!!
VA 26-dec-2024
"""

import struct
import time

try: 
    import serial #RS232/RS485/UART
except: #download from internet
    import os
    os.system("python.exe -m pip install --upgrade pip")
    os.system("pip install pyserial") #python -m pip install pyserial
    import serial #RS232


class FbNetPlcMasterForSerialPort(object):

    def __init__(self) -> None:
        self.SerialPort = serial.Serial()
        self.SerialPort.port = 'COM1'
        self.SerialPort.baudrate = 9600
        self.SerialPort.bytesize = 8
        self.SerialPort.parity = 'N'
        self.SerialPort.stopbits = 1
        #self.SerialPort.timeout = 1
        try:
            self.SerialPort.open()
            print("Open",self.SerialPort.name)
        except:
            print("ERROR OPEN SERIAL RORT", self.SerialPort.name)
            exit()
        return

    def __call__(self) -> None:
        self._send_message_()
        self._receive_message_()
        self._net_plc_protocol_()
        return

    def __del__(self) -> None:
        print("Close",self.SerialPort.name)
        del self
        return

    def _send_message_(self):
        START_BYTE = int(2)
        STOR_BYTE = int(1)
        TxMessage = (struct.pack(">B", START_BYTE) +
        struct.pack(">B", STOR_BYTE))
        print(TxMessage, "len=", len(TxMessage))
        self.SerialPort.write(TxMessage)
        return

    def _receive_message_(self):
        print("wait byte from plc")
        RxByte = self.SerialPort.read() #Receiving the first byte
        RxMessage = RxByte
        while ((RxByte != 0x33) or (len(RxMessage) > 64)): #Waiting for the end of the message symbol
            RxByte = self.SerialPort.read() #Receiving a message by bytes
            RxMessage = RxMessage + RxByte
        print(RxMessage, "len=", len(RxMessage))
        return

    def _net_plc_protocol_(self):
        return


if __name__ == "__main__":
    print("Start")
    DbNetPlcMasterForSerialPort1 = FbNetPlcMasterForSerialPort()
    time.sleep(1.0) #Delay for boot loader m328p
    if (1):
    #while(1):
        DbNetPlcMasterForSerialPort1()
        time.sleep(1.0)
        del DbNetPlcMasterForSerialPort1
    print("Stop")


# @COPYLEFT ALL WRONGS RESERVED :)
# Author: VA
# Contacts: DIY.PLC.314@gmail.com
# Date of foundation of the library LIB_PLC: 2014
# License: GNU GPL-2.0-or-later
# https://www.gnu.org/licenses/old-licenses/gpl-2.0.html
# https://www.youtube.com/watch?v=n1F_MfLRlX0
# See also:
# https://www.youtube.com/@DIY_PLC
# https://github.com/DIYPLC/LIB_PLC
# https://oshwlab.com/diy.plc.314/PLC_HW1_SW1
# https://3dtoday.ru/3d-models/mechanical-parts/body/korpus-na-din-reiku
# https://t.me/DIY_PLC

# Спасибо за лекции.
# https://www.youtube.com/@unx7784/playlists
# https://www.youtube.com/@tkhirianov/playlists
