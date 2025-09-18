#include "Global.h"

// ----------------- Main -------------------
void processGamepad(ControllerPtr ctl) {
  bool L1pressed = ctl->l1();
  bool R1pressed = ctl->r1();
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



  turnSpeed = exponentialMapping(joystickX_value, 120.0);
  speed = exponentialMapping(joystickY_value, 140.0) * -1;

  if (!L1pressed && !R1pressed) {
    ctl->setColorLED(0, 204, 204);
  }

  // ------------ L2 Pressed ------------------
  if (L2pressed) turnSpeed = -255;



  // -------------- R2 Pressed --------------
  if (R2pressed) turnSpeed = 255;

  if (Xpressed) {
    digitalWrite(KICKER, HIGH);
    delay(50);
    digitalWrite(KICKER, LOW);
  }


  // ==================== SKILL =================== //
  // ---------------- boost -------------------- //
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
      speed = -255;
    } else {
      speed = 255;
    }
    turnSpeed = exponentialMapping(joystickX_value, 100.0);

  } else {
    getar_flag = false;
  }

  //  -------------- reduce ------------------- //
  if (speed != 0 && L1pressed && !R1pressed) {
    ctl->setColorLED(255, 255, 0);

    if (speed < 0) {
      speed = -40;  // reduce

    } else {
      speed = 40;
    }
  }

  // --------------- Putar 180 ----------------
  if (TrianglePressed && !turnStarted && !isTurning) {
    isTurning = true;
    turnStarted = true;
    turnMillis = millis();
  }

  if (isTurning) {
    if (millis() - turnMillis <= 180) {
      turnSpeed = 255;
    } else {
      isTurning = false;
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
    if (millis() - turn90Millis_reverse <= 90) {
      turnSpeed = -255;
    } else {
      isTurning90_reverse = false;
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


  startMillis = millis();
}

void loop() {
  bool dataUpdated = BP32.update();
  if (dataUpdated) {
    processControllers();
  }
  // delayMicroseconds(20);
}