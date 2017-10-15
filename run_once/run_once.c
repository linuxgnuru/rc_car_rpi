/*
 * run_once.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// for I2C
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

// for die / signal trapping
#include <errno.h>
#include <signal.h>

#include <wiringPi.h>
#include <pcf8574.h>

#include "run_once.h"

#define ADDRESS_AR 0x04
#define ADDRESS_ES 0X36
#define ADDRESS_8574 0x20

#define PCF_8574 100

#define MAX_MILLI 500

const int remotePowPin = 25;
const int remoteForPin = 24;
const int remoteRevPin = 23;

// XXX the following pins are "reversed" i.e. to turn on you put pin LOW
const int batLedPins[4] = { 3, 2, 0, 7 };

//#define USE_CAMERA

//////////////////////
_Bool checkRoot();

static void die(int sig);

int send_I2C_Command(int u_cmd);

int turn(int d, int dur);
int move(int d, int dur);

_Bool checkBat(_Bool prt);
float ESgetData(int my_cmd, _Bool prt);

void printEnum(int, _Bool); // from libmyenum.a

//////////////////////

static const char *devName = "/dev/i2c-1";
// had to make i2c_file_* global so die could close it
int i2c_file_ar;
int i2c_file_es;

_Bool printF = TRUE;

_Bool ledState[8] = { FALSE };

// FIXME might need to make Sonar the same 4 array
unsigned long lastMillisSonar = 0;
unsigned long lastMillisMovement[4] = { 0, 0, 0, 0 };

int main(int argc, char **argv)
{
    int i;
    int ti;
    int result_from_running = -1;
    int dir, dur;
    _Bool errF = FALSE;
    _Bool testLed = FALSE;

    dir = dur = -1;
    // note: we're assuming BSD-style reliable signals here
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
    if (checkRoot())
    {
        printf("this must be run as root\ni.e.: \nsudo %s\n", argv[0]);
        return EXIT_FAILURE;
    }
    if (argc > 2)
    {
        if (argc > 3)
        {
            if (strcmp(argv[3], "noprint") == 0)
                printF = FALSE;
        }
        dir = atoi(argv[1]);
        if (dir < 0 || dir > 3)
        {
            errF = TRUE;
            printf("invalid direction\n");
        }
        dur = atoi(argv[2]);
        if (dur < 500)
        {
            errF = TRUE;
            printf("invalid duration\n");
        }
    }
    else
    {
        errF = TRUE;
        printf("no arguments\n");
    }
    if (errF)
    {
        printf("usage %s [dir] [duration] (noprint)\n", argv[0]);
        printf("optional:\nnoprint - disables printing of messages\n");
        printf("duration in milliseconds (no less than %d)\n", MAX_MILLI);
        printf("valid dir:\n0 - forward\n1 - backward\n2 - right\n3 - left\n");
        return EXIT_FAILURE;
    }

    dir += 29; // fix dir input into enum
    // For Arduino
    if ((i2c_file_ar = open(devName, O_RDWR)) < 0)
    {
        fprintf(stderr, "[Arduino] I2C: Failed to access %s\n", devName);
        exit(1);
    }
    if (ioctl(i2c_file_ar, I2C_SLAVE, ADDRESS_AR) < 0)
    {
        fprintf(stderr, "[Arduino] I2C: Failed to acquire bus access/talk to slave 0x%x\n", ADDRESS_AR);
        exit(1);
    }
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
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, " Error trying to setup wiringPi - oops: %s\n", strerror(errno));
        exit(1);
    }
    if (pcf8574Setup(PCF_8574, ADDRESS_8574) != 1)
    {
        fprintf(stdout, " Error trying to setup pcf8574 - oops: %s\n", strerror(errno));
        exit(1);
    }
    // Setup our priority
    piHiPri(99);

    // setup remote control pins
    pinMode(remotePowPin, OUTPUT);

    // make sure the remote isn't sending any command to the car just yet
    pinMode(remoteForPin, PUD_UP);
    digitalWrite(remoteForPin, HIGH);
    pinMode(remoteForPin, OUTPUT);

    pinMode(remoteRevPin, PUD_UP);
    digitalWrite(remoteRevPin, HIGH);
    pinMode(remoteRevPin, OUTPUT);

    // the following sets the pins for output but will start off all HIGH instead of all LOW
    for (i = 0; i < 4; i++)
    {
        pullUpDnControl(batLedPins[i], PUD_UP);
        digitalWrite(batLedPins[i], HIGH);
        pinMode(batLedPins[i], OUTPUT);
    }
    // very first thing we should do is check the battery of the arduino.
    if (checkBat(FALSE))
    {
        //printf("Can not continue; Arduino battery is too low (%0.2f%%)\n", bat_per);
        printf("Can not continue; Arduino battery is below 20%%\n");
        digitalWrite(batLedPins[2], LOW);
        digitalWrite(batLedPins[3], LOW);
        return EXIT_FAILURE;
    }
    // Test leds
    if (testLed)
    {
        // test battery status LEDs
        for (i = 0; i < 4; i++)
        {
            digitalWrite(batLedPins[i], LOW);
            delay(250);
        }
        for (i = 0; i < 4; i++)
        {
            digitalWrite(batLedPins[i], HIGH);
            delay(250);
        }
        // test directional LEDs
        for (i = 0; i < 8; i++)
        {
            ti = i;
            if (ti == 7)
                ti = 6;
            else if (i == 6)
                ti = 7;
            digitalWrite(PCF_8574 + ti, LOW);
            delay(250);
        }
        for (i = 7; i > -1; i--)
        {
            ti = i;
            if (ti == 7)
                ti = 6;
            else if (i == 6)
                ti = 7;
            digitalWrite(PCF_8574 + ti, HIGH);
            delay(250);
        }
    } // end testLed

    // Center the front wheels
    while (send_I2C_Command(GO_CENTER) != CENTER_)
        ;
    checkBat(TRUE);
    // turn on remote control
    digitalWrite(remotePowPin, HIGH);
    switch (dir)
    {
        case MOVE_F: case MOVE_B:
            if (printF)
            {
                printf("for/rev: ");
                printEnum(dir, TRUE);
                //if (dir == MOVE_F) printf("%d: forward\n", dir);
                //else printf("%d: backward\n", dir);
            }
            result_from_running = move((dir == MOVE_F ? REMOTE_FORWARD : REMOTE_BACKWARD), dur);
#ifdef USE_CAMERA
            //system("/usr/bin/raspistill -o /mnt/USB/pics/pic.jpg -w 640 -h 320 -n");
#endif
            break;
        case MOVE_L: case MOVE_R:
            if (printF)
            {
                printf("left/right: ");
                printEnum(dir, TRUE);
                //if (dir == MOVE_R) printf("%d: right\n", dir);
                //else printf("%d: left\n", dir);
            }
            result_from_running = turn((dir == MOVE_R ? GO_RIGHT : GO_LEFT), dur);
#ifdef USE_CAMERA
            //system("/usr/bin/raspistill -o /mnt/USB/pics/pic.jpg -w 640 -h 320 -n");
#endif
            break;
        default:
            // should never get here
            if (printF) printf("default: [%d]\n", dir);
            break;
    }
    // turn off remote control
    digitalWrite(remotePowPin, LOW);
    if (result_from_running != OK_)
    {
        digitalWrite(PCF_8574 + (LED_RED_B - 33), LOW);
        if (printF)
        {
            //printf("[%d]\n", result_from_running);
            printf("result (not OK_): ");
            printEnum(result_from_running, TRUE);
            //printf("error: %d\n", result_from_running);
        }
    }
    close(i2c_file_ar);
    close(i2c_file_es);
    digitalWrite(remoteForPin, HIGH);
    digitalWrite(remoteRevPin, HIGH);
    digitalWrite(remotePowPin, LOW);
    for (i = 0; i < 9; i++)
        digitalWrite(PCF_8574 + i, HIGH);
    for (i = 0; i < 4; i++)
        digitalWrite(batLedPins[i], HIGH);
    return EXIT_SUCCESS;
} // end main

/********************
 * send_I2C_Command *
 ********************/
