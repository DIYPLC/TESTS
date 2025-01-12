#!/usr/bin/python3
# -*- coding: utf-8 -*-

"""
SOFT PLC
ODOT CN-8031 по факту может одновременно обслужить 5 TCP сокетов.

HW:
MODBUS TCP SERVER ADAPTER ODOT CN-8031 1шт.
16DI 24VDC SINK ODOT CT-121F 1шт.
16DO 24VDC SOURCE ODOT CT-222F 1шт.
8AI 4-20mA 15bit ODOT CT-3238 1шт.
4AO 4-20mA 16bit ODOT CT-4234 1шт.
TERMINAL MODULE ODOT CT-5801 1шт.

python v3.9
IDLE v3.9
Windows10 pro x64
"""

import socket #FOR TCP SERVER
import select #FOR TCP SERVER
import struct #FOR MODBUS PROTOCOL
import array  #FOR MODBUS PROTOCOL
import time
import ctypes

class MODBUS_TCP_master():

    def __init__(self):
        self.MW = array.array('H', [0]*65536) #MW[0]...MW[65535] uint16
        self.Transaction_counter = 0
        return

    def Start_TCP_client(self, IP_address = '127.0.0.1', TCP_port = 502):
        self.Client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.Client_socket.connect((IP_address, TCP_port))
        return

    def Stop_TCP_client(self):
        self.Client_socket.close()
        self.Client_socket.__del__()
        return

    def Get_MW(self, MW_address):
        return self.MW[(MW_address & 0xFFFF)]

    def Set_MW(self, MW_address, MW_value):
        self.MW[(MW_address & 0xFFFF)] = MW_value & 0xFFFF
        return

    def Read_holding_register_uint16(self, MODBUS_address = 1, Register_address = 0):
        self.Transaction_counter = (self.Transaction_counter + 1) & 0xFFFF
        Tx_Transaction_ID   = self.Transaction_counter
        Tx_Protocol_ID      = 0
        Tx_Message_length   = 6
        Tx_MODBUS_address   = MODBUS_address
        Tx_MODBUS_function  = 3 #Read holding registers
        Tx_Register_address = Register_address
        Tx_Register_count   = 1
        Tx_ADU = struct.pack(">HHHBBHH",
                             Tx_Transaction_ID,
                             Tx_Protocol_ID,
                             Tx_Message_length,
                             Tx_MODBUS_address,
                             Tx_MODBUS_function,
                             Tx_Register_address,
                             Tx_Register_count)
        self.Client_socket.send(Tx_ADU)
        Rx_ADU = self.Client_socket.recv(1500) #1500 = Ethernet MTU size
        Error = (len(Rx_ADU) != 11) #11 byte OK
        if (Error == False):
            (Rx_Transaction_ID,
             Rx_Protocol_ID,
             Rx_Message_length,
             Rx_MODBUS_address,
             Rx_MODBUS_function,
             Rx_Byte_count,
             Rx_Register_value) = struct.unpack(">HHHBBBH",Rx_ADU)
            Error = Error or (Rx_Transaction_ID  != Tx_Transaction_ID         )
            Error = Error or (Rx_Protocol_ID     != Tx_Protocol_ID            )
            Error = Error or (Rx_Message_length  != Tx_Register_count * 2 + 3 )
            Error = Error or (Rx_MODBUS_address  != Tx_MODBUS_address         )
            Error = Error or (Rx_MODBUS_function != Tx_MODBUS_function        )
            Error = Error or (Rx_Byte_count      != Tx_Register_count * 2     )
            if (Error == False):
                Register_value = Rx_Register_value
            else:
                print("Error read holding register")
                print(Tx_ADU)
                print(Rx_ADU)
                Register_value = 0
        else:
            print("Error read holding register")
            print(Tx_ADU)
            print(Rx_ADU)
            Register_value = 0
        return Register_value

    def Read_holding_register_float32(self, MODBUS_address = 1, Register_address = 0):
        """use Register_address and Register_address + 1"""
        self.Transaction_counter = (self.Transaction_counter + 1) & 0xFFFF
        Tx_Transaction_ID   = self.Transaction_counter
        Tx_Protocol_ID      = 0
        Tx_Message_length   = 6
        Tx_MODBUS_address   = MODBUS_address
        Tx_MODBUS_function  = 3 #Read holding registers
        Tx_Register_address = Register_address
        Tx_Register_count   = 2
        Tx_ADU = struct.pack(">HHHBBHH",
                             Tx_Transaction_ID,
                             Tx_Protocol_ID,
                             Tx_Message_length,
                             Tx_MODBUS_address,
                             Tx_MODBUS_function,
                             Tx_Register_address,
                             Tx_Register_count)
        self.Client_socket.send(Tx_ADU)
        Rx_ADU = self.Client_socket.recv(1500) #1500 = Ethernet MTU size
        Error = (len(Rx_ADU) != 13) #13 byte OK
        if (Error == False):
            (Rx_Transaction_ID,
             Rx_Protocol_ID,
             Rx_Message_length,
             Rx_MODBUS_address,
             Rx_MODBUS_function,
             Rx_Byte_count,
             Rx_Register_value1,
             Rx_Register_value2) = struct.unpack(">HHHBBBHH",Rx_ADU)
            Error = Error or (Rx_Transaction_ID  != Tx_Transaction_ID         )
            Error = Error or (Rx_Protocol_ID     != Tx_Protocol_ID            )
            Error = Error or (Rx_Message_length  != Tx_Register_count * 2 + 3 )
            Error = Error or (Rx_MODBUS_address  != Tx_MODBUS_address         )
            Error = Error or (Rx_MODBUS_function != Tx_MODBUS_function        )
            Error = Error or (Rx_Byte_count      != Tx_Register_count * 2     )
            if (Error == False):
                Decode_float32 = struct.unpack(">f",struct.pack(">HH",Rx_Register_value2,Rx_Register_value1))
                Register_value = Decode_float32[0]
            else:
                print("Error read holding register")
                print(Tx_ADU)
                print(Rx_ADU)
                Register_value = 0.0
        else:
            print("Error read holding register")
            print(Tx_ADU)
            print(Rx_ADU)
            Register_value = 0.0
        return Register_value

    def Write_multiple_holding_register_uint16(self, MODBUS_address = 1, Register_address = 0, Register_value = 0):
        self.Transaction_counter = (self.Transaction_counter + 1) & 0xFFFF
        Tx_Transaction_ID   = self.Transaction_counter
        Tx_Protocol_ID      = 0
        Tx_Message_length   = 9
        Tx_MODBUS_address   = MODBUS_address
        Tx_MODBUS_function  = 16 #Write multiple holding registers
        Tx_Register_address = Register_address
        Tx_Register_count   = 1
        Tx_Byte_count       = 2
        Tx_Register_value   = Register_value
        Tx_ADU = struct.pack(">HHHBBHHBH",
                             Tx_Transaction_ID,
                             Tx_Protocol_ID,
                             Tx_Message_length,
                             Tx_MODBUS_address,
                             Tx_MODBUS_function,
                             Tx_Register_address,
                             Tx_Register_count,
                             Tx_Byte_count,
                             Tx_Register_value)
        self.Client_socket.send(Tx_ADU)
        Rx_ADU = self.Client_socket.recv(1500) #1500 Ethernet MTU size
        Error = (len(Rx_ADU) != 12) #12 byte OK
        if (Error == False):
            (Rx_Transaction_ID,
             Rx_Protocol_ID,
             Rx_Message_length,
             Rx_MODBUS_address,
             Rx_MODBUS_function,
             Rx_Register_address,
             Rx_Register_count) = struct.unpack(">HHHBBHH",Rx_ADU)
            Error = Error or (Rx_Transaction_ID  != Tx_Transaction_ID )
            Error = Error or (Rx_Protocol_ID     != Tx_Protocol_ID    )
            Error = Error or (Rx_Message_length  != 6                 )
            Error = Error or (Rx_MODBUS_address  != Tx_MODBUS_address )
            Error = Error or (Rx_MODBUS_function != Tx_MODBUS_function)
            Error = Error or (Rx_Register_count  != Tx_Register_count )
            if (Error == True):
                print("Error write multiple holding register")
                print(Tx_ADU)
                print(Rx_ADU)
        else:
            print("Error write multiple holding register")
            print(Tx_ADU)
            print(Rx_ADU)
            Error = True
        return Error

    def Write_multiple_holding_register_float32(self, MODBUS_address = 1, Register_address = 0, Register_value = 0.0):
        """use Register_address and Register_address + 1"""
        self.Transaction_counter = (self.Transaction_counter + 1) & 0xFFFF
        Tx_Transaction_ID   = self.Transaction_counter
        Tx_Protocol_ID      = 0
        Tx_Message_length   = 11
        Tx_MODBUS_address   = MODBUS_address
        Tx_MODBUS_function  = 16 #Write multiple holding registers
        Tx_Register_address = Register_address
        Tx_Register_count   = 2
        Tx_Byte_count       = 4
        Tx_Register_value   = Register_value
        (Tx_Register_value2, Tx_Register_value1) = struct.unpack(">HH",struct.pack(">f",Register_value))
        Tx_ADU = struct.pack(">HHHBBHHBHH",
                             Tx_Transaction_ID,
                             Tx_Protocol_ID,
                             Tx_Message_length,
                             Tx_MODBUS_address,
                             Tx_MODBUS_function,
                             Tx_Register_address,
                             Tx_Register_count,
                             Tx_Byte_count,
                             Tx_Register_value1,
                             Tx_Register_value2)
        self.Client_socket.send(Tx_ADU)
        Rx_ADU = self.Client_socket.recv(1500) #1500 Ethernet MTU size
        Error = (len(Rx_ADU) != 12) #12 byte OK
        if (Error == False):
            (Rx_Transaction_ID,
             Rx_Protocol_ID,
             Rx_Message_length,
             Rx_MODBUS_address,
             Rx_MODBUS_function,
             Rx_Register_address,
             Rx_Register_count) = struct.unpack(">HHHBBHH",Rx_ADU)
            Error = Error or (Rx_Transaction_ID  != Tx_Transaction_ID )
            Error = Error or (Rx_Protocol_ID     != Tx_Protocol_ID    )
            Error = Error or (Rx_Message_length  != 6                 )
            Error = Error or (Rx_MODBUS_address  != Tx_MODBUS_address )
            Error = Error or (Rx_MODBUS_function != Tx_MODBUS_function)
            Error = Error or (Rx_Register_count  != Tx_Register_count )
            if (Error == True):
                print("Error write multiple holding register")
                print(Tx_ADU)
                print(Rx_ADU)
        else:
            print("Error write multiple holding register")
            print(Tx_ADU)
            print(Rx_ADU)
            Error = True
        return Error

    def Write_single_register(self, MODBUS_address = 1, Register_address = 0, Register_value = 0):
        """uint16 Register_value"""
        self.Transaction_counter = (self.Transaction_counter + 1) & 0xFFFF
        Tx_Transaction_ID   = self.Transaction_counter
        Tx_Protocol_ID      = 0
        Tx_Message_length   = 6
        Tx_MODBUS_address   = MODBUS_address
        Tx_MODBUS_function  = 6 #write_single_register
        Tx_Register_address = Register_address
        Tx_Register_value   = Register_value
        Tx_MBAP = struct.pack(">HHHB", Tx_Transaction_ID, Tx_Protocol_ID, Tx_Message_length, Tx_MODBUS_address)
        Tx_PDU = struct.pack(">BHH", Tx_MODBUS_function, Tx_Register_address, Tx_Register_value)
        Tx_ADU = Tx_MBAP + Tx_PDU
        self.Client_socket.send(Tx_ADU)
        Rx_ADU = self.Client_socket.recv(1500) #1500 Ethernet MTU size
        Error = (len(Rx_ADU) != 12) #12 byte OK
        if (Error == False):
            Error = (Rx_ADU != Tx_ADU)
            if (Error == True):
                print("Error write single register")
                print(Tx_ADU)
                print(Rx_ADU)
        else:
            print("Error write single register")
            print(Tx_ADU)
            print(Rx_ADU)
            Error = True
        return Error

    def Read_holding_registers(self, MODBUS_address = 1, Register_address = 0, Register_count = 1):
        self.Transaction_counter = (self.Transaction_counter + 1) & 0xFFFF
        Tx_Transaction_ID   = self.Transaction_counter
        Tx_Protocol_ID      = 0
        Tx_Message_length   = 6
        Tx_MODBUS_address   = MODBUS_address & 0xFF
        Tx_MODBUS_function  = 3 #Read holding registers
        Tx_Register_address = int(Register_address) & 0xFFFF
        Tx_Register_count   = int(Register_count) & 0xFF #1...127 Maximum
        Tx_MBAP = struct.pack(">HHHB", Tx_Transaction_ID, Tx_Protocol_ID, Tx_Message_length, Tx_MODBUS_address)
        Tx_PDU = struct.pack(">BHH", Tx_MODBUS_function, Tx_Register_address, Tx_Register_count)
        Tx_ADU = Tx_MBAP + Tx_PDU
        self.Client_socket.send(Tx_ADU)
        Rx_ADU = self.Client_socket.recv(1500) #1500 Ethernet MTU size
        Error = (len(Rx_ADU) != Register_count * 2 + 9)#Check len ADU
        if(Error == False):
            Rx_Transaction_ID  = int((Rx_ADU[0] << 8 ) | Rx_ADU[1]) & 0xFFFF
            Rx_Protocol_ID     = int((Rx_ADU[2] << 8 ) | Rx_ADU[3]) & 0xFFFF
            Rx_Message_length  = int((Rx_ADU[4] << 8 ) | Rx_ADU[5]) & 0xFFFF
            Rx_MODBUS_address  = int(Rx_ADU[6]) & 0xFF
            Rx_MODBUS_function = int(Rx_ADU[7]) & 0xFF
            Rx_Byte_count      = int(Rx_ADU[8]) & 0xFF
            Error = Error or (Rx_Transaction_ID  != Tx_Transaction_ID         )
            Error = Error or (Rx_Protocol_ID     != Tx_Protocol_ID            )
            Error = Error or (Rx_Message_length  != Tx_Register_count * 2 + 3 )
            Error = Error or (Rx_MODBUS_address  != Tx_MODBUS_address         )
            Error = Error or (Rx_MODBUS_function != Tx_MODBUS_function        )
            Error = Error or (Rx_Byte_count      != Tx_Register_count * 2     )
            if(Error == False):
                for Counter in range(Tx_Register_count): #Counter = 0...Register_count
                    Hi_byte = int(Rx_ADU[9  + Counter * 2]) & 0xFF
                    Lo_byte = int(Rx_ADU[10 + Counter * 2]) & 0xFF
                    Register_value = int((Hi_byte << 8 ) | Lo_byte) & 0xFFFF
                    Reg_adr = int(Counter + Register_address) & 0xFFFF
                    self.MW[Reg_adr] = Register_value
            else:
                print("Error read holding register")
        else:
                print("Error read holding register")
        return


