#define MOTOR_ENABLE 32

#define RIGHT_FORWARD 25
#define RIGHT_BACKWARD 26
#define LEFT_BACKWARD 27
#define LEFT_FORWARD 14

#define KICKER 33

unsigned long startMillis;

// ----------- getar ------------------
unsigned long getarMillis;
bool getar_flag = false;

// ------------- turn 180 -----------
unsigned long turnMillis = 0;
bool isTurning = false;
bool turnStarted = false;

// ----------- turn 90 --------------
unsigned long turn90Millis = 0;
bool isTurning90 = false;
bool turn90started = false;

// ----------- turn -90 ------------
unsigned long turn90Millis_reverse = 0;
bool isTurning90_reverse = false;
bool turn90started_reverse = false;


#include <Bluepad32.h>
#include "Motion.h"