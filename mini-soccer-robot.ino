#include "Global.h"

// ----------------- Main -------------------
void processGamepad(ControllerPtr ctl) {
  bool L1pressed = ctl->buttons() == 0x0010;
  bool R1pressed = ctl->buttons() == 0x0020;
  bool L2pressed = ctl->buttons() == 0x0040;
  bool R2pressed = ctl->buttons() == 0x0080;
  bool Xpressed = ctl->buttons() == 0x0001;
  bool TrianglePressed = ctl->buttons() == 0x0008;
  bool SquarePressed = ctl->buttons() == 0x0004;
  bool CirclePressed = ctl->buttons() == 0x0002;



  int joystickY_value = ctl->axisY();
  int joystickX_value = ctl->axisRX();
  int joystickRY_value = ctl->axisRY();

  int speed = 0;
  int turnSpeed = 0;



  turnSpeed = exponentialMapping(joystickX_value, 80.0);
  speed = exponentialMapping(joystickY_value, 140.0) * -1;

  if (!L1pressed && !R1pressed) {
    ctl->setColorLED(0, 204, 204);
  }

  // ------------ L2 Pressed ------------------
  if (L2pressed) {
    if (speed == 0) {
      turnSpeed = -80;
    }

    if (speed != 0 && (speed <= 10 && speed >= -10)) {
      turnSpeed = -40;
      Serial.println("L2 Kecil");
    }

    if ((speed >= 10 && speed <= 120) || (speed <= -10 && speed >= -120)) {
      turnSpeed = -50;
      Serial.println("L2 Gede Aja");
    }

    if (speed > 120 || speed < -120) {
      turnSpeed = -120;
      Serial.println("L2 Gede Bgt");
    }
  }

  // -------------- R2 Pressed --------------
  if (R2pressed) {
    if (speed == 0) {
      turnSpeed = 80;
    }
    if (speed != 0 && (speed <= 10 && speed >= -10)) {
      turnSpeed = 40;
      Serial.println("R2 Kecil");
    }

    if ((speed >= 10 && speed <= 120) || (speed <= -10 && speed >= -120)) {
      turnSpeed = 50;
      Serial.println("R2 Gede Aja");
    }

    if (speed > 120 || speed < -120) {
      turnSpeed = 120;
      Serial.println("R2 Gede Bgt");
    }
  }

  if (Xpressed) {
    digitalWrite(KICKER, HIGH);
    delay(50);
    digitalWrite(KICKER, LOW);
    Serial.print("KICK");
  }
  // ------------ SKILL -----------------------
  // boost
  if (speed != 0 && R1pressed) {
    ctl->setColorLED(255, 0, 127);
    if (getarMillis <= 250 && !getar_flag) {
      ctl->playDualRumble(0, 500, 0x80, 0x40);
      getarMillis = millis();
      getar_flag = true;
    } else {
      getarMillis = 0;
      ctl->playDualRumble(0, 0, 0, 0);
    }

    if (speed < 0) {
      speed = -255;  // Boosting
      Serial.println("BOOST BACK");
    } else {
      speed = 255;  // Boosting
      Serial.println("BOOST FWD");
    }
    turnSpeed = exponentialMapping(joystickX_value, 100.0);

  } else {
    getar_flag = false;
  }

  //  reduce
  if (speed != 0 && L1pressed) {
    ctl->setColorLED(255, 255, 0);

    if (speed < 0) {
      speed = -20;  // reduce

    } else {
      speed = 20;
      Serial.println("REDUCE FRWD");
    }
  }

  // --------------- Putar 180 ----------------
  if (TrianglePressed && !turnStarted && !isTurning) {
    isTurning = true;
    turnStarted = true;
    turnMillis = millis();
    Serial.println("PUTAR");
  }

  if (isTurning) {
    if (millis() - turnMillis <= 150) {
      turnSpeed = 255;
    } else {
      isTurning = false;
      Serial.println("STOP PUTAR");
    }
  }
  // reset
  if (!TrianglePressed) {
    turnStarted = false;
  }

  // ------------- Putar 90 ---------------------
  if (CirclePressed && !turn90started && !isTurning90) {
    isTurning90 = true;
    turn90started = true;
    turn90Millis = millis();
    Serial.println("PUTAR 90");
  }
  if (isTurning90) {
    if (millis() - turn90Millis <= 100) {
      turnSpeed = 255;
    } else {
      isTurning90 = false;
      Serial.println("STOP PUTAR");
    }
  }
  // reset
  if (!CirclePressed) {
    turn90started = false;
  }

  // -------------- Putar -90 -------------------------- //
  if (SquarePressed && !turn90started_reverse && !isTurning90_reverse) {
    isTurning90_reverse = true;
    turn90started_reverse = true;
    turn90Millis_reverse = millis();
    Serial.println("PUTAR -90");
  }
  if (isTurning90_reverse) {
    if (millis() - turn90Millis_reverse <= 80) {
      turnSpeed = -255;
    } else {
      isTurning90_reverse = false;
      Serial.println("STOP PUTAR");
    }
  }
  // reset
  if (!SquarePressed) {
    turn90started_reverse = false;
  }


  Motion(speed, turnSpeed);

  Serial.print("Speed : ");
  Serial.print(speed);
  Serial.print("Turn : ");
  Serial.print(turnSpeed);
  Serial.print(" axis Y : ");
  Serial.print(ctl->axisY());
  Serial.print(" axis RY : ");
  Serial.println(ctl->axisRY());

  // dumpGamepad(ctl); // for debugging
}

// ---------------- Which controller do u use? ----------------
void processControllers() {
  for (auto myController : myControllers) {
    if (myController && myController->isConnected() && myController->hasData()) {
      if (myController->isGamepad()) {
        processGamepad(myController);
      } else {
        Serial.println("Unsupported controller");
      }
    }
  }
}


void setup() {
  Serial.begin(115200);
  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  const uint8_t* addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.forgetBluetoothKeys();
  BP32.enableVirtualDevice(false);

  pinMode(RIGHT_FORWARD, OUTPUT);
  pinMode(RIGHT_BACKWARD, OUTPUT);
  pinMode(LEFT_FORWARD, OUTPUT);
  pinMode(LEFT_BACKWARD, OUTPUT);
  pinMode(KICKER, OUTPUT);

  pinMode(MOTOR_ENABLE, OUTPUT);
  digitalWrite(MOTOR_ENABLE, HIGH);

  startMillis = millis();
}

void loop() {
  bool dataUpdated = BP32.update();
  if (dataUpdated) {
    processControllers();
  }
  delayMicroseconds(20);
}