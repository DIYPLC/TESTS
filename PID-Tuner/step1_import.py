#!/usr/bin/python3
# -*- coding: utf-8 -*-

"""
Программа для настройки ПИД- регулятора.
Шаг 0й импорт данный PLC PIDcontrol из WinCC classic trends или PLC анализатора.
Шаг 1й импорт данных из csv файла и подготовка их к идентификации передаточной функции в формате json.
Шаг 2й Идентификация математематической модели объекта управления (передаточной функции).
Шаг 3й Рассчет настроек ПИД- регулятора
Шаг 3.1 Построение модели замкнутой системы управления в рабочей точке и в отклонениях от абсолютных величин.
Шаг 3.2 Расчет критериев качества работы замкнутой системы управления.
Шаг 3.3 Оптимизация настроек ПИД- регулятора по выбранному критерию.
"""

import csv
import json
try: #импортируем библиотеку
    import matplotlib.pyplot
except: #если нет библиотеки качаем ее из интернета и пробуем еще раз
    import os
    os.system("pip install matplotlib")

def read_csv_file_to_list_3d(filename = 'step0_trends.csv'):
    list1 = []
    with open(filename, newline='') as csvfile:
        reader = csv.reader(csvfile, delimiter = ';')
        i = 0
        for row in reader:
            list1.append(row)
            i = i + 1
    del list1[0] # del text string
    return list1

def list_3d_to_time_axis(list_3d):
    list1 = list_3d
    time = []
    for i in list1:
        time.append(int(i[0]))
    return time

def list_3d_to_MW_axis(list_3d):
    list1 = list_3d
    MW = []
    for i in list1:
        MW.append(float(i[1]))
    return MW

def list_3d_to_PV_axis(list_3d):
    list1 = list_3d
    PV = []
    for i in list1:
        PV.append(float(i[2]))
    return PV

def offset_for_time(list_in):
    list_out = []
    offset = 129930
    for current_vale in list_in:
        list_out.append(current_vale - offset)
    return list_out

def offset_for_MW(list_in):
    list_out = []
    offset = 42
    for current_vale in list_in:
        list_out.append(current_vale - offset)
    return list_out

def offset_for_PV(list_in):
    list_out = []
    offset = 5
    for current_vale in list_in:
        list_out.append(current_vale - offset)
    return list_out

def resampling_10ms_to_100ms(list_in):
    list_out = []
    #list_out.append(list_in[0]) первый сэмпл выкидываем для постоянства времени скана в начале
    offset = 5
    i = 1
    for current_vale in list_in:
        if (i%10 == 0): # читаем каждый 10й сэмпл.
            list_out.append(current_vale)
        i = i + 1
    return list_out

def offset_for_time2(list_in):
    list_out = []
    offset = 90
    for current_vale in list_in:
        list_out.append(current_vale - offset)
    return list_out

def export_axis_to_json(t_axis, x_axis, y_axis):
    with open('step2_t_axis.json', 'w') as file:
        json.dump(t_axis, file)
    with open('step2_x_axis.json', 'w') as file:
        json.dump(x_axis, file)
    with open('step2_y_axis.json', 'w') as file:
        json.dump(y_axis, file)
        return

def print_trend1(t_axis, y_axis, x_axis):
    try:
        matplotlib.pyplot.title("Speed to Pressure")
        matplotlib.pyplot.xlabel("time ms")
        matplotlib.pyplot.ylabel("HZ, Kgs/cm2")
        matplotlib.pyplot.plot(t_axis,y_axis)
        matplotlib.pyplot.plot(t_axis,x_axis)
        matplotlib.pyplot.show()
    except:
        ("error import matplotlib")

if __name__ == "__main__":
    # читаем из файла в общий массив с тремя осями
    list_3d = read_csv_file_to_list_3d(filename = 'step0_trends.csv')
    print("----------------------------------------------------------------------------")
    print(list_3d[0], "// list_3d[]")
    print(list_3d[1], "// list_3d[]")
    print(list_3d[2], "// list_3d[]")
    print("----------------------------------------------------------------------------")
    # из общего массива достаем отдеьные оси
    time = list_3d_to_time_axis(list_3d)
    MW = list_3d_to_MW_axis(list_3d)
    PV = list_3d_to_PV_axis(list_3d)
    print(time[0], "time ms", MW[0], "MW Hz", PV[0], "PV Kgs/cm2", "// list_3d_to_time_axis()")
    print(time[1], "time ms", MW[1], "MW Hz", PV[1], "PV Kgs/cm2", "// list_3d_to_time_axis()")
    print(time[2], "time ms", MW[2], "MW Hz", PV[2], "PV Kgs/cm2", "// list_3d_to_time_axis()")
    print("----------------------------------------------------------------------------")
    # Перенос графиков в начало координат для интегрирования с нулевыми начальными условиями
    time = offset_for_time(time)
    MW = offset_for_MW(MW)
    PV = offset_for_PV(PV)
    print(time[0], "time ms", MW[0], "MW Hz", PV[0], "PV Kgs/cm2", "// offset_for_time()")
    print(time[1], "time ms", MW[1], "MW Hz", PV[1], "PV Kgs/cm2", "// offset_for_time()")
    print(time[2], "time ms", MW[2], "MW Hz", PV[2], "PV Kgs/cm2", "// offset_for_time()")
    print("----------------------------------------------------------------------------")
    # исходные данные из анализатора излишне продудлирваны на одну реальную выборку 10 записей
    # Ресемплинг читаем каждую 10ю выборку остальные выкидываем
    time = resampling_10ms_to_100ms(time)
    MW = resampling_10ms_to_100ms(MW)
    PV = resampling_10ms_to_100ms(PV)
    print(time[0], "time ms", MW[0], "MW Hz", PV[0], "PV Kgs/cm2", "// resampling_10ms_to_100ms()")
    print(time[1], "time ms", MW[1], "MW Hz", PV[1], "PV Kgs/cm2", "// resampling_10ms_to_100ms()")
    print(time[2], "time ms", MW[2], "MW Hz", PV[2], "PV Kgs/cm2", "// resampling_10ms_to_100ms()")
    print("----------------------------------------------------------------------------")
    # Коррекция оси времени после ресемплинга.
    time= offset_for_time2(time)
    print(time[0], "time ms", MW[0], "MW Hz", PV[0], "PV Kgs/cm2", "// offset_for_time2()")
    print(time[1], "time ms", MW[1], "MW Hz", PV[1], "PV Kgs/cm2", "// offset_for_time2()")
    print(time[2], "time ms", MW[2], "MW Hz", PV[2], "PV Kgs/cm2", "// offset_for_time2()")
    print("----------------------------------------------------------------------------")
    # Опрравка данных в программу для идентификации передаточной функции
    export_axis_to_json(t_axis = time, y_axis = PV, x_axis = MW)
    # 2D график для просмотра результата
    print_trend1(t_axis = time, y_axis = PV, x_axis = MW)
    input("Press any key for exit...")

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