class FbBitsToWord():
    #Входные переменные, сохраняемые.
    In0 = False
    In1 = False
    In2 = False
    In3 = False
    In4 = False
    In5 = False
    In6 = False
    In7 = False
    In8 = False
    In9 = False
    In10 = False
    In11 = False
    In12 = False
    In13 = False
    In14 = False
    In15 = False
    #Выходные переменные, сохраняемые.
    Out = 0
    def run(self):
        self.Out = 0;
        if(bool(self.In0)):
            self.Out = (self.Out | 0b0000000000000001)
        if(bool(self.In1)):
            self.Out = (self.Out | 0b0000000000000010)
        if(bool(self.In2)):
            self.Out = (self.Out | 0b0000000000000100)
        if(bool(self.In3)):
            self.Out = (self.Out | 0b0000000000001000)
        if(bool(self.In4)):
            self.Out = (self.Out | 0b0000000000010000)
        if(bool(self.In5)):
            self.Out = (self.Out | 0b0000000000100000)
        if(bool(self.In6)):
            self.Out = (self.Out | 0b0000000001000000)
        if(bool(self.In7)):
            self.Out = (self.Out | 0b0000000010000000)
        if(bool(self.In8)):
            self.Out = (self.Out | 0b0000000100000000)
        if(bool(self.In9)):
            self.Out = (self.Out | 0b0000001000000000)
        if(bool(self.In10)):
            self.Out = (self.Out | 0b0000010000000000)
        if(bool(self.In11)):
            self.Out = (self.Out | 0b0000100000000000)
        if(bool(self.In12)):
            self.Out = (self.Out | 0b0001000000000000)
        if(bool(self.In13)):
            self.Out = (self.Out | 0b0010000000000000)
        if(bool(self.In14)):
            self.Out = (self.Out | 0b0100000000000000)
        if(bool(self.In15)):
            self.Out = (self.Out | 0b1000000000000000)
        return


