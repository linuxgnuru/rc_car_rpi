#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <errno.h>
#include <signal.h>

#include "rc_header.h"

//                           123456789012345678
//const char fileName[19] = "/mnt/USB/robot.txt";
const char fileName[10] = "robot.txt";
FILE *fp = NULL;

static void die(int sig);

int readFile();

int commandList[2][20];
_Bool fileErr = TRUE;

int main(int argc, char **argv)
{
  int cnt = 0;

  //if (argc > 1) { }
  // note: we're assuming BSD-style reliable signals here
  (void)signal(SIGINT, die);
  (void)signal(SIGHUP, die);
  (void)signal(SIGTERM, die);
  (void)signal(SIGABRT, die);
  cnt = readFile();
  if (!fileErr)
  {
    printf("cnt: %d\n", cnt);
  }
  return EXIT_SUCCESS;
} // end main

static void die(int sig)
{
  if (fp != NULL)
    fclose(fp);
  if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
  if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
  exit(0);
}

int readFile()
{
  _Bool a_b = TRUE;
  _Bool first = TRUE;
  int numA = 0;
  int numB = 0;
  int cnt = 0;
  char c;

  if (access(fileName, F_OK) != -1)
  {
    if ((fp = fopen(fileName, "r")) != NULL)
    {
      while (! feof(fp) && cnt < 20) // only read first 20 lines
      {
        c = fgetc(fp);
        if (c == '\n')
        {
          // TODO skip first line
          if (numB != 0)
          {
            printf("A: %d B: %d\n", numA, numB);
            commandList[0][cnt] = numA;
            commandList[1][cnt] = numB * 1000;
            first = a_b = TRUE;
            cnt++;
          }
        }
        if (c == 32 && first)
        {
          first = a_b = FALSE;
        }
        if (c > 47 && c < 58) 
        {
          if (a_b) numA = c - 48;
          else numB = c - 48;
        }
      } // end while
    } // end fopen
    fclose(fp);
    // return true if no error
    fileErr = FALSE;
  } // end if access
  else
    fileErr = TRUE;
  return cnt;
}

