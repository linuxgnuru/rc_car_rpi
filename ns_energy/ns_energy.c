#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#include <ncurses.h>

#include <wiringPi.h>

#ifndef	TRUE
#  define	TRUE	(1==1)
#  define	FALSE	(1==2)
#endif

#define NSE_ADDRESS 0x36

typedef enum {
  VOLTAGE,
  CURRENT,
  PERCENTAGE,
  TEMPERATURE
} command_enum;

_Bool isN = FALSE;

const int cols[4] = {
  2, 18, 34, 48
};

unsigned long lastM = 0;

static const char *devName = "/dev/i2c-1";
int i2c_file;

void getData(int my_cmd);

void printTable();
void clearLine(int col);
void runAll();

_Bool checkRoot()
{
  uid_t uid = getuid(), euid = geteuid();
  return (uid != 0 || uid != euid);
}

static void die(int sig)
{
  close(i2c_file);
  if (isN == TRUE) endwin();
  if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
  if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
  exit(0);
}

int main(int argc, char **argv)
{
  int cmd_ = -1;

  // note: we're assuming BSD-style reliable signals here
  (void)signal(SIGINT, die);
  (void)signal(SIGHUP, die);
  (void)signal(SIGTERM, die);
  (void)signal(SIGABRT, die);

  if (checkRoot())
  {
    printf("invalid credentials\nsudo %s\n", argv[0]);
    return EXIT_FAILURE;
  }
  if (argc > 1)
  {
    cmd_ = atoi(argv[1]);
    if (cmd_ < 0 || cmd_ > 4)
      cmd_ = -1;
  }
  if (cmd_ == -1)
  {
    printf("usage %s number\n", argv[0]);
    printf("valid numbers:\n"
        "0 - voltage\n"
        "1 - percentage\n"
        "2 - current\n"
        "3 - temperature\n"
        "4 - ncurses\n"
        );
    return EXIT_FAILURE;
  }
  if ((i2c_file = open(devName, O_RDWR)) < 0)
  {
    fprintf(stderr, "[%d] [%s] [%s] I2C: Failed to access %s\n", __LINE__, __FILE__, __func__, devName);
    exit(1);
  }
  if (ioctl(i2c_file, I2C_SLAVE, NSE_ADDRESS) < 0)
  {
    fprintf(stderr, "[%d] [%s] [%s] I2C: Failed to acquire bus access/talk to slave 0x%x\n", __LINE__, __FILE__, __func__, NSE_ADDRESS);
    exit(1);
  }
  if (cmd_ == 4)
  {
    isN = TRUE;
    initscr();
    if (has_colors() == FALSE)
    {
      endwin();
      fprintf(stdout, "[%d] [%s] [%s] Your terminal does not support color", __LINE__, __FILE__, __func__);
      exit(1);
    }
    use_default_colors();
    // Start color
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    noecho();
    cbreak();
    nodelay(stdscr, true);
    curs_set(0);
    move(0, 1);
    printw("I2C: Acquiring bus to 0x%x", NSE_ADDRESS);
    printTable();
  }
  //if (sendCommand(GET_DIR) != CENTER_) sendCommand(GO_CENTER);
  switch (cmd_)
  {
    case 0: getData(VOLTAGE); break;
    case 1: getData(PERCENTAGE); break;
    case 2: getData(CURRENT); break;
    case 3: getData(TEMPERATURE); break;
    case 4: runAll(); break;
    default: printf("how did you get here?\n"); break;
  }
  if (isN)
    endwin();
  close(i2c_file);
  return 0;
} // end main

void runAll()
{
  int i;
  int key;
  _Bool stopMe = FALSE;
  unsigned long cm;

  while (stopMe == FALSE)
  {
    cm = millis();
    key = getch();
    if (key > -1 && key == 'q')
      stopMe = TRUE;
    if (cm - lastM >= 2000)
    {
      lastM = cm;
      for (i = VOLTAGE; i < TEMPERATURE + 1; i++)
        getData(i);
    }
  }
}

