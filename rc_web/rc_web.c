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

typedef enum {
    NO_ERR,
    ERR_ARG_DIR,
    ERR_NO_ARG
} err_enum;

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
    //_Bool errA = FALSE, errF = FALSE;
    int errF = NO_ERR;

    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
    if (argc > 1)
    {
        dir = atoi(argv[1]);
        if (dir < 0 || dir > 4)
        {
            errF = ERR_ARG_DIR;
        }
    }
    else
    {
        errF = ERR_NO_ARG;
    }
    if (errF)
    {
        printf("usage %s [dir]\n", argv[0]);
        printf("valid dir:\n0 - forward\n1 - backward\n2 - right\n3 - left\n4 - stop\n");
        printf("ERROR: ");
        if (errF == ERR_ARG_DIR) printf("Direction must be 0 through 4\n");
        else printf("Nothing entered\n");
        return EXIT_FAILURE;
    }
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
    for (i = 0; i < 4; i++)
    {
        pullUpDnControl(batLedPins[i], PUD_UP);
        digitalWrite(batLedPins[i], HIGH);
        pinMode(batLedPins[i], OUTPUT);
    }
    if (checkBat())
    {
        digitalWrite(batLedPins[LED_BAT_RT], LOW);
        digitalWrite(batLedPins[LED_BAT_RB], LOW);
        return EXIT_FAILURE;
    }
    digitalWrite(remotePowPin, HIGH);
    delay(2000);
    if (dir != 4)
    {
        while (send_I2C_Command(GO_CENTER) != CENTER_)
            ;
        checkBat();
        switch (dir)
        {
            case MOVE_F: case MOVE_B: result_run = move(dir); break;
            case MOVE_L: result_run = turn(GO_LEFT); break;
            case MOVE_R: result_run = turn(GO_RIGHT); break;
            default: break;
        }
        if (result_run != OK_)
            digitalWrite(PCF_8574 + LED_RED_B, LOW);
    }
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
    while (1)
    {
        result_write = write(i2c_file_ar, cmd, 1);
        if (result_write == 1)
            break;
    }
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

    d += 21;
    if (d != REMOTE_FORWARD && d != REMOTE_BACKWARD)
        return -1;
    if (d == REMOTE_FORWARD)
    {
        f_or_b = TIMMER_F;
        fbPin = remoteForPin;
        sonarCmd = CHECK_SONAR_CENTER;
        ledColor = LED_GREEN_B;
    }
    else
    {
        f_or_b = TIMMER_B;
        fbPin = remoteRevPin;
        sonarCmd = CHECK_SONAR_REAR;
        ledColor = LED_RED_B;
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
        l_or_r = TIMMER_R;
        sonarCmd = CHECK_SONAR_RIGHT;
        thresh = FAR_RIGHT_;
        ledColor = LED_BLUE_B;
    }
    else
    {
        l_or_r = TIMMER_L;
        sonarCmd = CHECK_SONAR_LEFT;
        thresh = FAR_LEFT_;
        ledColor = LED_YELLOW_B;
    }
    lastMillisSonar = 0;
    digitalWrite(PCF_8574 + ledColor, LOW);
    while (send_I2C_Command(d) != thresh)
        ;
    digitalWrite(PCF_8574 + LED_GREEN_B, LOW);
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
    digitalWrite(PCF_8574 + LED_GREEN_B, HIGH);
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
    if (perc <= 80.0) digitalWrite(batLedPins[LED_BAT_YT], LOW);
    if (perc <= 70.0) digitalWrite(batLedPins[LED_BAT_YB], LOW);
    if (perc <= 60.0) digitalWrite(batLedPins[LED_BAT_RT], LOW);
    if (perc <= 50.0) digitalWrite(batLedPins[LED_BAT_RB], LOW);
    return (perc < 20);
}

static void die(int sig)
{
    int i;

    digitalWrite(remotePowPin, LOW);
    digitalWrite(remoteForPin, HIGH);
    digitalWrite(remoteRevPin, HIGH);
    for (i = 0; i < 9; i++) digitalWrite(PCF_8574 + i, HIGH);
    for (i = 0; i < 4; i++) digitalWrite(batLedPins[i], HIGH);
    close(i2c_file_ar);
    close(i2c_file_es);
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    exit(0);
}

