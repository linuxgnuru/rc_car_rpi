#ifndef	TRUE
#  define	TRUE	(1==1)
#  define	FALSE	(1==2)
#endif

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

  MOVE_F,             // 29
  MOVE_B,             // 30
  MOVE_R,             // 31
  MOVE_L,             // 32

  LED_BLUE_B,         // 33
  LED_BLUE_S,         // 34
  LED_GREEN_B,        // 35
  LED_GREEN_S,        // 36
  LED_YELLOW_B,       // 37
  LED_YELLOW_S,       // 38
  LED_RED_S,          // 39
  LED_RED_B           // 40
} my_enum;

/*
// for last millis array
typedef enum {
  LAST_F,
  LAST_B,
  LAST_R,
  LAST_L
} mili_enum;
*/

/*
typedef enum {
  YEL_TOP,
  YEL_BOT,
  RED_TOP,
  RED_BOT
} battery_leds_enum;
*/
