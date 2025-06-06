#define MOTOR_ENABLE 32

#define RIGHT_FORWARD 27   //19
#define RIGHT_BACKWARD 14  //18
#define LEFT_BACKWARD 26   //16
#define LEFT_FORWARD 25    //17
#define KICKER 33

void dumpGamepad(ControllerPtr ctl) {
  // Serial.printf(
  // "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
  // "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
  // ctl->index(),        // Controller Index
  // ctl->dpad(),         // D-pad
  // ctl->buttons(),      // bitmask of pressed buttons
  // ctl->axisX(),        // (-511 - 512) left X Axis
  // ctl->axisY(),        // (-511 - 512) left Y axis
  // ctl->axisRX(),       // (-511 - 512) right X axis
  // ctl->axisRY(),       // (-511 - 512) right Y axis
  // ctl->brake(),        // (0 - 1023): brake button
  // ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
  // ctl->miscButtons(),  // bitmask of pressed "misc" buttons
  // ctl->gyroX(),        // Gyro X
  // ctl->gyroY(),        // Gyro Y
  // ctl->gyroZ(),        // Gyro Z
  // ctl->accelX(),       // Accelerometer X
  // ctl->accelY(),       // Accelerometer Y
  // ctl->accelZ()        // Accelerometer Z
  // );
}

void AllMotor_FORWARD () {
  analogWrite(RIGHT_FORWARD, 100);
  analogWrite(LEFT_FORWARD, 100);
  analogWrite(RIGHT_BACKWARD, 0);
  analogWrite(LEFT_BACKWARD, 0);
}

void AllMotor_BACKWARD () {
  analogWrite(RIGHT_FORWARD, 0);
  analogWrite(LEFT_FORWARD, 0);
  analogWrite(RIGHT_BACKWARD, 100);
  analogWrite(LEFT_BACKWARD, 100);
}