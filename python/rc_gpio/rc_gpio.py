#!/usr/bin/python

import smbus
import time
import RPi.GPIO as GPIO

GPIO.setwarnings(False)

bus = smbus.SMBus(1)

DEVICE_ADDRESS = 0x04

remPower = 18
remForward = 23
remBackward = 24

batLedYT = 22
batLedYB = 27
batLedRT = 17
batLedRB = 4

GPIO.setmode(GPIO.BCM)
GPIO.setup(remPower, GPIO.OUT)
#GPIO.setup(remForward, GPIO.IN, pull_up_down = GPIO_PUD_UP)
#GPIO.output(remForward, 1)
GPIO.setup(remForward, GPIO.OUT, initial=1)
#GPIO.setup(remBackward, GPIO.IN, pull_up_down = GPIO_PUD_UP)
#GPIO.output(remBackward, 1)
GPIO.setup(remBackward, GPIO.OUT, initial=1)
GPIO.setup(batLedYT, GPIO.OUT, initial=1)
GPIO.setup(batLedYB, GPIO.OUT, initial=1)
GPIO.setup(batLedRT, GPIO.OUT, initial=1)
GPIO.setup(batLedRB, GPIO.OUT, initial=1)

#GPIO.cleanup()

try:
    while True:
        GPIO.output(batLedYT, 0)
        time.sleep(.250)
        GPIO.output(batLedYT, 1)
        GPIO.output(batLedYB, 0)
        time.sleep(.250)
        GPIO.output(batLedYB, 1)
        GPIO.output(batLedRT, 0)
        time.sleep(.250)
        GPIO.output(batLedRT, 1)
        GPIO.output(batLedRB, 0)
        time.sleep(.250)
        GPIO.output(batLedRB, 1)
#        var = input("Enter command (1 for help, 2-10): ")
#        if not var:
#            continue
#        if var == 1:
#            printMenu()
#        else:
except KeyboardInterrupt:
    pass
GPIO.output(batLedYT, 1)
GPIO.output(batLedYB, 1)
GPIO.output(batLedRT, 1)
GPIO.output(batLedRB, 1)
