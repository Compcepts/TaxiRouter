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

GPIO.setup(38, GPIO.OUT)
GPIO.setup(15, GPIO.OUT)
GPIO.setup(16, GPIO.OUT)
left_motor = GPIO.PWM(38, 100)

GPIO.setup(40, GPIO.OUT)
GPIO.setup(11, GPIO.OUT)
GPIO.setup(12, GPIO.OUT)
right_motor = GPIO.PWM(40, 100)

GPIO.setup(18, GPIO.IN)
GPIO.setup(37, GPIO.IN)
GPIO.setup(3, GPIO.IN)


def forward():
	GPIO.output(11, True)
	GPIO.output(16, True)
	GPIO.output(12, False)
	GPIO.output(15, False)

def reverse():
	GPIO.output(11, False)
	GPIO.output(16, False)
	GPIO.output(12, True)
	GPIO.output(15, True)

def left():
	GPIO.output(11, True)
	GPIO.output(16, False)
	GPIO.output(12, False)
	GPIO.output(15, True)

def right():
	GPIO.output(11, False)
	GPIO.output(16, True)
	GPIO.output(12, True)
	GPIO.output(15, False)

speed = 10

turn_fast = 55
turn_slow = 5

def drive():

    forward()

    left_motor.start(35)
    right_motor.start(35)

    time.sleep(0.5)

	while True:
		left_s = GPIO.input(37)
		right_s = GPIO.input(3)

		intersection = GPIO.input(18)

		if intersection == 1:
			break

		if left_s == 1:
			left()
			left_motor.start(turn_fast+10)
			right_motor.start(turn_fast+10)

			time.sleep(0.25)

			left_motor.start(turn_fast)
			right_motor.start(turn_fast)


		elif right_s == 1:
			right()
			left_motor.start(turn_fast+10)
			right_motor.start(turn_fast+10)

			time.sleep(0.25)

			left_motor.start(turn_fast)
			right_motor.start(turn_fast)

		else:
			forward()
			left_motor.start(speed+5)
			right_motor.start(speed)

	reverse()

	left_motor.start(speed+10)
	right_motor.start(speed+10)

	time.sleep(1)


def stop():
    left_motor.start(0)
    right_motor.start(0)

def turn_right():
    right()

	left_motor.start(turn_fast+10)
	right_motor.start(turn_fast+10)

	time.sleep(0.25)

    while True:


		left_s = GPIO.input(37)
		right_s = GPIO.input(3)
    
		intersection = GPIO.input(18)

		left_motor.start(turn_fast)
		right_motor.start(turn_fast)

        if intersection == 1:
            break

    c_dir = (c_dir+1)%4
   
def turn_left():
    left()

	left_motor.start(turn_fast+10)
	right_motor.start(turn_fast+10)

	time.sleep(0.25)

    while True:


		left_s = GPIO.input(37)
		right_s = GPIO.input(3)

		intersection = GPIO.input(18)

		left_motor.start(turn_fast)
		right_motor.start(turn_fast)

        if intersection == 1:
            break

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
    left_motor.stop()
    right_motor.stop()
    GPIO.cleanup()


if __name__ == '__main__':
    drive()
    turn_left()
    drive()