void getData(int my_cmd)
{
  unsigned char cmd[16];
  char HB = 0, LB = 0;
  char p = 0;
  int t = 0;
  int Iraw = 0;
  long V = 0;
  float perc = 0;
  float Temp = 0.0;

  switch (my_cmd)
  {
    case VOLTAGE:
      cmd[0] = 0x0C;
      if (write(i2c_file, cmd, 1) == 1)
      {
        usleep(10000); // wait for messages to be sent
        char buf[1];
        if (read(i2c_file, buf, 1) == 1)
          HB = buf[0];
        if (read(i2c_file, buf, 1) == 1)
          LB = buf[0];
        V = (unsigned long) ((HB << 5) + (LB >> 3)) * 122/100;
        if (isN)
        {
          clearLine(cols[my_cmd]);
          move(5, cols[my_cmd]);
          printw("%ld", V);
        }
        else
          printf("Voltage: %ld\n", V);
      }
      break;
    case CURRENT:
      cmd[0] = 0x0E;
      if (write(i2c_file, cmd, 1) == 1)
      {
        usleep(10000); // wait for messages to be sent
        char buf[1];
        if (read(i2c_file, buf, 1) == 1)
          HB = buf[0];
        if (read(i2c_file, buf, 1) == 1)
          LB = buf[0];
        Iraw = (long) (((HB << 8) + LB) >> 4) * 5 / 4;
        if (isN)
        {
          clearLine(cols[my_cmd]);
          move(5, cols[my_cmd]);
          printw("%d", Iraw);
        }
        else
          printf("Current: %d mA\n", Iraw);
      }
      break;
    case PERCENTAGE:
      cmd[0] = 0x02;
      if (write(i2c_file, cmd, 1) == 1)
      {
        usleep(10000); // wait for messages to be sent
        char buf[1];
        if (read(i2c_file, buf, 1) == 1)
          p = buf[0];
        perc = (float)p / 2;
        if (isN)
        {
          clearLine(cols[my_cmd]);
          move(5, cols[my_cmd]);
          if (perc == 100)
            printw("%0.0f%%", perc);
          else
            printw("%0.2f%%", perc);
        }
        else
        {
          if (perc == 100)
            printf("Percentage: %0.0f%%\n", perc);
          else
            printf("Percentage: %0.2f%%\n", perc);
        }
      }
      break;
    case TEMPERATURE:
      cmd[0] = 0x0A;
      if (write(i2c_file, cmd, 1) == 1)
      {
        usleep(10000); // wait for messages to be sent
        char buf[1];
        if (read(i2c_file, buf, 1) == 1)
          HB = buf[0];
        if (read(i2c_file, buf, 1) == 1)
          LB = buf[0];
        t = HB * 8 + LB / 32;
        Temp = (float) t / 8;
        if (isN)
        {
          clearLine(cols[my_cmd]);
          move(5, cols[my_cmd]);
          printw("%0.2f", Temp);
        }
        else
          printf("Temperature: %0.2f C\n", Temp);
      }
      break;
    default:
      break;
  }
  if (isN)
    refresh();
}

void printTable()
{
  mvaddstr(1, 0, "press 'q' to quit");
  //mvaddstr(3, 2, "Data:");
  //                       1111111111222222222233333333334444444444555555555566666666667
  //---------------234567890123456789012345678901234567890123456789012345678901234567890
  //               Voltage (mV)    Current (mA)    Percentage    Temperature (C)
  mvaddstr(4,  2, "------------    ------------    ----------    ---------------");
  mvaddstr(3,  2, "Voltage (mV)");
  mvaddstr(3, 18, "Current (mA)");
  mvaddstr(3, 34, "Percentage");
  mvaddstr(3, 48, "Temperature (C)");
  refresh();
}

void clearLine(int col)
{
  if (col < 0)
    return;
  mvaddstr(5, cols[col], "             ");
  //refresh();
  move(5, cols[col]);
#if 0
  attron(COLOR_PAIR(1));
  mvprintw(38, 2, "command sent: [%d]", cmd[0]);
  attroff(COLOR_PAIR(1));
#endif
}