class FbWordToBits():
    #Входные переменные, сохраняемые.
    In = 0
    #Выходные переменные, сохраняемые.
    Out0 = False
    Out1 = False
    Out2 = False
    Out3 = False
    Out4 = False
    Out5 = False
    Out6 = False
    Out7 = False
    Out8 = False
    Out9 = False
    Out10 = False
    Out11 = False
    Out12 = False
    Out13 = False
    Out14 = False
    Out15 = False
    #Внутренние переменные, сохраняемые.
    def run(self):
        self.Out0  = bool(self.In & 0b0000000000000001)
        self.Out1  = bool(self.In & 0b0000000000000010)
        self.Out2  = bool(self.In & 0b0000000000000100)
        self.Out3  = bool(self.In & 0b0000000000001000)
        self.Out4  = bool(self.In & 0b0000000000010000)
        self.Out5  = bool(self.In & 0b0000000000100000)
        self.Out6  = bool(self.In & 0b0000000001000000)
        self.Out7  = bool(self.In & 0b0000000010000000)
        self.Out8  = bool(self.In & 0b0000000100000000)
        self.Out9  = bool(self.In & 0b0000001000000000)
        self.Out10 = bool(self.In & 0b0000010000000000)
        self.Out11 = bool(self.In & 0b0000100000000000)
        self.Out12 = bool(self.In & 0b0001000000000000)
        self.Out13 = bool(self.In & 0b0010000000000000)
        self.Out14 = bool(self.In & 0b0100000000000000)
        self.Out15 = bool(self.In & 0b1000000000000000)
        return


