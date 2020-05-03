import RPi.GPIO as GPIO
import time


# Cart global variables:

c_dir = 1

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

speed = 30

turn_fast = 50
veer = 43

def drive():
    start = time.time()

    left_motor.start(speed+5)
    right_motor.start(speed+5)

    time.sleep(0.15)

    forward()

    while True:
        left_s = GPIO.input(37)
        right_s = GPIO.input(3)

        if right_s == 1 and left_s == 1:
            break

        elif left_s == 1 and right_s == 0:
            left()
            left_motor.start(veer-15)
            right_motor.start(veer)
            start = time.time()

        elif left_s == 0 and right_s == 1:
            right()
            left_motor.start(veer)
            right_motor.start(veer-15)
            start = time.time()

        else:
            forward()

            if time.time() - start < 1.0:
                left_motor.start(speed+5)
                right_motor.start(speed+5)
            else:
                left_motor.start(speed)
                right_motor.start(speed)

    time.sleep(0.6)
    stop()

    time.sleep(2)

def stop():
    reverse()
    left_motor.start(1)
    right_motor.start(1)

def turn_right():
    global c_dir
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
    global c_dir
    left()

    left_motor.start(turn_fast)
    right_motor.start(turn_fast)

    while True:

        intersection = GPIO.input(18)

        if intersection == 1:
            break

    time.sleep(0.1)

    while True:

        intersection = GPIO.input(18)

        if intersection == 0:
            break

    time.sleep(0.15)

    forward()
    left_motor.start(speed)
    right_motor.start(speed)

    time.sleep(0.5)

    stop()

    c_dir = (c_dir+3)%4

    time.sleep(2)

def turn_around():
    global c_dir
    left()

    left_motor.start(turn_fast)
    right_motor.start(turn_fast)

    while True:

        intersection = GPIO.input(18)

        if intersection == 1:
            break

    time.sleep(0.1)

    while True:

        intersection = GPIO.input(18)

        if intersection == 0:
            break

    stop()

    reverse()
    left_motor.start(speed)
    right_motor.start(speed)
    time.sleep(1)

    stop()

    left()
    left_motor.start(turn_fast+10)
    right_motor.start(turn_fast+10)

    while True:

        intersection = GPIO.input(18)

        if intersection == 1:
            break

    time.sleep(0.1)

    while True:

        intersection = GPIO.input(18)

        if intersection == 0:
            break

    forward()

    right_motor.start(speed)
    left_motor.start(speed)

    time.sleep(0.25)

    stop()

    c_dir = (c_dir+2)%4



def turn(direction):
    if (c_dir == (direction+5)%4):
        turn_left()
    elif (c_dir == (direction+3)%4):
        turn_right()
    elif (c_dir == (direction+2)%4):
        turn_around()


def shutdown():
    left_motor.stop()
    right_motor.stop()
    GPIO.cleanup()


if __name__ == '__main__':
    try:
        drive()
        turn_left()
        drive()
        turn_left()
        drive()
        turn_left()
        drive()
        shutdown()

    except KeyboardInterrupt:
        shutdown()
