#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <wiringPi.h>
#include <pcf8574.h>

#include "rc_header.h"

#define PCF_8574 100
#define MAX_MILLI 500

int i2c_file_ar;
int i2c_file_es;
_Bool ledState[8] = { FALSE };
unsigned long lastMillisSonar = 0;
unsigned long lastMillisMovement[4] = { 0, 0, 0, 0 };
const int durration = 2500;
static void die(int sig);

int send_I2C_Command(int u_cmd);
int turn(int d);
int move(int d);
_Bool checkBat();

int main(int argc, char **argv)
{
    int i, dir = -1, result_run = -1;
    _Bool errF = FALSE;

    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
    if (argc > 1)
    {
        dir = atoi(argv[1]);
        if (dir < 0 || dir > 3)
            errF = TRUE;
    }
    else
        errF = TRUE;
    if (errF)
    {
        printf("usage %s [dir]\n", argv[0]);
        printf("valid dir:\n0 - forward\n1 - backward\n2 - right\n3 - left\n");
        return EXIT_FAILURE;
    }
    dir += 29;
    if ((i2c_file_ar = open(devName, O_RDWR)) < 0)     { fprintf(stderr, "[Arduino] I2C: Failed to access %s\n", devName); exit(1); }
    if (ioctl(i2c_file_ar, I2C_SLAVE, ADDRESS_AR) < 0) { fprintf(stderr, "[Arduino] I2C: Failed to acquire bus access/talk to slave 0x%x\n", ADDRESS_AR); exit(1); }
    if ((i2c_file_es = open(devName, O_RDWR)) < 0)     { fprintf(stderr, " [EnergyShield] I2C: Failed to access %s\n", devName); exit(1); }
    if (ioctl(i2c_file_es, I2C_SLAVE, ADDRESS_ES) < 0) { fprintf(stderr, " [EnergyShield] I2C: Failed to acquire bus access/talk to slave 0x%x\n", ADDRESS_ES); exit(1); }
    if (wiringPiSetup() == -1)                         { fprintf(stdout, " Error trying to setup wiringPi - oops: %s\n", strerror(errno)); exit(1); }
    if (pcf8574Setup(PCF_8574, ADDRESS_8574) != 1)     { fprintf(stdout, " Error trying to setup pcf8574 - oops: %s\n", strerror(errno)); exit(1); }
    piHiPri(99);
    pinMode(remoteForPin, PUD_UP);
    digitalWrite(remoteForPin, HIGH);
    pinMode(remoteForPin, OUTPUT);
    pinMode(remoteRevPin, PUD_UP);
    digitalWrite(remoteRevPin, HIGH);
    pinMode(remoteRevPin, OUTPUT);
    pinMode(remotePowPin, OUTPUT);
    digitalWrite(remotePowPin, HIGH);
    delay(2000);
    for (i = 0; i < 4; i++)
    {
        pullUpDnControl(batLedPins[i], PUD_UP);
        digitalWrite(batLedPins[i], HIGH);
        pinMode(batLedPins[i], OUTPUT);
    }
    if (checkBat())
    {
        digitalWrite(batLedPins[2], LOW);
        digitalWrite(batLedPins[3], LOW);
        return EXIT_FAILURE;
    }
    while (send_I2C_Command(GO_CENTER) != CENTER_)
        ;
    checkBat();
    switch (dir)
    {
        case MOVE_F: case MOVE_B: result_run = move((dir == MOVE_F ? REMOTE_FORWARD : REMOTE_BACKWARD)); break;
        case MOVE_L: case MOVE_R: result_run = turn((dir == MOVE_R ? GO_RIGHT : GO_LEFT)); break;
        default: break;
    }
    if (result_run != OK_)
        digitalWrite(PCF_8574 + (LED_RED_B - 33), LOW);
    close(i2c_file_ar);
    close(i2c_file_es);
    digitalWrite(remotePowPin, LOW);
    digitalWrite(remoteForPin, HIGH);
    digitalWrite(remoteRevPin, HIGH);
    for (i = 0; i < 9; i++)
        digitalWrite(PCF_8574 + i, HIGH);
    for (i = 0; i < 4; i++)
        digitalWrite(batLedPins[i], HIGH);
    return EXIT_SUCCESS;
}

int send_I2C_Command(int u_cmd)
{
    unsigned char cmd[16];
    int result_write = 1, result_read = 1, ret = BAD_;
    cmd[0] = u_cmd;
    while (1) { result_write = write(i2c_file_ar, cmd, 1); if (result_write == 1) break; }
    usleep(1000);
    if (result_write == 1)
    {
        char buf[1];
        while (1)
        {
            result_read = read(i2c_file_ar, buf, 1);
            if (result_read == 1)
                break;
        }
        if (result_read == 1)
            ret = (int)buf[0];
    }
    return ret;
}