class FbBlink():
    #Входные переменные, сохраняемые.
    Time_on_ns = 1000*1000000
    Time_off_ns = 1000*1000000
    Time_sample_ns = 0
    Reset = False
    #Выходные переменные, сохраняемые.
    Out = False
    #Внутренние переменные, сохраняемые.
    Timer1_ns = 0
    def run(self):
        #Внутренние переменные, не сохраняемые.
        self.Time_period_ns = self.Time_on_ns + self.Time_off_ns
        if((self.Timer1_ns >= self.Time_period_ns) or self.Reset):
            self.Timer1_ns = 0
        else:
            self.Timer1_ns = self.Timer1_ns + self.Time_sample_ns
        if(self.Timer1_ns <= self.Time_on_ns):
            self.Out = True
        else:
            self.Out = False
        return


class Global_var():
    """Глобальные переменные и классы"""
    Time_cur_ns = 0
    Time_prev_ns = 0
    Time_sample_ns = 0
    Time_sample_max_ns = 0
    IO49_S1_Di0  = False #READ FROM ODOT CN-8031 \ 16DI CT-121F Slot1
    IO49_S1_Di1  = False #READ FROM ODOT CN-8031 \ 16DI CT-121F Slot1
    IO49_S1_Di2  = False #READ FROM ODOT CN-8031 \ 16DI CT-121F Slot1
    IO49_S1_Di3  = False #READ FROM ODOT CN-8031 \ 16DI CT-121F Slot1
    IO49_S1_Di4  = False #READ FROM ODOT CN-8031 \ 16DI CT-121F Slot1
    IO49_S1_Di5  = False #READ FROM ODOT CN-8031 \ 16DI CT-121F Slot1
    IO49_S1_Di6  = False #READ FROM ODOT CN-8031 \ 16DI CT-121F Slot1
    IO49_S1_Di7  = False #READ FROM ODOT CN-8031 \ 16DI CT-121F Slot1
    IO49_S1_Di8  = False #READ FROM ODOT CN-8031 \ 16DI CT-121F Slot1
    IO49_S1_Di9  = False #READ FROM ODOT CN-8031 \ 16DI CT-121F Slot1
    IO49_S1_Di10 = False #READ FROM ODOT CN-8031 \ 16DI CT-121F Slot1
    IO49_S1_Di11 = False #READ FROM ODOT CN-8031 \ 16DI CT-121F Slot1
    IO49_S1_Di12 = False #READ FROM ODOT CN-8031 \ 16DI CT-121F Slot1
    IO49_S1_Di13 = False #READ FROM ODOT CN-8031 \ 16DI CT-121F Slot1
    IO49_S1_Di14 = False #READ FROM ODOT CN-8031 \ 16DI CT-121F Slot1
    IO49_S1_Di15 = False #READ FROM ODOT CN-8031 \ 16DI CT-121F Slot1
    IO49_S2_Do0  = False #WRITE TO ODOT CN-8031 \ 16DO CT-222F Slot2
    IO49_S2_Do1  = False #WRITE TO ODOT CN-8031 \ 16DO CT-222F Slot2
    IO49_S2_Do2  = False #WRITE TO ODOT CN-8031 \ 16DO CT-222F Slot2
    IO49_S2_Do3  = False #WRITE TO ODOT CN-8031 \ 16DO CT-222F Slot2
    IO49_S2_Do4  = False #WRITE TO ODOT CN-8031 \ 16DO CT-222F Slot2
    IO49_S2_Do5  = False #WRITE TO ODOT CN-8031 \ 16DO CT-222F Slot2
    IO49_S2_Do6  = False #WRITE TO ODOT CN-8031 \ 16DO CT-222F Slot2
    IO49_S2_Do7  = False #WRITE TO ODOT CN-8031 \ 16DO CT-222F Slot2
    IO49_S2_Do8  = False #WRITE TO ODOT CN-8031 \ 16DO CT-222F Slot2
    IO49_S2_Do9  = False #WRITE TO ODOT CN-8031 \ 16DO CT-222F Slot2
    IO49_S2_Do10 = False #WRITE TO ODOT CN-8031 \ 16DO CT-222F Slot2
    IO49_S2_Do11 = False #WRITE TO ODOT CN-8031 \ 16DO CT-222F Slot2
    IO49_S2_Do12 = False #WRITE TO ODOT CN-8031 \ 16DO CT-222F Slot2
    IO49_S2_Do13 = False #WRITE TO ODOT CN-8031 \ 16DO CT-222F Slot2
    IO49_S2_Do14 = False #WRITE TO ODOT CN-8031 \ 16DO CT-222F Slot2
    IO49_S2_Do15 = False #WRITE TO ODOT CN-8031 \ 16DO CT-222F Slot2
    IO49_S3_Ai0  = 0 #READ FROM ODOT CN-8031 \ 8AI CT-3238 Slot3
    IO49_S3_Ai1  = 0 #READ FROM ODOT CN-8031 \ 8AI CT-3238 Slot3
    IO49_S3_Ai2  = 0 #READ FROM ODOT CN-8031 \ 8AI CT-3238 Slot3
    IO49_S3_Ai3  = 0 #READ FROM ODOT CN-8031 \ 8AI CT-3238 Slot3
    IO49_S3_Ai4  = 0 #READ FROM ODOT CN-8031 \ 8AI CT-3238 Slot3
    IO49_S3_Ai5  = 0 #READ FROM ODOT CN-8031 \ 8AI CT-3238 Slot3
    IO49_S3_Ai6  = 0 #READ FROM ODOT CN-8031 \ 8AI CT-3238 Slot3
    IO49_S3_Ai7  = 0 #READ FROM ODOT CN-8031 \ 8AI CT-3238 Slot3
    IO49_S4_Ao0  = 0 #WRITE TO ODOT CN-8031 \ 4AO CT-4234 Slot4
    IO49_S4_Ao1  = 0 #WRITE TO ODOT CN-8031 \ 4AO CT-4234 Slot4
    IO49_S4_Ao2  = 0 #WRITE TO ODOT CN-8031 \ 4AO CT-4234 Slot4
    IO49_S4_Ao3  = 0 #WRITE TO ODOT CN-8031 \ 4AO CT-4234 Slot4
    DbBlink1 = FbBlink()


