const byte anaPin = A1;

const byte E1 = 5;
const byte I1 = 2;
const byte I2 = 3;

const byte sonarPins[NUM_SONAR][2] = {
  // output  input
  // trigger echo
  // white   brown
  {  9,      8 }, // left sensor
  { A2,     A3 }, // center sensor
  { A0,     12 }, // right sensor
  { 11,     10 }  // rear sensor
};