int send_I2C_Command(int u_cmd)
{
    unsigned char cmd[16];
    int ret = BAD_;
    int result_write, result_read;

    result_write = result_read = 1;
    cmd[0] = u_cmd;
    while (1)
    {
        result_write = write(i2c_file_ar, cmd, 1);
        if (result_write == 1)
            break;
    }
    // wait for message to be sent and data gathered
    // originally usleep(1000);
    delay(1);
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
    } // end write
    return ret;
} // end send_I2C_Command

/********
 * move *
 ********/
//  FIXME Try to combine move and turn into one function?
int move(int d, int dur)
{
    int sonarCmd;
    int ledColor;
    int f_or_b;
    int fbPin;
    int Ret = OK_;

    unsigned long currentMillis;

    // if direction isn't forward or reverse; just return
    if (d != REMOTE_FORWARD && d != REMOTE_BACKWARD)
        return -1;
    if (d == REMOTE_FORWARD)
    {
        //f_or_b = LAST_F;
        f_or_b = 0;
        fbPin = remoteForPin;
        sonarCmd = CHECK_SONAR_CENTER;
        ledColor = LED_GREEN_B - 33;
    }
    else
    {
        //f_or_b = LAST_B;
        f_or_b = 1;
        fbPin = remoteRevPin;
        sonarCmd = CHECK_SONAR_REAR;
        ledColor = LED_RED_B - 33;
    }
    if (printF)
    {
        printf("f_or_b: %d\n", f_or_b);
        printf("fbPin: %d\n", fbPin);
        printf("sonarCmd: ");
        printEnum(sonarCmd, TRUE);
        printf("ledColor: ");
        printEnum(ledColor + 33, TRUE);
    }
    // test if something is in the way before we even try to move
    if (send_I2C_Command(sonarCmd) == OK_)
    {
        lastMillisSonar = 0;
        digitalWrite(PCF_8574 + ledColor, LOW);
        digitalWrite(fbPin, LOW);
        while (1)
        {
            currentMillis = millis();
            // after dur seconds stop going XXXward
            if (currentMillis - lastMillisMovement[f_or_b] >= dur)
            {
                lastMillisMovement[f_or_b] = currentMillis;
                break;
            }
            // only check sonar every 5 milliseconds
            if (currentMillis - lastMillisSonar >= 5)
            {
                lastMillisSonar = currentMillis;
                // something is in the way and can't go XXXward
                if (send_I2C_Command(sonarCmd) != OK_)
                {
                    Ret = d;
                    break;
                } // end sonar test
            } // end sonar millis
        } // end while
        digitalWrite(fbPin, HIGH);
        digitalWrite(PCF_8574 + ledColor, HIGH);
    }
    return Ret;
} // end move

