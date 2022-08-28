#https://github.com/emakefun/microbit-mecanum-car
#https://makecode.microbit.org/#editor
#Car random time go fwd and back

def rew():
    magicbit.motor_run_dual(magicbit.Motors.M1, 150, magicbit.Motors.M4, -150)
    magicbit.motor_run_dual(magicbit.Motors.M2, 150, magicbit.Motors.M3, -150)
    basic.pause(2000)
    magicbit.motor_stop_all()
def fwd():
    magicbit.motor_run_dual(magicbit.Motors.M1, -150, magicbit.Motors.M4, 150)
    magicbit.motor_run_dual(magicbit.Motors.M2, -150, magicbit.Motors.M3, 150)
    basic.pause(2000)
    magicbit.motor_stop_all()

def on_button_pressed_a():
    pins.digital_write_pin(DigitalPin.P12, 0)
    basic.pause(2000)
    pins.digital_write_pin(DigitalPin.P12, 1)
input.on_button_pressed(Button.A, on_button_pressed_a)

def on_button_pressed_b():
    pins.digital_write_pin(DigitalPin.P8, 0)
    basic.pause(2000)
    pins.digital_write_pin(DigitalPin.P8, 1)
input.on_button_pressed(Button.B, on_button_pressed_b)

def on_logo_pressed():
    soundExpression.hello.play_until_done()
input.on_logo_event(TouchButtonEvent.PRESSED, on_logo_pressed)

magicbit.motor_stop_all()
soundExpression.hello.play_until_done()
basic.show_leds("""
    . . . . .
        . # . # .
        . . . . .
        # . . . #
        . # # # .
""")
magicbit.Setting_the_on_board_lights(Offset.ONE, RgbColors.BLUE, rgb_ColorEffect.BREATHING)
pins.digital_write_pin(DigitalPin.P8, 0)
pins.digital_write_pin(DigitalPin.P12, 0)
basic.pause(100)
pins.digital_write_pin(DigitalPin.P8, 1)
pins.digital_write_pin(DigitalPin.P12, 1)

def on_forever():
    basic.show_string("" + str(input.temperature()) + "C")
    basic.pause(randint(20000, 60000))
    fwd()
    rew()
basic.forever(on_forever)
