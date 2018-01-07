#!/usr/bin/python

import smbus
import time

bus = smbus.SMBus(1)

DEVICE_ADDRESS = 0x04

CMD_CHECK_SONAR_ALL = 2
CMD_CHECK_SONAR_LEFT = 3
CMD_CHECK_SONAR_CENTER = 4
CMD_CHECK_SONAR_RIGHT = 5
CMD_CHECK_SONAR_REAR = 6
CMD_GO_RIGHT = 7
CMD_GO_CENTER = 8
CMD_GO_LEFT = 9
CMD_GET_DIR = 10

def writeData(value):
    bus.write_byte(DEVICE_ADDRESS, value)
    return -1

def readData():
    number = bus.read_byte(DEVICE_ADDRESS)
    return number

def printMenu():
    print " 2 CHECK SONAR"
    print " 3 CHECK SONAR LEFT"
    print " 4 CHECK SONAR CENTER"
    print " 5 CHECK SONAR RIGHT"
    print " 6 CHECK SONAR REAR"
    print " 7 GO RIGHT"
    print " 8 GO CENTER"
    print " 9 GO LEFT"
    print "10 GET DIR"
    return -1

def Logic(number):
    if number == 1:
        print "OK"
    elif number == 11:
        print "Left Blocked"
    elif number == 12:
        print "Center Blocked"
    elif number == 13:
        print "Right Blocked"
    elif number == 14:
        print "Rear Blocked"
    elif number == 21:
        print "Somewhere Blocked"
    elif number == 16:
        print "Right"
    elif number == 17:
        print "Left"
    elif number == 18:
        print "Center"
    elif number == 19:
        print "Far right"
    elif number == 20:
        print "Far left"
sleep_time = .30

printMenu()
try:
    while True:
        
        var = input("Enter command (1 for help, 2-10): ")
        if not var:
            continue
        if var == 1:
            printMenu()
        else:
            if var > 1 and var < 7 or var == 10:
                writeData(var)
                time.sleep(sleep_time)
                number = readData()
                Logic(number)
            elif var > 6 and var < 10:
                if var == 7:
                    ret = 19 
                elif var == 8:
                    ret = 18
                elif var == 9:
                    ret = 20
                writeData(var)
                time.sleep(1)
                number = readData()
#Logic(number)
                while number != ret:
                    writeData(var)
                    time.sleep(sleep_time)
                    number = readData()
                Logic(number)
except KeyboardInterrupt:
    pass
