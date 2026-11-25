# Mini Soccer Robot

ESP32 firmware for a ~3 kg Bluetooth gamepad controlled soccer robot with differential drive and a solenoid kicker.

<!-- TODO: add demo GIF/photo -->

## Overview

This robot was used by team **R2C-Tenda** (Robotics Research Center, Universitas Kristen Satya Wacana), which won a **Silver medal at FIRA RoboWorld Cup 2025** in the mini soccer category.

The robot is driven manually by an operator. A Bluetooth gamepad connects directly to the ESP32 through [Bluepad32](https://github.com/ricardoquesada/bluepad32), and the firmware translates stick and button input into motor PWM and kicker pulses. Most of the code is about driver feel: input shaping, speed modes and fast fixed-angle turns that are hard to do reliably with a thumbstick under match pressure.

## Features

- **Bluetooth gamepad control** through Bluepad32, with connect/disconnect callbacks and multi-controller slots.
- **Arcade-style differential drive**: left stick Y sets forward speed, right stick X sets turn rate, mixed into left/right wheel commands.
- **Exponential input shaping**: stick values are normalized and squared (sign preserved) so small stick movements give fine control and full deflection gives full speed. Values near the end of travel snap to full scale.
- **Speed modes**
  - R1: boost to full PWM (255) with rumble feedback and a magenta controller LED.
  - L1: precision mode, speed clamped to 40 with a yellow LED.
  - Default: cyan LED, forward speed capped at 140 and turn rate at 120.
- **Timed turn macros**: Triangle (~180°), Circle (~90°), Square (~-90°), implemented as non-blocking `millis()` timers at full turn PWM. Each macro is edge-triggered so holding the button does not retrigger it.
- **Spin in place**: L2 / R2 force full-speed rotation in either direction.
- **Kicker**: X fires a 50 ms pulse on the kicker output.

## Architecture

```
Gamepad --(Bluetooth)--> ESP32 (Bluepad32)
                           |
                           |-- processGamepad(): input shaping, modes, turn macros
                           |
                           |-- Motion(speed, turn) --> PWM --> H-bridge --> L/R motors
                           |
                           '-- GPIO 33 (50 ms pulse) --> kicker driver
```

### Drive mixing

`Motion(speed, turnspeed)` in `Motion.h`:

- If `speed == 0`, the robot spins in place: one side forward, the other backward, at `|turnspeed|`.
- Otherwise, `left = |speed| + turn` and `right = |speed| - turn`, clamped to ±255, applied on the forward or backward pins depending on the sign of `speed`.

## Hardware and Software Stack

| Item | Details |
| --- | --- |
| MCU | ESP32 (Arduino core with Bluepad32 board package) |
| Input | Bluetooth gamepad supported by Bluepad32 (button mapping uses PlayStation-style names) |
| Drive | 2 DC motors, dual H-bridge driven by 4 PWM pins |
| Kicker | Digital output to a kicker driver |
| Language | Arduino C++ |

Pin map (`Global.h`):

| Signal | GPIO |
| --- | --- |
| RIGHT_FORWARD | 14 |
| RIGHT_BACKWARD | 27 |
| LEFT_FORWARD | 25 |
| LEFT_BACKWARD | 26 |
| KICKER | 33 |

## Controls

| Input | Action |
| --- | --- |
| Left stick Y | Forward / backward |
| Right stick X | Turn |
| R1 (hold) | Boost (full speed) |
| L1 (hold) | Precision (slow) |
| L2 / R2 | Spin in place |
| Triangle | Turn ~180° |
| Circle | Turn ~90° |
| Square | Turn ~-90° |
| X | Kick |

Turn macro angles are open-loop (time based) and were tuned on the robot.

## Repository Structure

```
mini-soccer-robot/
├── mini-soccer-robot.ino   # setup/loop, gamepad processing, modes and turn macros
├── Global.h                # pin map, timer state, Bluepad32 callbacks
└── Motion.h                # differential drive mixing and input mapping functions
```

## Build and Upload

1. Install the Arduino IDE (or arduino-cli).
2. Add the Bluepad32 ESP32 board package. See the [Bluepad32 Arduino guide](https://bluepad32.readthedocs.io/en/latest/plat_arduino/).
3. Open `mini-soccer-robot.ino`, select an ESP32 board from the Bluepad32 package, and upload.
4. Open the serial monitor at 115200 baud and put the gamepad in pairing mode. Pairing keys are cleared on every boot (`BP32.forgetBluetoothKeys()`).

## My Role

I (Moses Jaguar Ersaptiarso) wrote this firmware as part of R2C, covering motion control, gamepad input handling and kicker control, and iterated on the input mapping and turn macros with the team's drivers.
