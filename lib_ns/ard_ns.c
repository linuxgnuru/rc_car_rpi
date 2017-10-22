#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// for I2C
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "ard_ns.h"

#define ADDRESS_ES 0X36

#define BATTERY_CAPACITY 1200 // mAh

//_Bool checkBat(_Bool prt);
void ESopen();
void ESclose();
float ESgetData(int);
_Bool TTE();

//void printEnum(int, _Bool); // from libmyenum.a

static const char *devName = "/dev/i2c-1";
int i2c_file_es;
int Current[10];
static _Bool isOpen = FALSE;

void ESopen()
{
    // for energy shield module
    if ((i2c_file_es = open(devName, O_RDWR)) < 0)
    {
        fprintf(stderr, " [EnergyShield] I2C: Failed to access %s\n", devName);
        exit(1);
    }
    if (ioctl(i2c_file_es, I2C_SLAVE, ADDRESS_ES) < 0)
    {
        fprintf(stderr, " [EnergyShield] I2C: Failed to acquire bus access/talk to slave 0x%x\n", ADDRESS_ES);
        exit(1);
    }
    isOpen = TRUE;
}

void ESclose()
{
    isOpen = FALSE;
    close(i2c_file_es);
}

/*************
 * ESgetData *
 *************/
// for arduino battery
float ESgetData(int my_cmd)
{
    unsigned char cmd[16];
    char buf[1];
    char HB, LB, p;
    int t, Iraw;
    long V = 0;
    float perc, Temp, ret;

    HB = LB = p = 0;
    perc = Temp = ret = 0.0;
    t = Iraw = 0;

    switch (my_cmd)
    {
        case VOLTAGE:
            cmd[0] = 0x0C;
            if (write(i2c_file_es, cmd, 1) == 1)
            {
                usleep(10000); // wait for messages to be sent
                if (read(i2c_file_es, buf, 1) == 1) HB = buf[0];
                if (read(i2c_file_es, buf, 1) == 1) LB = buf[0];
                V = (unsigned long) ((HB << 5) + (LB >> 3)) * 122/100;
                //if (prt && printF) printf("Voltage: %ld\n", V);
                ret = (float) V;
            }
            break;
        case CURRENT:
            cmd[0] = 0x0E;
            if (write(i2c_file_es, cmd, 1) == 1)
            {
                usleep(10000); // wait for messages to be sent
                if (read(i2c_file_es, buf, 1) == 1) HB = buf[0];
                if (read(i2c_file_es, buf, 1) == 1) LB = buf[0];
                Iraw = (long) (((HB << 8) + LB) >> 4) * 5 / 4;
                //if (prt && printF) printf("Current: %d mA\n", Iraw);
                ret = (float) Iraw;
            }
            break;
        case PERCENTAGE:
            cmd[0] = 0x02;
            if (write(i2c_file_es, cmd, 1) == 1)
            {
                usleep(10000); // wait for messages to be sent
                if (read(i2c_file_es, buf, 1) == 1) p = buf[0];
                perc = (float)p / 2;
                //if (prt && printF) printf("Percentage: %0.2f%%\n", perc);
                ret = perc;
            }
            break;
        case TEMPERATURE:
            cmd[0] = 0x0A;
            if (write(i2c_file_es, cmd, 1) == 1)
            {
                usleep(10000); // wait for messages to be sent
                if (read(i2c_file_es, buf, 1) == 1) HB = buf[0];
                if (read(i2c_file_es, buf, 1) == 1) LB = buf[0];
                t = HB * 8 + LB / 32;
                Temp = (float) t / 8;
                //if (prt && printF) printf("Temperature: %0.2f C\n", Temp);
                ret = Temp;
            }
            break;
        default: break;
    }
    return ret;
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

/*
    // very first thing we should do is check the battery of the arduino.
    if (checkBat(FALSE))
    {
        //printf("Can not continue; Arduino battery is too low (%0.2f%%)\n", bat_per);
        printf("Can not continue; Arduino battery is below 20%%\n");
        digitalWrite(batLedPins[2], LOW);
        digitalWrite(batLedPins[3], LOW);
        return EXIT_FAILURE;
    }
}
*/

/*
_Bool checkBat(_Bool prt)
{
    float bat_per = 0.0;

    bat_per = ESgetData(PERCENTAGE, prt);
    if (bat_per <= 80.0) digitalWrite(batLedPins[0], LOW);
    if (bat_per <= 70.0) digitalWrite(batLedPins[1], LOW);
    if (bat_per <= 60.0) digitalWrite(batLedPins[2], LOW);
    if (bat_per <= 50.0) digitalWrite(batLedPins[3], LOW);
    return (bat_per < 20);
}
*/
