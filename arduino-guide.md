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

## SG90 servo — datasheet & wire colours

The datasheet is saved locally at [`datasheets/sg90_datasheet.pdf`](datasheets/sg90_datasheet.pdf) (source: [Tower Pro SG90 datasheet, via GitHub](https://github.com/Jason2062/2D-Printer/blob/master/Component%20Datasheets/SG90%20Servo%20Motor%20Datasheet.pdf)).

**Confirmed wire colours (straight from the datasheet's own wiring diagram):**

| Wire colour | Function | Goes to |
|---|---|---|
| Orange | Signal (PWM) | An Arduino digital pin |
| Red | Power (Vcc, +) | The battery pack — **not** the Arduino 5V pin |
| Brown | Ground (–) | Battery pack ground + Arduino GND (all tied together) |

(The other common SG90 colour scheme in the wild is white/red/black, same order: signal/power/ground. If a servo ever turns up with different colours, the middle wire is always power, flanked by signal on one side and ground on the other.)

Genuine SG90s come with a small female 3-pin connector already crimped onto the end of these wires — it just pushes straight onto male header pins or male-to-female jumper wires, no soldering required. If soldering came up for this build, it's more likely for something else (joining the battery pack's leads to a connector, extending a wire that's too short, etc.) rather than the servo's own plug — see Step 2 of PLAN.md for solder-free workarounds for that.

**Key specs from the datasheet:**

| Spec | Value |
|---|---|
| Weight | 9 g |
| Dimensions | 22.2 × 11.8 × 31 mm |
| Stall torque | 1.8 kgf·cm |
| Operating speed | 0.1 s / 60° |
| Operating voltage | 4.8 V (~5 V) |
| Temperature range | 0–55 °C |
| Control signal | Standard hobby PWM: 50 Hz (20 ms period), ~1–2 ms pulse width — 1.5 ms = centre (0°), ~2 ms = +90°, ~1 ms = –90° |

This confirms the `Servo.write(angle)` calls in the example sketch above (0–180°, centred at 90) map correctly onto this servo's real pulse-width range — the `Servo` library handles the pulse timing for us, so there's nothing extra to configure.

## The power rule (important — read before wiring any servo)

> **Never power a servo motor from the Arduino's 5V pin.**

Official guidance from Arduino: the 5V pin is fed by the board's onboard regulator, which also powers the microcontroller and its own components. Servo motors draw current in on/off bursts as they move, and pulling that through the regulator can brown out the board (resets, glitches, weird behaviour).

Instead:
- Servo **signal** wire → an Arduino digital pin
- Servo **power (red)** wire → the external battery pack, directly
- Servo **ground** wire → tied together with the battery pack's ground, and *that* joint connects to the Arduino's **GND** pin (this "common ground" is what lets the signal timing and the power stay in sync)

Extra detail if useful later: the board can be powered itself via VIN (6–24V, e.g. the barrel jack) or via USB-C (5V only). When powered via VIN, the 5V pin can supply up to 1.2A through the onboard regulator; via USB, up to 2A. Either way, that's the board's own budget, not something to route servo current through.

### The actual numbers: what a SG90 draws vs. what the board can give

An SG90's current draw depends heavily on what it's doing:

| Servo state | Typical current (per servo) |
|---|---|
| Idle (holding position, not moving) | ~10 mA |
| Moving normally | ~100–250 mA |
| Stalled (pushed past its mechanical limit, or under heavy load) | ~360–700 mA |

(Figures from independent servo testing — [ProtoSupplies](https://protosupplies.com/product/servo-motor-micro-sg90/) measured 360 mA stall current on their units; [Zbotic's guide](https://zbotic.in/sg90-servo-motor-pinout-torque-specs-arduino-projects/) cites 500–700 mA as a stall-current range seen in the wild. Exact numbers vary batch to batch.)

Now scale that to **4 servos moving at once** — e.g. every step of a walking gait:
- Normal movement: 4 × 100–250 mA = **400 mA to 1 A**
- Worst case (a couple of legs stalled against something, e.g. the dog's own weight or a joint that's a bit too tight): could spike well past **1.5–2 A**

Compare that to the UNO R4 WiFi's 5V pin budget: **1.2 A** if the board is powered via the barrel jack/VIN, or **2 A** if powered via USB — and that budget is shared with the microcontroller itself, the onboard LED matrix, and (this board specifically) the ESP32 WiFi/Bluetooth module, which has its own current spikes when transmitting. So even in the best case, 4 servos moving together can eat the board's *entire* power budget with nothing left over for the board itself, and a stall condition can exceed it outright — that's what causes the resets/glitches/brownouts. It's not that the board "can't do 5V" — it's that servos are current-hungry, bursty loads and the board's regulator has a hard ceiling.

This is exactly why the plan is: signal wires to the Arduino, but power (red) wires straight to the battery pack, with only a shared ground tying the two power systems together.

### Choosing the power source: 4×AA vs. a USB power bank

We have both a 4×AA holder and USB lithium-ion power banks on hand — a power bank is actually the better choice for the servo rail, for a few concrete reasons:

| | 4×AA (alkaline) | USB power bank |
|---|---|---|
| Voltage | 6 V fresh, sagging as they drain (SG90's max rated voltage is 6 V, so fresh alkalines are right at the edge) | Regulated 5 V output, stays steady until it's nearly empty — sits comfortably mid-range for the SG90 (4.8–6 V rated) |
| Capacity | ~2,000–3,000 mAh, but alkaline cells have higher internal resistance, so voltage droops more under the bursty current a servo pulls | Typically 5,000–20,000 mAh — several times the runtime, and lithium cells handle sudden current draws better |
| Current capability | Fine for occasional bursts, but not designed for sustained high current | Most banks are rated 1–2.4 A per port (check the label) — comfortably covers our 400 mA–1 A normal-movement estimate above, with headroom for brief stalls too |

Rough runtime estimate: a 10,000 mAh power bank against an average draw of ~400 mA–1 A (servos aren't drawing their peak current the whole time — only while actually moving) works out to somewhere in the region of 10-20+ hours of real use, far more than a 4×AA pack would give.

**One quirk to know about:** some USB power banks auto-shutoff when they detect a very low current draw for a while (often somewhere around 50–60 mA) — this is a battery-saving feature meant for phone charging, and can cause the bank to switch itself off if the dog sits completely idle for a stretch. In practice this is unlikely to bite here: while the servos are actively walking they pull far more than that threshold, and if the Arduino's WiFi/Bluetooth module is active it also draws well above it. If it ever does trigger, the fix is usually to keep something drawing a small current in the background (an LED, or a tiny periodic servo twitch) — worth knowing if the dog ever seems to "run out of power" despite a full battery.

**Wiring it up:** rather than a breakout board dangling on its own cable, the plan is a small **USB-C female breakout board** — a compact PCB with the USB-C socket on one side and 0.1"/2.54mm header pins on the other (same pitch as the servo headers) — soldered directly onto the veroboard itself, so the finished board just has a female USB-C port built in. The battery's USB A-to-C cable then plugs straight into that, no separate cord in between. We only need to wire up the 5V and GND pins from its header out to the servo power rail (it also exposes D+/D-, CC1/CC2, SBU1/SBU2, which we can ignore). See shopping-list.md for the exact part. Worth double-checking it's a plain 5V breakout rather than a "PD trigger" board (those are designed to negotiate a higher voltage like 9V/12V from the source, which we don't want here).

The Arduino itself can be powered the normal way, via its own USB-C cable, ideally from a second power bank (or a second port on the same one) so its power draw is never sharing a rail with the servos — grounds still get tied together as usual.

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

- [SG90 servo datasheet (local copy)](datasheets/sg90_datasheet.pdf) — specs, dimensions, and the wiring/pulse diagram
- [Getting Started with UNO R4 WiFi](https://docs.arduino.cc/tutorials/uno-r4-wifi/r4-wifi-getting-started) — IDE + board package install (source for the setup steps above)
- [UNO R4 WiFi User Manual / Cheat Sheet](https://docs.arduino.cc/tutorials/uno-r4-wifi/cheat-sheet) — the master reference: full pinout, power supply rules, PWM, I2C, SPI, LED matrix, Bluetooth/WiFi (source for the pinout + power info above)
- [UNO R4 WiFi datasheet (PDF)](https://docs.arduino.cc/resources/datasheets/ABX00087-datasheet.pdf) — official electrical specs, for when we need exact numbers
- [Using the LED Matrix](https://docs.arduino.cc/tutorials/uno-r4-wifi/led-matrix) — full guide to the built-in display
- [Arduino UNO R4 WiFi Pinout Reference (Last Minute Engineers)](https://lastminuteengineers.com/arduino-uno-r4-wifi-pinout-reference/) — a more visual/annotated pinout diagram, handy alongside the official table
- [Arduino Servo library reference](https://arduinogetstarted.com/reference/library/arduino-servo-library) — the `Servo.h` functions used above
- [Arduino UNO R4 Servo Motor tutorial (newbiely.com)](https://newbiely.com/tutorials/arduino-uno-r4/arduino-uno-r4-servo-motor) — worked example specific to this board
- [VarSpeedServoRA4M1 library (GitHub)](https://github.com/KaledSouky/VarSpeedServoRA4M1) — fallback if servo jitter shows up
- Arduino Forum threads on R4 servo quirks, if needed: ["Trouble with Servos on R4 Wifi"](https://forum.arduino.cc/t/trouble-with-servos-on-r4-wifi/1151749), ["Fixing Jumping UNO R4 Servos" (DigiKey forum)](https://forum.digikey.com/t/fixing-jumping-arduino-uno-r4-servos-for-smooth-motion/50873)
- [SG90 current draw measurements (ProtoSupplies)](https://protosupplies.com/product/servo-motor-micro-sg90/) and [Zbotic's SG90 guide](https://zbotic.in/sg90-servo-motor-pinout-torque-specs-arduino-projects/) — source for the current-draw table above
- [Arduino Forum: power bank auto-shutoff discussion](https://forum.arduino.cc/t/powering-arduino-uno-and-servomotor-from-usb-power-bank/438001) — background on the low-current cutoff quirk
- [USB-C female breakout board, 24-pin, header pins (Amazon UK)](https://www.amazon.co.uk/Breakout-Female-Socket-Connector-Board/dp/B0C7KN8VK7) — a bare connector board (no cable) that solders onto the veroboard to give it a built-in female USB-C port

## Code references on GitHub

A few open-source starting points, roughly in order of how close they are to our exact setup:

- [Arduino's own `Servo` library examples](https://github.com/arduino-libraries/Servo/blob/master/examples/Sweep/Sweep.ino) — the canonical single-servo sweep example, functionally the same as the minimal example above. Good baseline, definitely works on the R4 WiFi since it's the official library.
- [advayc/Quadruped](https://github.com/advayc/Quadruped) — a 4-servo walking quadruped (one servo per leg, same scale as ours), with separate sketches for forward/backward/left/right/turning gaits — a good source of gait *logic* to study. **Caveat:** it's written for an ATmega328p board (Arduino Nano, via the MightyCore library) and pokes hardware timer registers directly rather than using the `Servo` library — that low-level code won't run as-is on the R4 WiFi's different (Renesas ARM) chip. Worth reading for the walking-sequence logic (which servo moves when), then re-implementing the actual servo control with `Servo.write()`, which does work on the R4.
- [SilentWoof/Barkduino](https://github.com/SilentWoof/Barkduino) — a 4-servo, Arduino Uno/Nano-compatible "expressive" quadruped: modular leg poses (`poseSleep()`, `poseStand()`), a distance sensor for triggered behaviours, and an expandable "trait" system for personality actions. This lines up nicely with both the walking-gait step and the longer-term "give it personality" ambition. **Note:** its license is personal/educational/non-commercial use only, which is fine for this project, but worth knowing if it's ever shared beyond that.
- [anoochit/arduino-quadruped-robot](https://github.com/anoochit/arduino-quadruped-robot) — a more advanced 12-servo (3 per leg) spider-style robot with Bluetooth app control and 3D-printable STL files — more ambitious than our 1-servo-per-leg design, but a good reference for later if we ever want more articulated legs.
- [PetoiCamp/OpenCat-Quadruped-Robot](https://github.com/PetoiCamp/OpenCat-Quadruped-Robot) — a full open-source framework for Boston-Dynamics-style robot pets. Overkill for where we are now, but a good source of inspiration/ideas for the ambitious extras down the line.
- [Quadruino: Robot Quadruped Walking (Arduino Project Hub)](https://projecthub.arduino.cc/escorpia/quadruino-robot-quadruped-walking-09fb92) — a full build walkthrough (not just code) with wiring diagrams, useful alongside the GitHub repos above.

## Later: parts to research for the ambitious extras
- Small OLED/LCD (I2C, e.g. SSD1306) for eyes
- Piezo speaker (simple beeps via `tone()`) or DFPlayer Mini MP3 module (recorded voice clips) for the "robot voice"
