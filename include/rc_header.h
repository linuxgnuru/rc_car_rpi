#ifndef	TRUE
#  define	TRUE	(1==1)
#  define	FALSE	(1==2)
#endif

#define ADDRESS_AR 0x04
#define ADDRESS_ES 0X36
#define ADDRESS_8574 0x20

typedef enum {
  BAD_,               //  0
  OK_,                //  1
  CHECK_SONAR,        //  2 return 0-4 0 = ok, 1-3 left, center, right
  CHECK_SONAR_LEFT,   //  3 return if left sonar is seeing something
  CHECK_SONAR_CENTER, //  4 return if center sonar is seeing something
  CHECK_SONAR_RIGHT,  //  5 return if right sonar is seeing something
  CHECK_SONAR_REAR,   //  6 return if right sonar is seeing something
  GO_RIGHT,           //  7 try to go right; return 0 if not ok 1 if ok
  GO_CENTER,          //  8 try to go to the center; return 0 if not ok 1 if ok
  GO_LEFT,            //  9 try to go left; return 0 if not ok 1 if ok
  GET_DIR,            // 10 return where the servo is, left, center, or right
  
  SONAR_LEFT,         // 11
  SONAR_CENTER,       // 12
  SONAR_RIGHT,        // 13
  SONAR_REAR,         // 14

  NOCHANGE_,          // 15
  RIGHT_,             // 16
  LEFT_,              // 17
  CENTER_,            // 18
  FAR_RIGHT_,         // 19
  FAR_LEFT_,          // 20

  REMOTE_FORWARD,     // 21
  REMOTE_BACKWARD,    // 22
  REMOTE_STOP,        // 23
  REMOTE_POWER,       // 24

  VOLTAGE,            // 25
  CURRENT,            // 26
  PERCENTAGE,         // 27
  TEMPERATURE,        // 28

<<<<<<< HEAD
  MOVE_F = 0,         // 29
  MOVE_B,             // 30
  MOVE_R,             // 31
  MOVE_L,             // 32

  LED_BLUE_B = 0,     // 0
  LED_BLUE_S,         // 34
  LED_GREEN_B,        // 35
  LED_GREEN_S,        // 36
  LED_YELLOW_B,       // 37
  LED_YELLOW_S,       // 38
  LED_RED_S,          // 39
  LED_RED_B,           // 40

  LED_BAT_RT = 0,
  LED_BAT_RB,
  LED_BAT_YT,
  LED_BAT_YB,

  TIMMER_F = 0,
  TIMMER_B,
  TIMMER_R,
  TIMMER_L

=======
  MOVE_F = 21,        // 21
  MOVE_B,             // 22
  MOVE_R = 7,         //  7
  MOVE_L = 9,         //  9

  LED_BLUE_B = 0,     //  0
  LED_BLUE_S,         //  1
  LED_GREEN_B,        //  2
  LED_GREEN_S,        //  3
  LED_YELLOW_B,       //  4
  LED_YELLOW_S,       //  5
  LED_RED_S,          //  6
  LED_RED_B,          //  7

  LED_BAT_YT = 0,     //  0
  LED_BAT_YB,         //  1
  LED_BAT_RT,         //  2
  LED_BAT_RB,         //  3

  TIMMER_F = 0,       //  0
  TIMMER_B,           //  1
  TIMMER_R,           //  2
  TIMMER_L            //  3

>>>>>>> 8675f812e0eb6114ff61efa20e033284c0824f19
} my_enum;

const int remotePowPin = 1;
const int remoteForPin = 4;
const int remoteRevPin = 5;

// XXX the following pins are "reversed" i.e. to turn on you put pin LOW
// 0 = 3 = yellow top
// 1 = 2 = yellow bottom
// 2 = 0 = red top
// 3 = 7 = red bottom
const int batLedPins[4] = { 3, 2, 0, 7 };

#define MX_ 6
const int GPIOpins[MX_] = { 3, 2, 0, 7, 4, 5 };

static const char *devName = "/dev/i2c-1";
