#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include <unistd.h> // for check_root
//#include <sys/ioctl.h>
//#include <fcntl.h>

// for die / signal trapping
#include <errno.h>
#include <signal.h>

#include <wiringPi.h>

#include "man_rc.h"

#define MAX_MILLI 500

const int remotePowPin = 25;
const int remoteForPin = 24;
const int remoteRevPin = 23;

//unsigned long lastMillisMovement[4] = { 0, 0, 0, 0 };
unsigned long lmf = 0;
unsigned long lmr = 0;

const int dur = 2000;

void f();
void r();

static void die(int sig)
{
    digitalWrite(remoteForPin, HIGH);
    digitalWrite(remoteRevPin, HIGH);
    digitalWrite(remotePowPin, LOW);
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    exit(0);
}

int main(int argc, char **argv)
{
    int dir = -1;
    _Bool errF = FALSE;

    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
    if (argc > 1)
    {
        dir = atoi(argv[1]);
        if (dir < 0 || dir > 1)
        {
            errF = TRUE;
            printf("invalid direction (0 or 1)\n");
        }
    }
    else
        errF = TRUE;
    if (errF)
    {
        printf("usage %s [dir] [duration]\n", argv[0]);
        printf("duration in milliseconds (no less than %d)\n", 500);
        printf("valid dir:\n0 - forward\n1 - backward\n");
        return EXIT_FAILURE;
    }
    piHiPri(99);
    pinMode(remotePowPin, OUTPUT);
    pinMode(remoteForPin, PUD_UP);
    digitalWrite(remoteForPin, HIGH);
    pinMode(remoteForPin, OUTPUT);
    pinMode(remoteRevPin, PUD_UP);
    digitalWrite(remoteRevPin, HIGH);
    pinMode(remoteRevPin, OUTPUT);
    digitalWrite(remotePowPin, HIGH);
    switch (dir)
    {
        case 0:
            printf("forward\n");
            f();
            break;
        case 1:
            printf("backward\n");
            r();
            break;
        default:
            break;
    }
    digitalWrite(remoteForPin, HIGH);
    digitalWrite(remoteRevPin, HIGH);
    digitalWrite(remotePowPin, LOW);
    return EXIT_SUCCESS;
} // end main

void f()
{
    unsigned long currentMillis;

    digitalWrite(remoteForPin, LOW);
    while (1)
    {
        currentMillis = millis();
        if (currentMillis - lmf >= dur)
        {
            lmf = currentMillis;
            break;
        }
    } // end while
    digitalWrite(remoteForPin, HIGH);
}

void r()
{
    unsigned long currentMillis;

    //printf("[%s]\n", __func__);
    //printf("f_or_b: %d\n", f_or_b);
    //printf(" fbPin: %d\n", fbPin);
    digitalWrite(remoteRevPin, LOW);
    while (1)
    {
        currentMillis = millis();
        // after dur seconds stop going XXXward
        if (currentMillis - lmr >= dur)
        {
            lmr = currentMillis;
            break;
        }
    } // end while
    digitalWrite(remoteRevPin, HIGH);
}

