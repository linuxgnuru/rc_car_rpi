#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <signal.h>

#include <string.h> // for fprintf
//#include <unistd.h> // for uid_t

#include <wiringPi.h>

#include "rc_header.h"

static void die(int sig);

int main(int argc, char **argv)
{
  int i;

  // note: we're assuming BSD-style reliable signals here
  (void)signal(SIGINT, die);
  (void)signal(SIGHUP, die);
  (void)signal(SIGTERM, die);
  (void)signal(SIGABRT, die);
  if (wiringPiSetup() == -1)
  {
    fprintf(stdout, "[%d] [%s] [%s] Error trying to setup wiringPi - oops: %s\n", __LINE__, __FILE__, __func__, strerror(errno));
    exit(1);
  }
  for (i = 0; i < MX_; i++)
  {
    // first setup pins to be pull up so that when they are turned to OUTPUT,
    // the pins will be HIGH not LOW
    pullUpDnControl(GPIOpins[i], PUD_UP);
    digitalWrite(GPIOpins[i], HIGH);
    pinMode(GPIOpins[i], OUTPUT);
  }
  pinMode(remotePowPin, OUTPUT);
  return EXIT_SUCCESS;
}

static void die(int sig)
{
  if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
  if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
  exit(0);
}