/********
 * turn *
 ********/
int turn(int d, int dur)
{
    int sonarCmd;
    int thresh;
    int ledColor;
    int l_or_r;
    int Ret = OK_;

    unsigned long currentMillis;

    if (d != GO_RIGHT && d != GO_LEFT)
        return -1;
    if (d == GO_RIGHT)
    {
        //l_or_r = LAST_R;
        l_or_r = 2;
        sonarCmd = CHECK_SONAR_RIGHT;
        thresh = FAR_RIGHT_;
        ledColor = LED_BLUE_B - 33;
    }
    else
    {
        //l_or_r = LAST_L;
        l_or_r = 3;
        sonarCmd = CHECK_SONAR_LEFT;
        thresh = FAR_LEFT_;
        ledColor = LED_YELLOW_B - 33;
    }
    if (printF)
    {
        printf("l_or_r: %d\n", l_or_r);
        printf("thresh: ");
        printEnum(thresh, TRUE);
        printf("sonarCmd: ");
        printEnum(sonarCmd, TRUE);
        printf("ledColor: ");
        printEnum(ledColor + 33, TRUE);
    }
    // test if something is in the way before we even try to turn
    if (send_I2C_Command(sonarCmd) == OK_)
    {
        lastMillisSonar = 0;
        // maybe easy way
        // try to turn wheel turn right for dur seconds
        digitalWrite(PCF_8574 + ledColor, LOW);
        while (send_I2C_Command(d) != thresh)
            ;
        digitalWrite(PCF_8574 + (LED_GREEN_B - 33), LOW);
        // go forward
        digitalWrite(remoteForPin, LOW);
        while (1)
        {
            currentMillis = millis();
            if (currentMillis - lastMillisMovement[l_or_r] >= dur)
            {
                lastMillisMovement[l_or_r] = currentMillis;
                break;
            }
            // only check sonar every 5 milliseconds
            if (currentMillis - lastMillisSonar >= 5)
            {
                lastMillisSonar = currentMillis;
                if (send_I2C_Command(sonarCmd) != OK_)
                {
                    Ret = d;
                    break;
                } // end sonar test
            } // end sonar millis
        } // end while 1
        digitalWrite(remoteForPin, HIGH);
        digitalWrite(PCF_8574 + (LED_GREEN_B - 33), HIGH);
        digitalWrite(PCF_8574 + ledColor, HIGH);
        // TODO might put move to center in forward or backward?
        // go back to center
        while (send_I2C_Command(GO_CENTER) != CENTER_)
            ;
    } // end sonar check
    else // can't go right / left
        Ret = (d == GO_RIGHT ? RIGHT_ : LEFT_);
    return Ret;
} // end turn

