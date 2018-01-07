#!/usr/bin/python

import smbus

bus = smbus.SMBus(1)

DEVICE_ADDRESS = 0x36
CMD_VOLTAGE = 0x0C
CMD_CURRENT = 0x0E
CMD_PERCENTAGE = 0X02
CMD_TEMPERATURE = 0X0A

# current (in mA)
bus.write_byte(DEVICE_ADDRESS, CMD_CURRENT)
H = bus.read_byte(DEVICE_ADDRESS)
L = bus.read_byte(DEVICE_ADDRESS)
I = (((H << 8) + L) >> 4) * 5 / 4
print "Current (in mA)"
print I

# percentage of battery
bus.write_byte(DEVICE_ADDRESS, CMD_PERCENTAGE)
p = bus.read_byte(DEVICE_ADDRESS)
p = p / 2
print "Percentage: "
print p

# temperature of device
bus.write_byte(DEVICE_ADDRESS, CMD_TEMPERATURE)
H = bus.read_byte(DEVICE_ADDRESS)
L = bus.read_byte(DEVICE_ADDRESS)
t = H * 8 + L / 32
T = t / 8
print "Temperature:"
print T

# voltage
bus.write_byte(DEVICE_ADDRESS, CMD_VOLTAGE)
H = bus.read_byte(DEVICE_ADDRESS)
L = bus.read_byte(DEVICE_ADDRESS)
V = ((H << 5) + (L >> 3)) * 122/100
print "Voltage:"
print V

