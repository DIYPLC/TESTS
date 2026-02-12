#!/usr/bin/python3
# -*- coding: utf-8 -*-

"""
Мат модель объекта управления.
W(p) = y(t)/x(t) = PV(t)/MV(t) = K1 / (1 + T1 * p), p==d/dt
"""

import json
try: #импортируем библиотеку
    import matplotlib.pyplot
except: #если нет библиотеки качаем ее из интернета и пробуем еще раз
    import os
    os.system("pip install matplotlib")
import FbFilterA

with open('step2_t_axis.json') as file:
    t_axis = json.load(file)

with open('step2_x_axis.json') as file:
    x_axis = json.load(file)

with open('step2_y_axis.json') as file:
    y_axis = json.load(file)


def calc_time_sample_ms_axis(axis_in):
    # инкрементная ось времени преобразуется в ось временем скана как в плк в милисекундах.
    axis_out = []
    time_previous_ms = 0
    time_sample_ms = 0
    for time_current_ms in axis_in:
        time_sample_ms = time_current_ms - time_previous_ms
        time_previous_ms = time_current_ms
        axis_out.append(time_sample_ms)
    return axis_out


def ident(gain = 0.2, time_filter_s = 0.3):
    DbFilterA1 = FbFilterA.FbFilterA()
    error_model = 0.0
    i = 0
    for time_sample_ms in Ts_ms_axis:
        DbFilterA1.In = x_axis[i] * gain
        DbFilterA1.Tf = time_filter_s
        DbFilterA1.Ts = time_sample_ms * 0.001
        DbFilterA1()
        try:
            error_model = error_model + abs(y_axis[i] - DbFilterA1.Out)
        except:
            error_model = 2**32
        i = i + 1
    return error_model

def find_tranfer_function():
    error_minimum = 2**64
    error = error_minimum
    gain_find = 0
    time_filter_s_find = 0
    gain = 0.1
    time_filter_s = 0.3
    for i in range(200):
        gain = gain + 0.001
        time_filter_s = 0.1
        for j in range(200):
            time_filter_s = time_filter_s + 0.01
            error = ident(gain = gain, time_filter_s = time_filter_s)
            if error < error_minimum: # Запомнить найденный минимум
                error_minimum = error
                gain_find = gain
                time_filter_s_find = time_filter_s
    gain = gain_find
    time_filter_s = time_filter_s_find
    error = ident(gain = gain, time_filter_s = time_filter_s)

    return (gain, time_filter_s, error)

y_axis_math_model = []

def plotter(gain = 0.0, time_filter_s = 0.0):
    DbFilterA1 = FbFilterA.FbFilterA()
    error_model = 0.0
    i = 0
    for time_sample_ms in Ts_ms_axis:
        DbFilterA1.In = x_axis[i] * gain
        DbFilterA1.Tf = time_filter_s
        DbFilterA1.Ts = time_sample_ms * 0.001
        DbFilterA1()
        y_axis_math_model.append(DbFilterA1.Out)
        i = i + 1
    return error_model

def print_trend1(t_axis, y_axis, x_axis):
    try:
        matplotlib.pyplot.title("trend and model")
        matplotlib.pyplot.xlabel("time ms")
        matplotlib.pyplot.ylabel("HZ, Kgs/cm2")
        matplotlib.pyplot.plot(t_axis,y_axis)
        matplotlib.pyplot.plot(t_axis,x_axis)
        matplotlib.pyplot.show()
    except:
        ("error import matplotlib")

if __name__ == "__main__":
    print("len(t_axis) =", len(t_axis))
    print("len(x_axis) =", len(x_axis))
    print("len(y_axis) =", len(y_axis))
    Ts_ms_axis = calc_time_sample_ms_axis(t_axis)
    print("len(Ts_ms_axis) =", len(Ts_ms_axis))
    gain, time_filter_s, error = find_tranfer_function()
    print("-------------------------------------------------------------------")
    print("W(p) = y(t)/x(t) = PV(t)/MV(t) = K1 / (1 + T1 * p), p==d/dt")
    print("K1 =", gain, "T1 =", time_filter_s)
    print("gain =", gain, "time_filter_s =", time_filter_s ,"error =", error)
    print("-------------------------------------------------------------------")
    plotter(gain = gain, time_filter_s = time_filter_s)
    print("len(y_axis_math_model) =", len(y_axis_math_model))
    print_trend1(t_axis, y_axis, y_axis_math_model)

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