def Time_sampling(Reset = False):
    #t = time.time_ns() #Точнее
    t = time.monotonic_ns() #Надежнее
    #print(t)
    if(Reset): #Инициализация при сбросе
        GV.Time_cur_ns = t
        GV.Time_prev_ns = GV.Time_cur_ns
    else:
        GV.Time_cur_ns = t
        if(GV.Time_cur_ns >= GV.Time_prev_ns): #Все идет хорошо
            GV.Time_sample_ns = GV.Time_cur_ns - GV.Time_prev_ns
            #print(GV.Time_sample_ns)
        GV.Time_prev_ns = GV.Time_cur_ns
        if(GV.Time_sample_ns > GV.Time_sample_max_ns): #Запомнить максимум
            GV.Time_sample_max_ns = GV.Time_sample_ns
            print("Ts max:", GV.Time_sample_max_ns, "ns")
    return

def Read_digital_input():
    #READ FROM ODOT CN-8031 \ 16DI CT-121F
    DI_WORD = IO49.Read_holding_register_uint16(Register_address = 20480)
    DbWordToBits = FbWordToBits()
    DbWordToBits.In = DI_WORD
    DbWordToBits.run()
    GV.IO49_S1_Di0  = DbWordToBits.Out0
    GV.IO49_S1_Di1  = DbWordToBits.Out1
    GV.IO49_S1_Di2  = DbWordToBits.Out2
    GV.IO49_S1_Di3  = DbWordToBits.Out3
    GV.IO49_S1_Di4  = DbWordToBits.Out4
    GV.IO49_S1_Di5  = DbWordToBits.Out5
    GV.IO49_S1_Di6  = DbWordToBits.Out6
    GV.IO49_S1_Di7  = DbWordToBits.Out7
    GV.IO49_S1_Di8  = DbWordToBits.Out8
    GV.IO49_S1_Di9  = DbWordToBits.Out9
    GV.IO49_S1_Di10 = DbWordToBits.Out10
    GV.IO49_S1_Di11 = DbWordToBits.Out11
    GV.IO49_S1_Di12 = DbWordToBits.Out12
    GV.IO49_S1_Di13 = DbWordToBits.Out13
    GV.IO49_S1_Di14 = DbWordToBits.Out14
    GV.IO49_S1_Di15 = DbWordToBits.Out15
    return

