import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BOARD)


GPIO.setup(12, GPIO.OUT)
pwm = GPIO.PWM(12, 100)

pwm.start(50)

try:
	while True:
		time.sleep(0.05)


except KeyboardInterrupt:
	print('ended program')

pwm.stop()
GPIO.cleanup()
