import RPi.GPIO as GPIO
import time


from enum import Enum

class Direction(Enum):
    LEFT = 0
    UP = 1
    RIGHT = 2
    DOWN = 3


# Cart global variables:

c_dir = UP

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


def turn_right():
    forward1.start(50)
    reverse1.stop()

    c_dir = (c_dir+1)%4
   
def turn_left():
    forward1.stop()
    reverse1.start(50)

    c_dir = (c_dir+3)%4

def turn(direction):
    if (c_dir == (direction+5)%4):
        turn_left()
    elif (c_dir == (direction+3)%4):
        turn_right()
    elif (c_dir == (direction+2)%4):
        turn_left()
        turn_left()
       

def shutdown():
    forward1.stop()
    reverse1.stop()
    GPIO.cleanup()
