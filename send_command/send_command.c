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

#include "send_command.h"

#define ADDRESS 0x04

#define MAX_WAIT 30000

static const char *devName = "/dev/i2c-1";
int i2c_file;
int length;
unsigned char buffer[60] = {0};

static void die(int sig);
_Bool checkRoot();

int sendCommand(int u_cmd);
void showCmd(int u_cmd, _Bool printCommand);
void showResult(int u_res, _Bool printResult);

int convert_cmd(int cmd);

int conv_u(int cm);

int main(int argc, char **argv)
{
    int cmd_ = -1;
    int u_cmd;
    //int t_cmd;

    // note: we're assuming BSD-style reliable signals here
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
    /*
    if (checkRoot())
    {
        printf("this must be run as root\ni.e.: \nsudo %s\n", argv[0]);
        return EXIT_FAILURE;
    }
    */
    if (argc > 1)
    {
        cmd_ = atoi(argv[1]);
        if (cmd_ < 0 || cmd_ > 8)
            cmd_ = -1;
    }
    if (cmd_ == -1)
    {
        printf("usage %s number\n", argv[0]);
        printf("valid commands:\n");
        printf("0 CHECK SONAR\n");
        printf("1 CHECK SONAR LEFT\n");
        printf("2 CHECK SONAR CENTER\n");
        printf("3 CHECK SONAR RIGHT\n");
        printf("4 CHECK SONAR REAR\n");
        printf("5 GO RIGHT\n");
        printf("6 GO CENTER\n");
        printf("7 GO LEFT\n");
        printf("8 GET DIR\n");
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
    if (cmd_ > 4 && cmd_ < 8)
    {
        //printf("cmd_: %d\n", cmd_);
        u_cmd = convert_cmd(cmd_);
        switch (u_cmd)
        {
            case GO_RIGHT:
                while (sendCommand(GO_RIGHT) != FAR_RIGHT_)
                    ;
                break;
            case GO_CENTER:
                while (sendCommand(GO_CENTER) != CENTER_)
                    ;
                break;
            case GO_LEFT:
                while (sendCommand(GO_LEFT) != FAR_LEFT_)
                    ;
                break;
            default:
                //printf("default - %d\n", u_cmd);
                break;
        }
        //printf("%d\n", u_cmd);
    }
    else
    {
        u_cmd = convert_cmd(cmd_);
        showResult(sendCommand(u_cmd), 0);
    }
    close(i2c_file);
    return EXIT_SUCCESS;
} // end main

int sendCommand(int u_cmd)
{
    unsigned char cmd[16];
    int ret = -1;
    int rs_w = 1;
    int rs_r = 1;

    //cmd[0] = convert_cmd(u_cmd);
    cmd[0] = u_cmd;
    //showCmd(cmd[0], TRUE);
    // FIXME XXX TODO XXX FIXME
    // WTF!!  WHY IS THIS XXX IN A WHILE LOOP?!?!?!?!?!
    while (1)
    {
        rs_w = write(i2c_file, cmd, 1);
        if (rs_w == 1)
            break;
    } // end while (1)
    usleep(10000);
    if (rs_w == 1)
    {
        char buf[1];
        while (1)
        {
            rs_r = read(i2c_file, buf, 1);
            if (rs_r == 1)
                break;
        } // end while (1)
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
        //usleep(MAX_WAIT);
    }
    else
    {
        printf("failed to write to the i2c bus.\n");
        printf("length: %d\n", rs_w);
    }
    //usleep(MAX_WAIT); // is this needed?
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

void showCmd(int u_cmd, _Bool printCommand)
{
    switch (u_cmd)
    {
        case CHECK_SONAR:
            if (printCommand) printf("[%d] ", u_cmd);
            printf("CHECK SONAR\n");
            break;
        case CHECK_SONAR_LEFT:
            if (printCommand) printf("[%d] ", u_cmd);
            printf("CHECK SONAR LEFT\n");
            break;
        case CHECK_SONAR_CENTER:
            if (printCommand) printf("[%d] ", u_cmd);
            printf("CHECK SONAR CENTER\n");
            break;
        case CHECK_SONAR_RIGHT:
            if (printCommand) printf("[%d] ", u_cmd);
            printf("CHECK SONAR RIGHT\n");
            break;
        case CHECK_SONAR_REAR:
            if (printCommand) printf("[%d] ", u_cmd);
            printf("CHECK SONAR REAR\n");
            break;
        case GO_RIGHT:
            if (printCommand) printf("[%d] ", u_cmd);
            printf("GO RIGHT\n");
            break;
        case GO_CENTER:
            if (printCommand) printf("[%d] ", u_cmd);
            printf("GO CENTER\n");
            break;
        case GO_LEFT:
            if (printCommand) printf("[%d] ", u_cmd);
            printf("GO LEFT\n");
            break;
        case GET_DIR:
            if (printCommand) printf("[%d] ", u_cmd);
            printf("GET DIR\n");
            break;
        default:
            if (printCommand) showResult(u_cmd, TRUE);
            else printf("unknown command [%d]\n", u_cmd);
            break;
    }
}

int convert_cmd(int cmd)
{
    int r;

    switch (cmd)
    {
        case 0: r = CHECK_SONAR; break;
        case 1: r = CHECK_SONAR_LEFT; break;
        case 2: r = CHECK_SONAR_CENTER; break;
        case 3: r = CHECK_SONAR_RIGHT; break;
        case 4: r = CHECK_SONAR_REAR; break;
        case 5: r = GO_RIGHT; break;
        case 6: r = GO_CENTER; break;
        case 7: r = GO_LEFT; break;
        case 8: r = GET_DIR; break;
        default: r = BAD_; break;
    }
    return r;
}

void showResult(int u_res, _Bool printResult)
{
    if (printResult)
        printf("[%d] ", u_res);
    switch (u_res)
    {
        case OK_: printf("ok\n"); break;
        case BAD_: printf("bad\n"); break;
        case SONAR_LEFT: printf("left sonar hit something\n"); break;
        case SONAR_CENTER: printf("center sonar hit something\n"); break;
        case SONAR_RIGHT: printf("right sonar hit something\n"); break;
        case SONAR_REAR: printf("rear sonar hit something\n"); break;
        case NOCHANGE_: printf("position no change\n"); break;
        case RIGHT_: printf("position right\n"); break;
        case CENTER_: printf("position center\n"); break;
        case LEFT_: printf("position left\n"); break;
        case FAR_RIGHT_: printf("position far right\n"); break;
        case FAR_LEFT_: printf("position far left\n"); break;
        case REMOTE_FORWARD: printf("remote forward\n"); break;
        case REMOTE_BACKWARD: printf("remote backward\n"); break;
        case REMOTE_STOP: printf("remote stop\n"); break;
        case REMOTE_POWER: printf("remote power\n"); break;
        default: printf("unknown [%d]\n", u_res); break;
    } // end switch
}

int conv_u(int cm)
{
    int ret = BAD_;

    switch (cm)
    {
        case GO_RIGHT: ret = FAR_RIGHT_; break;
        case GO_CENTER: ret = CENTER_; break;
        case GO_LEFT: ret = FAR_LEFT_; break;
        default: printf("\n-def--\n"); ret = cm; break;
    }
    return ret;
}

