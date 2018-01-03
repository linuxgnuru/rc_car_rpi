#define SLAVE_ADDRESS 0x04

#define NUM_SONAR 4

#define TRIG 0
#define ECHO 1

typedef enum {
  BAD_,               //  0
  OK_,                //  1
  CHECK_SONAR,        //  2 return 0-4 0 = ok, 1-3 left, center, right
  CHECK_SONAR_LEFT,   //  3 return if left sonar is seeing something
  CHECK_SONAR_CENTER, //  4 return if center sonar is seeing something
  CHECK_SONAR_RIGHT,  //  5 return if right sonar is seeing something
  CHECK_SONAR_REAR,   //  6 return if rear sonar is seeing something
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
  FAR_LEFT_           // 20
} my_enum;

const int waitDelay = 1000;
const int emergDelay = 2000;

unsigned long lastMillis = 0;
unsigned long lastMillisBreak = 0;
unsigned long lastMillisEmerg[6] = {
  0, 0, 0, 0, 0, 0
};

boolean runOnce = true;

int curDir = CENTER_;

// ranges for left and right:
//const float vFarLeftMax = 4.0;

volatile int command;
volatile int data = OK_;

