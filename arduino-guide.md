# Arduino UNO R4 WiFi — First-Time Guide & Reference

This is our cheat sheet for the actual Arduino board — what to install, how to talk to it, and where the official docs live (the Arduino site is a bit of a maze, so these are direct links to the exact pages we need).

## First-time setup

1. **Download the Arduino IDE** (the software you write and upload code from) from arduino.cc/software — get IDE 2.x, the current version.
2. **Install the IDE** on the computer like any other program.
3. **Install the board package.** Open the IDE, click "Boards Manager" in the left sidebar, search for **"UNO R4 WiFi"**, and install it. This is a one-time step — it teaches the IDE how to talk to this specific board.
4. **Plug in the board** via its USB-C cable.
5. **Select the board.** In the board selector dropdown (top of the IDE), the UNO R4 WiFi should now appear — select it, and select the matching COM/serial port.
6. **Upload a test sketch.** File → Examples has several built-in examples that need no extra libraries. Start with **Blink** (makes the onboard LED flash) to confirm everything's working: click the checkmark to compile, then the right-arrow to upload.

### Fun fact: it already does something out of the box
The board ships pre-loaded with a Tetris animation that plays on its built-in LED matrix. Uploading your own sketch will overwrite it — if you want it back later, the restore code is on the [Getting Started page](https://docs.arduino.cc/tutorials/uno-r4-wifi/r4-wifi-getting-started) (search "Tetris Animation Sketch").

## The power rule (important — read before wiring any servo)

> **Never power a servo motor from the Arduino's 5V pin.**

Official guidance from Arduino: the 5V pin is fed by the board's onboard regulator, which also powers the microcontroller and its own components. Servo motors draw current in on/off bursts as they move, and pulling that through the regulator can brown out the board (resets, glitches, weird behaviour).

Instead:
- Servo **signal** wire → an Arduino digital pin
- Servo **power (red)** wire → the external battery pack, directly
- Servo **ground** wire → tied together with the battery pack's ground, and *that* joint connects to the Arduino's **GND** pin (this "common ground" is what lets the signal timing and the power stay in sync)

Extra detail if useful later: the board can be powered itself via VIN (6–24V, e.g. the barrel jack) or via USB-C (5V only). When powered via VIN, the 5V pin can supply up to 1.2A through the onboard regulator; via USB, up to 2A. Either way, that's the board's own budget, not something to route servo current through.

## Pinout — what to plug in where

Full official pin table (Arduino UNO R4 WiFi):

| Pin | Type | Notes |
|---|---|---|
| D0 | Digital | UART Receive (avoid for general use) |
| D1 | Digital | UART Transmit (avoid for general use) |
| D2 | Digital | GPIO, interrupt |
| D3 | Digital | GPIO, interrupt, **PWM** |
| D4 | Digital | GPIO |
| D5 | Digital | GPIO, **PWM** |
| D6 | Digital | GPIO, **PWM** |
| D7 | Digital | GPIO |
| D8 | Digital | GPIO |
| D9 | Digital | GPIO, **PWM** |
| D10 | Digital | SPI (CS), GPIO, **PWM** |
| D11 | Digital | SPI (COPI), GPIO, **PWM** |
| D12 | Digital | SPI (CIPO), GPIO |
| D13 | Digital | SPI (SCK), GPIO, built-in LED |
| A0–A5 | Analog | Analog in; A4/A5 double as I2C (SDA/SCL) |

**Officially-supported PWM pins: D3, D5, D6, D9, D10, D11.** For our 4 leg servos, a good pin choice is **D9, D10, D11, D6** (all PWM-capable, and D9–D11 are free of any other special function). Note: the Arduino `Servo` library actually works on *any* digital pin (it uses timer interrupts, not true PWM), so this isn't a hard requirement — but sticking to these pins keeps us clear of SPI/UART/I2C pins we might want later.

**I2C pins (for a future LCD eye screen): A4 (SDA) and A5 (SCL).**

## Known gotcha: servo jitter on the R4

A few people online have reported servos twitching/jumping unexpectedly on the UNO R4 (different timer hardware to older Uno boards). If we see this:
- Make sure the servo's ground is genuinely common with the battery's ground (the #1 cause)
- There's a community library, `VarSpeedServoRA4M1`, written specifically to smooth this out on the R4 — worth trying if the standard `Servo` library misbehaves

## Minimal example: sweep one servo

```cpp
#include <Servo.h>

Servo legServo;

void setup() {
  legServo.attach(9); // signal wire on D9
}

void loop() {
  legServo.write(45);   // move to 45 degrees
  delay(500);
  legServo.write(135);  // move to 135 degrees
  delay(500);
}
```

## Built-in LED matrix (for early "face" experiments)

The board has a **12×8 built-in LED matrix** — no extra hardware needed to try a simple robot "face" before wiring up separate eyes.

```cpp
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

const uint32_t frames[][4] = {
  { 0x0, 0x0, 0xc00c0000, 150 },   // frame 1
  { 0x0, 0x1e01, 0x201201e0, 150 } // frame 2
};

void setup() {
  matrix.begin();
  matrix.loadSequence(frames);
  matrix.play(true);
}

void loop() {}
```

## Key reference links

- [Getting Started with UNO R4 WiFi](https://docs.arduino.cc/tutorials/uno-r4-wifi/r4-wifi-getting-started) — IDE + board package install (source for the setup steps above)
- [UNO R4 WiFi User Manual / Cheat Sheet](https://docs.arduino.cc/tutorials/uno-r4-wifi/cheat-sheet) — the master reference: full pinout, power supply rules, PWM, I2C, SPI, LED matrix, Bluetooth/WiFi (source for the pinout + power info above)
- [UNO R4 WiFi datasheet (PDF)](https://docs.arduino.cc/resources/datasheets/ABX00087-datasheet.pdf) — official electrical specs, for when we need exact numbers
- [Using the LED Matrix](https://docs.arduino.cc/tutorials/uno-r4-wifi/led-matrix) — full guide to the built-in display
- [Arduino UNO R4 WiFi Pinout Reference (Last Minute Engineers)](https://lastminuteengineers.com/arduino-uno-r4-wifi-pinout-reference/) — a more visual/annotated pinout diagram, handy alongside the official table
- [Arduino Servo library reference](https://arduinogetstarted.com/reference/library/arduino-servo-library) — the `Servo.h` functions used above
- [Arduino UNO R4 Servo Motor tutorial (newbiely.com)](https://newbiely.com/tutorials/arduino-uno-r4/arduino-uno-r4-servo-motor) — worked example specific to this board
- [VarSpeedServoRA4M1 library (GitHub)](https://github.com/KaledSouky/VarSpeedServoRA4M1) — fallback if servo jitter shows up
- Arduino Forum threads on R4 servo quirks, if needed: ["Trouble with Servos on R4 Wifi"](https://forum.arduino.cc/t/trouble-with-servos-on-r4-wifi/1151749), ["Fixing Jumping UNO R4 Servos" (DigiKey forum)](https://forum.digikey.com/t/fixing-jumping-arduino-uno-r4-servos-for-smooth-motion/50873)

## Later: parts to research for the ambitious extras
- Small OLED/LCD (I2C, e.g. SSD1306) for eyes
- Piezo speaker (simple beeps via `tone()`) or DFPlayer Mini MP3 module (recorded voice clips) for the "robot voice"
