#!/usr/bin/python

import smbus

bus = smbus.SMBus(1)

DEVICE_ADDRESS = 0x36
CMD_VOLTAGE = 0x0C
CMD_CURRENT = 0x0E
CMD_PERCENTAGE = 0X02
CMD_TEMPERATURE = 0X0A

bus.write_byte(DEVICE_ADDRESS, CMD_VOLTAGE)
H = bus.read_byte(DEVICE_ADDRESS)
L = bus.read_byte(DEVICE_ADDRESS)
V = ((H << 5) + (L >> 3)) * 122/100
print V
#    case VOLTAGE:
#      cmd[0] = 0x0C;
#        if (read(i2c_file, buf, 1) == 1) HB = buf[0];
#        if (read(i2c_file, buf, 1) == 1) LB = buf[0];
#        V = (unsigned long) ((HB << 5) + (LB >> 3)) * 122/100;
#        printf("Voltage: %ld\n", V);