def Write_digital_output():
    DbBitsToWord = FbBitsToWord()
    DbBitsToWord.In0 = GV.IO49_S2_Do0
    DbBitsToWord.In1 = GV.IO49_S2_Do1
    DbBitsToWord.In2 = GV.IO49_S2_Do2
    DbBitsToWord.In3 = GV.IO49_S2_Do3
    DbBitsToWord.In4 = GV.IO49_S2_Do4
    DbBitsToWord.In5 = GV.IO49_S2_Do5
    DbBitsToWord.In6 = GV.IO49_S2_Do6
    DbBitsToWord.In7 = GV.IO49_S2_Do7
    DbBitsToWord.In8 = GV.IO49_S2_Do8
    DbBitsToWord.In9 = GV.IO49_S2_Do9
    DbBitsToWord.In10 = GV.IO49_S2_Do10
    DbBitsToWord.In11 = GV.IO49_S2_Do11
    DbBitsToWord.In12 = GV.IO49_S2_Do12
    DbBitsToWord.In13 = GV.IO49_S2_Do13
    DbBitsToWord.In14 = GV.IO49_S2_Do14
    DbBitsToWord.In15 = GV.IO49_S2_Do15
    DbBitsToWord.run()
    DO_WORD = DbBitsToWord.Out
    #WRITE TO ODOT CN-8031 \ 16DO CT-222F
    IO49.Write_multiple_holding_register_uint16(Register_address = 12288, Register_value = DO_WORD)
    return