/************
 * checkBat *
 ************/
_Bool checkBat(_Bool prt)
{
    float bat_per = 0.0;

    bat_per = ESgetData(PERCENTAGE, prt);
    /*
    if (bat_per <= 80.0) digitalWrite(batLedPins[YEL_TOP], LOW);
    if (bat_per <= 70.0) digitalWrite(batLedPins[YEL_BOT], LOW);
    if (bat_per <= 60.0) digitalWrite(batLedPins[RED_TOP], LOW);
    if (bat_per <= 50.0) digitalWrite(batLedPins[RED_BOT], LOW);
    */
    if (bat_per <= 80.0) digitalWrite(batLedPins[0], LOW);
    if (bat_per <= 70.0) digitalWrite(batLedPins[1], LOW);
    if (bat_per <= 60.0) digitalWrite(batLedPins[2], LOW);
    if (bat_per <= 50.0) digitalWrite(batLedPins[3], LOW);
    return (bat_per < 20);
} // end checkBat

/*************
 * ESgetData *
 *************/
// for arduino battery
float ESgetData(int my_cmd, _Bool prt)
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
                if (prt && printF) printf("Voltage: %ld\n", V);
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
                if (prt && printF) printf("Current: %d mA\n", Iraw);
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
                if (prt && printF) printf("Percentage: %0.2f%%\n", perc);
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
                if (prt && printF) printf("Temperature: %0.2f C\n", Temp);
                ret = Temp;
            }
            break;
        default: break;
    }
    return ret;
} // end ESgetData

/*************
 * checkRoot *
 *************/
// check if you are root
_Bool checkRoot()
{
    uid_t uid = getuid();
    uid_t euid = geteuid();
    return (uid != 0 || uid != euid);
}

/*******
 * die *
 *******/
static void die(int sig)
{
    int i;

    for (i = 0; i < 9; i++)
        digitalWrite(PCF_8574 + i, HIGH);
    for (i = 0; i < 4; i++)
        digitalWrite(batLedPins[i], HIGH);
    digitalWrite(remoteForPin, HIGH);
    digitalWrite(remoteRevPin, HIGH);
    digitalWrite(remotePowPin, LOW);
    close(i2c_file_ar);
    close(i2c_file_es);
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    exit(0);
} // end die

