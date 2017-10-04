#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Needed for I2C port
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

// for die / signal trapping
#include <errno.h>
#include <signal.h>

#include <wiringPi.h>

#include "force_center.h"

#define ADDRESS 0x04

static const char *devName = "/dev/i2c-1";
int i2c_file;
//int length;
//unsigned char buffer[60] = {0};

static void die(int sig);
_Bool checkRoot();

int sendCommand(int u_cmd);

int main(int argc, char **argv)
{
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
    if ((i2c_file = open(devName, O_RDWR)) < 0)
    {
        fprintf(stderr, "[%d] [%s] [%s] I2C: Failed to access %s\n", __LINE__, __FILE__, __func__, devName);
        exit(1);
    }
    if (ioctl(i2c_file, I2C_SLAVE, ADDRESS) < 0)
    {
        fprintf(stderr, "[%d] [%s] [%s] I2C: Failed to acquire bus access/talk to slave 0x%x\n", __LINE__, __FILE__, __func__, ADDRESS);
        exit(1);
    }
    while (sendCommand(GO_CENTER) != CENTER_)
       ;
    close(i2c_file);
    return EXIT_SUCCESS;
} // end main

int sendCommand(int u_cmd)
{
    unsigned char cmd[16];
    int ret = -1;
    int rs_w = 1;
    int rs_r = 1;

    cmd[0] = u_cmd;
    // FIXME XXX TODO XXX FIXME
    // WTF!!  WHY IS THIS XXX IN A WHILE LOOP?!?!?!?!?!
    while (1)
    {
        rs_w = write(i2c_file, cmd, 1);
        if (rs_w == 1)
            break;
    } // end while
    usleep(10000);
    if (rs_w == 1)
    {
        char buf[1];
        while (1)
        {
            rs_r = read(i2c_file, buf, 1);
            if (rs_r == 1)
                break;
        } // end while
        if (rs_r == 1)
        {
            ret = (int)buf[0];
        }
        else
        {
            printf("failed to read from the i2c bus.\n");
            printf("length: %d\n", rs_r);
        }
        // now wait else you could crash the arduino by sending requests too fast
        usleep(10000);
    }
    else
    {
        printf("failed to write to the i2c bus.\n");
        printf("length: %d\n", rs_w);
    }
    usleep(10000);
    return ret;
}

static void die(int sig)
{
    close(i2c_file);
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    exit(0);
}

// check if you are root
_Bool checkRoot()
{
    uid_t uid = getuid();
    uid_t euid = geteuid();
    return (uid != 0 || uid != euid);
}

