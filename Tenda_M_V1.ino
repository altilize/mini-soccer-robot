#include "Global.h"

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void dumpGamepad(ControllerPtr ctl) {
  Serial.printf(
    "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
    "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
    ctl->index(),        // Controller Index
    ctl->dpad(),         // D-pad
    ctl->buttons(),      // bitmask of pressed buttons
    ctl->axisX(),        // (-511 - 512) left X Axis
    ctl->axisY(),        // (-511 - 512) left Y axis
    ctl->axisRX(),       // (-511 - 512) right X axis
    ctl->axisRY(),       // (-511 - 512) right Y axis
    ctl->brake(),        // (0 - 1023): brake button
    ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
    ctl->miscButtons(),  // bitmask of pressed "misc" buttons
    ctl->gyroX(),        // Gyro X
    ctl->gyroY(),        // Gyro Y
    ctl->gyroZ(),        // Gyro Z
    ctl->accelX(),       // Accelerometer X
    ctl->accelY(),       // Accelerometer Y
    ctl->accelZ()        // Accelerometer Z
  );
}

void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
      ControllerProperties properties = ctl->getProperties();
      Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id, properties.product_id);
      myControllers[i] = ctl;
      foundEmptySlot = true;
      break;
    }
  }

  if (!foundEmptySlot) {
    Serial.println("CALLBACK: Controller connected, but could not found empty slot");
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  bool foundController = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
      myControllers[i] = nullptr;
      foundController = true;
      break;
    }
  }

  if (!foundController) {
    Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
  }
}

void processGamepad(ControllerPtr ctl) {
  bool L1pressed = ctl->l1();
  bool R1pressed = ctl->r1();
  bool L2pressed = ctl->buttons() == 0x0040;
  bool R2pressed = ctl->buttons() == 0x0080;
  bool Xpressed = ctl->a();
  bool TrianglePressed = ctl->y();



  int joystickY_value = ctl->axisY();
  int joystickX_value = ctl->axisRX();
  int speed = 0;
  int turnSpeed = 0;

  speed = mixedCubicMapping(joystickY_value);
  turnSpeed = mixedCubicMapping(joystickX_value) * -1;

  if (!L2pressed && !R2pressed && !L1pressed && !R1pressed) {
    ctl->setColorLED(1, 8, 79);
  }

  // ------------ L2 Pressed ------------------
  if (L2pressed) {
    ctl->setColorLED(255, 255, 0);
    if (speed == 0) {
      turnSpeed = -170;
    }

    if (speed != 0 && (speed <= 10 && speed >= -10)) {
      turnSpeed = -70;
      Serial.println("L2 Kecil");
    }

    if ((speed >= 10 && speed <= 120) || (speed <= -10 && speed >= -120)) {
      turnSpeed = -80;
      Serial.println("L2 Gede Aja");
    }

    if (speed > 120 || speed < -120) {
      turnSpeed = -150;
      Serial.println("L2 Gede Bgt");
    }
  }

  // -------------- R2 Pressed --------------
  if (R2pressed) {
    ctl->setColorLED(0, 255, 255);
    if (speed == 0) {
      turnSpeed = 170;
    }
    if (speed != 0 && (speed <= 10 && speed >= -10)) {
      turnSpeed = 70;
      Serial.println("R2 Kecil");
    }

    if ((speed >= 10 && speed <= 120) || (speed <= -10 && speed >= -120)) {
      turnSpeed = 80;
      Serial.println("R2 Gede Aja");
    }

    if (speed > 120 || speed < -120) {
      turnSpeed = 150;
      Serial.println("R2 Gede Bgt");
    }
  }

  if (Xpressed) {
    digitalWrite(KICKER, HIGH);
    delay(5);
    digitalWrite(KICKER, LOW);
    Serial.print("KICK");
  }

  // --------- boost --------------
  if (R1pressed) {
    if (getarMillis <= 250 && !getar_flag) {
      ctl->playDualRumble(0, 400, 0x80, 0x40);
      getarMillis = millis();
      getar_flag = true;
    } else {
      getarMillis = 0;
      ctl->playDualRumble(0, 0, 0, 0);
    }
    ctl->setColorLED(255, 0, 0);
    Serial.println("BOOST");
    if (speed < 0) {
      speed = -255;  // Boosting
      Serial.println("BOOST BACK");
    } else {
      speed = 255;  // Boosting
      Serial.println("BOOST FWD");
    }

  } else {
    getar_flag = false;
  }

  //  ------ reduce ----------
  if (L1pressed) {
    ctl->setColorLED(0, 255, 0);

    if (speed < 0) {
      speed = -100;  // reduce

    } else {
      speed = 50;
      Serial.println("REDUCE FRWD");
    }
  }

  Motion(speed, turnSpeed);

  Serial.print("Speed : ");
  Serial.print(speed);
  Serial.print("Turn : ");
  Serial.print(turnSpeed);
  Serial.print(" axis Y : ");
  Serial.println(ctl->axisY());


  // dumpGamepad(ctl);
}

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
}

void loop() {
  bool dataUpdated = BP32.update();
  if (dataUpdated) {
    processControllers();
  }
  delayMicroseconds(20);
}