def Read_analog_input():
    #READ FROM ODOT CN-8031 \ 8AI CT-3238
    GV.IO49_S3_Ai0 = IO49.Read_holding_register_uint16(Register_address = 16384) #0...27648 -> 4...20mA
    GV.IO49_S3_Ai1 = IO49.Read_holding_register_uint16(Register_address = 16385) #0...27648 -> 4...20mA
    GV.IO49_S3_Ai2 = IO49.Read_holding_register_uint16(Register_address = 16386) #0...27648 -> 4...20mA
    GV.IO49_S3_Ai3 = IO49.Read_holding_register_uint16(Register_address = 16387) #0...27648 -> 4...20mA
    GV.IO49_S3_Ai4 = IO49.Read_holding_register_uint16(Register_address = 16388) #0...27648 -> 4...20mA
    GV.IO49_S3_Ai5 = IO49.Read_holding_register_uint16(Register_address = 16389) #0...27648 -> 4...20mA
    GV.IO49_S3_Ai6 = IO49.Read_holding_register_uint16(Register_address = 16390) #0...27648 -> 4...20mA
    GV.IO49_S3_Ai7 = IO49.Read_holding_register_uint16(Register_address = 16391) #0...27648 -> 4...20mA
    return

def Write_analog_output():
    #WRITE TO ODOT CN-8031 \ 4AO CT-4234
    IO49.Write_multiple_holding_register_uint16(Register_address = 0, Register_value = GV.IO49_S4_Ao0) #0...27648 -> 4...20mA
    IO49.Write_multiple_holding_register_uint16(Register_address = 1, Register_value = GV.IO49_S4_Ao1) #0...27648 -> 4...20mA
    IO49.Write_multiple_holding_register_uint16(Register_address = 2, Register_value = GV.IO49_S4_Ao2) #0...27648 -> 4...20mA
    IO49.Write_multiple_holding_register_uint16(Register_address = 3, Register_value = GV.IO49_S4_Ao3) #0...27648 -> 4...20mA
    return

