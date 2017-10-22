#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "rc_header.h"

#define BATTERY_CAPACITY 1200 // mAh

int i2c_file;
int Current[10];

_Bool TTE();

_Bool isCharging()
{
    _Bool b;
    if ((i2c_file = open(devName, O_RDWR)) < 0)
    {
        fprintf(stderr, "[%d] [%s] [%s] I2C: Failed to access %s\n", __LINE__, __FILE__, __func__, devName);
        exit(1);
    }
    if (ioctl(i2c_file, I2C_SLAVE, ADDRESS_ES) < 0)
    {
        fprintf(stderr, "[%d] [%s] [%s] I2C: Failed to acquire bus access/talk to slave 0x%x\n", __LINE__, __FILE__, __func__, ADDRESS_ES);
        exit(1);
    }
    b = TTE();
    b = TTE();
    close(i2c_file);
    return b;
}

_Bool TTE()
{
    unsigned char cmd[16];
    int i;
    char HB = 0, LB = 0;
    int Iraw = 0;
    int p = 0;
    float perc = 0;
    _Bool charging = FALSE;

    for (i = 9; i > 0; i--)
        Current[i] = Current[i - 1];
    cmd[0] = 0x0E;
    if (write(i2c_file, cmd, 1) == 1)
    {
        usleep(10000); // wait for messages to be sent
        char buf[1];
        if (read(i2c_file, buf, 1) == 1) HB = buf[0];
        if (read(i2c_file, buf, 1) == 1) LB = buf[0];
        Iraw = (long) (((HB << 8) + LB) >> 4) * 5 / 4;
        Current[0] = Iraw;
        for (i = 9; i >= 0; i--)
        {
            if (Current[i] < 0)
            {
                Current[i] = -Current[i];
                charging = FALSE;
            }
            else
                charging = TRUE;
        }
    }
    return charging;
}

