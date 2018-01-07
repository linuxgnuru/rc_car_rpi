#!/usr/bin/python

import smbus

bus = smbus.SMBus(1)

DEVICE_ADDRESS = 0x36
CMD_VOLTAGE = 0x0C
CMD_CURRENT = 0x0E
CMD_PERCENTAGE = 0X02
CMD_TEMPERATURE = 0X0A

bus.write_byte(DEVICE_ADDRESS, CMD_CURRENT)
H = bus.read_byte(DEVICE_ADDRESS)
L = bus.read_byte(DEVICE_ADDRESS)
I = (((H << 8) + L) >> 4) * 5 / 4
print I
#  char HB = 0, LB = 0, p = 0; int t = 0, Iraw = 0; long V = 0;
#  float perc = 0, Temp = 0.0;
#    case CURRENT:
#      cmd[0] = 0x0E;
#        if (read(i2c_file, buf, 1) == 1) HB = buf[0];
#        if (read(i2c_file, buf, 1) == 1) LB = buf[0];
#        Iraw = (long) (((HB << 8) + LB) >> 4) * 5 / 4;
#        printf("Current: %d mA\n", Iraw);
#    case TEMPERATURE:
#      cmd[0] = 0x0A;
#        if (read(i2c_file, buf, 1) == 1) HB = buf[0];
#        if (read(i2c_file, buf, 1) == 1) LB = buf[0];
#        t = HB * 8 + LB / 32;
#        Temp = (float) t / 8;
#        printf("Temperature: %0.2f C\n", Temp);

