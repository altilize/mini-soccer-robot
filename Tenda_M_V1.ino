#define MOTOR_ENABLE 32

#define RIGHT_FORWARD 25
#define RIGHT_BACKWARD 26
#define LEFT_BACKWARD 27
#define LEFT_FORWARD 14

#define KICKER 33

#include <Bluepad32.h>
#include "Motion.h"
#include "Debug.h"

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

unsigned long prevMillis;
// unsigned long currentMillis;

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

// ======================= GAME CONTROLLER ACTIONS SECTION ======================= //
void processGamepad(ControllerPtr ctl) {

  bool L2pressed = ctl->buttons() == 0x0040;
  bool R2pressed = ctl->buttons() == 0x0080;
  bool kicker = ctl->a();
  int turnSpeed = 0;
  int joystickY_value = ctl->axisY();
  int speed = 0;
  bool boostLEFT = ctl->l1();
  bool boostRIGHT = ctl->r1();
  bool boostUP = ctl->y();

  speed = mixedCubicMapping(joystickY_value);

  if (!L2pressed && !L2pressed && !boostUP) {
    ctl->setColorLED(128, 128, 128);
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
  if (kicker) {
    digitalWrite(KICKER, HIGH);
    delay(5);
    digitalWrite(KICKER, LOW);
    Serial.print("KICK");
  }


  if (boostUP) {
    unsigned long currentMillis = millis();
    if (currentMillis - prevMillis >= 8000) {
      ctl->playDualRumble(0, 250, 0x80, 0x40);
      Serial.println("GETAR");
      prevMillis = currentMillis;
    }
    ctl->setColorLED(255, 0, 0);
    Serial.println("UP BOOST");
    speed = 255;
  }

  if (boostRIGHT) {
    Serial.println("RIGHT BOOST");
    turnSpeed = 180;
  }

  if (boostLEFT) {
    ctl->setColorLED(255, 0, 0);
    Serial.println("LEFT BOOST");
    turnSpeed = -180;
  }


  Serial.print("Speed : ");
  Serial.print(speed);
  Serial.print("Turn : ");
  Serial.print(turnSpeed);
  Serial.print(" axis Y : ");
  Serial.println(ctl->axisY());
  Motion(speed, turnSpeed);

  dumpGamepad(ctl);
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
  if (dataUpdated)
    processControllers();


  delayMicroseconds(20);
}