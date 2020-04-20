import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BOARD)


GPIO.setup(12, GPIO.OUT)
GPIO.setup(11, GPIO.OUT)

forward1 = GPIO.PWM(12, 100)
reverse1 = GPIO.PWM(11, 100)


def drive():
    forward1.start(50)
    reverse1.stop()


def stop():
    forward1.stop()
    reverse1.stop()


def turn_left():
    reverse1.start(50)
    forward1.stop()


def turn_right():
    forward1.start(50)
    reverse1.stop()


def shutdown():
    forward1.stop()
    reverse1.stop()
    GPIO.cleanup()