def Task1(Reset = False, Ts_ns = 0): #Пользовательская задача.
    GV.DbBlink1.Time_on_ns  = 250*1000000
    GV.DbBlink1.Time_off_ns = 250*1000000
    GV.DbBlink1.Time_sample_ns = Ts_ns
    GV.DbBlink1.Reset = Reset
    GV.DbBlink1.run()
    GV.IO49_S2_Do0 = GV.DbBlink1.Out
    return

def init(): #Как в ардуино
    Time_sampling(Reset = True)
    Task1(Reset = True, Ts_ns = 0)
    return

def loop(): #Как в ардуино
    Time_sampling(Reset = False)
    Read_digital_input()
    Read_analog_input()
    Task1(Reset = False, Ts_ns = GV.Time_sample_ns)
    Write_digital_output()
    Write_analog_output()
    return

IO49 = MODBUS_TCP_master()
IO49.Start_TCP_client(IP_address = '192.168.13.49')
GV = Global_var()
init()
while(True):
    loop()

"""
***********************************************************************************
Упрощенная архитектура программного ПЛК.
ODOT CN-8031 Remote IO MODBUS TCP SLAVE
 |
MODBUS_TCP_MASTER
 |
SOFT_PLC.py
 |
MODBUS_TCP_SLAVE
 |
SCADA WinCC MODBUS_TCP_MASTER

Основной код управления программного ПЛК на языке си.
Связь с аппаратными ресурсами на языке пайтон.
SOFT_PLC.py <-> SOFT_PLC.c <-+-> Task_1.c <-> PIDcontrol.c
                             |
                             +-> Task_2.c <-> Blink.c

Вариант архитектуры где можно проще добиться надежности и синхронизации.
Он менее логичный и вероятно более медленный.
А еще при данной архитектуре приложение можно выполнять одновременно на четырех компьютерах по сети не знаю только зачем.
MODBUS_TCP_SLAVE.py (приложение фоновое 1)
 |
 +-GUI.pyw (приложение графическое 1)
 |
 +-MODBUS_TCP_MASTER_IO1.py (приложение фоновое 2)
 |
 +-SOFT_PLC.py (приложение фоновое 3)

Основной модуль (Можно заменить на InSat MODBUS OPC).
MODBUS_TCP_SLAVE.py
 |
MODBUS_TCP_SLAVE

Модуль связи с удаленным вводом выводом.
MODBUS_TCP_MASTER_IO1.py
 |
 +-MODBUS_TCP_MASTER ->- MODBUS_TCP_SLAVE.py
 |
 +-MODBUS_TCP_MASTER -<- ODOT CN-8031 Remote IO MODBUS TCP SLAVE

Модуль программного ПЛК.
SOFT_PLC.py
 |
 +-MODBUS_TCP_MASTER ->- MODBUS_TCP_SLAVE.py
 |
 +-SOFT_PLC.cpp -<- Task_1.c -<- PIDcontrol.c

Модуль управления с графическим интерфейсом управления.
GUI.py
 |
 +-MODBUS_TCP_MASTER ->- MODBUS_TCP_SLAVE.py
 |
 +-tkinter -<- GUI

Модуль связи с ПЛК SIMATIC S7 (опция).
PLC_S7.py
 |
 +-MODBUS_TCP_MASTER ->- MODBUS_TCP_SLAVE.py
 |
 +-snap7 -<- PLC S7-300

Модуль связи с СУБД MS SQL Server (опция).
MS_SQL.py
 |
 +-MODBUS_TCP_MASTER -<- MODBUS_TCP_SLAVE.py
 |
 +-pyodbc ->- MS SQL Server

Карта регистров MODBUS.
MODBUS TCP SLAVE
IP ADDRESS = 127.0.0.1
TCP PORT = 502
MODBUS ADDRESS = 1
MODBUS function 3 read_holding_register
MODBUS function 16 write multiple holding registers
HOLDING REGISTER uint16 default data type
HR0 uint16 Reset 0/1 Флаг первого скана.
HR1 uint32 Ts_ms Время скана
HR2 uint32 Ts_ms Время скана
HR3 uint32 Ts_ms millis()
HR4 uint32 Ts_ms millis()
"""

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