int move(int d)
{
    int sonarCmd, ledColor, f_or_b, fbPin, Ret = OK_;
    unsigned long currentMillis;

    if (d != REMOTE_FORWARD && d != REMOTE_BACKWARD)
        return -1;
    if (d == REMOTE_FORWARD)
    {
        f_or_b = 0;
        fbPin = remoteForPin;
        sonarCmd = CHECK_SONAR_CENTER;
        ledColor = LED_GREEN_B - 33;
    }
    else
    {
        f_or_b = 1;
        fbPin = remoteRevPin;
        sonarCmd = CHECK_SONAR_REAR;
        ledColor = LED_RED_B - 33;
    }
    if (send_I2C_Command(sonarCmd) == OK_)
    {
        lastMillisSonar = 0;
        digitalWrite(PCF_8574 + ledColor, LOW);
        digitalWrite(fbPin, LOW);
        while (1)
        {
            currentMillis = millis();
            if (currentMillis - lastMillisMovement[f_or_b] >= durration)
            {
                lastMillisMovement[f_or_b] = currentMillis;
                break;
            }
            if (currentMillis - lastMillisSonar >= 5)
            {
                lastMillisSonar = currentMillis;
                if (send_I2C_Command(sonarCmd) != OK_)
                {
                    Ret = d;
                    break;
                }
            }
        }
        digitalWrite(fbPin, HIGH);
        digitalWrite(PCF_8574 + ledColor, HIGH);
    }
    return Ret;
}

int turn(int d)
{
    int sonarCmd, thresh, ledColor, l_or_r, Ret = OK_;
    unsigned long currentMillis;

    if (d != GO_RIGHT && d != GO_LEFT)
        return -1;
    if (d == GO_RIGHT)
    {
        l_or_r = 2;
        sonarCmd = CHECK_SONAR_RIGHT;
        thresh = FAR_RIGHT_;
        ledColor = LED_BLUE_B - LED_BLUE_B;
    }
    else
    {
        l_or_r = 3;
        sonarCmd = CHECK_SONAR_LEFT;
        thresh = FAR_LEFT_;
        ledColor = LED_YELLOW_B - LED_BLUE_B;
    }
    lastMillisSonar = 0;
    digitalWrite(PCF_8574 + ledColor, LOW);
    while (send_I2C_Command(d) != thresh)
        ;
    digitalWrite(PCF_8574 + (LED_GREEN_B - LED_BLUE_B), LOW);
    digitalWrite(remoteForPin, LOW);
    while (1)
    {
        currentMillis = millis();
        if (currentMillis - lastMillisMovement[l_or_r] >= durration)
        {
            lastMillisMovement[l_or_r] = currentMillis;
            Ret = OK_;
            break;
        }
        if (currentMillis - lastMillisSonar >= 5)
        {
            lastMillisSonar = currentMillis;
            if (send_I2C_Command(sonarCmd) != OK_)
            {
                Ret = d;
                break;
            }
        }
    }
    digitalWrite(remoteForPin, HIGH);
    digitalWrite(PCF_8574 + (LED_GREEN_B - LED_BLUE_B), HIGH);
    digitalWrite(PCF_8574 + ledColor, HIGH);
    while (send_I2C_Command(GO_CENTER) != CENTER_)
        ;
    return Ret;
}

_Bool checkBat()
{
    unsigned char cmd[16];
    char buf[1];
    char p = 0;
    float perc = 0;

    cmd[0] = 0x02;
    if (write(i2c_file_es, cmd, 1) == 1)
    {
        usleep(10000);
        if (read(i2c_file_es, buf, 1) == 1)
            p = buf[0];
        perc = (float)p / 2;
    }
    if (perc <= 80.0) digitalWrite(batLedPins[0], LOW);
    if (perc <= 70.0) digitalWrite(batLedPins[1], LOW);
    if (perc <= 60.0) digitalWrite(batLedPins[2], LOW);
    if (perc <= 50.0) digitalWrite(batLedPins[3], LOW);
    return (perc < 20);
}

static void die(int sig)
{
    int i;
    digitalWrite(remotePowPin, LOW);
    for (i = 0; i < 9; i++) digitalWrite(PCF_8574 + i, HIGH);
    for (i = 0; i < 4; i++) digitalWrite(batLedPins[i], HIGH);
    digitalWrite(remoteForPin, HIGH);
    digitalWrite(remoteRevPin, HIGH);
    close(i2c_file_ar);
    close(i2c_file_es);
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    exit(0);
}

