#define MOTOR_ENABLE 32

#define RIGHT_FORWARD 25
#define RIGHT_BACKWARD 26
#define LEFT_BACKWARD 27
#define LEFT_FORWARD 14

#define KICKER 33



unsigned long prevMillis;
unsigned long getarMillis;
bool getar_flag = false;

#include <Bluepad32.h>
#include "Motion.h"