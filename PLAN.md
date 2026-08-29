# Robot Dog — PLAN.md

*This is the living project plan. Keep it up to date as steps are finished or the plan changes — check items off, add new ones, and note dates when something is decided.*

**Last updated:** 29 August 2026

Related docs:
- [handover.md](handover.md) — origin story of the project (the original Gemini conversation) and the shopping list
- [arduino-guide.md](arduino-guide.md) — first-time Arduino UNO R4 WiFi setup + reference links (pinout, wiring, docs)

## Current stage

**We are at: Step 2 — first servo test.** Today (29 Aug 2026) we planned to wire up the servos, but discovered some servo connectors need soldering before they'll plug in. See the note under Step 2 below for solder-free workarounds — recommend trying those first so we're not blocked waiting for a soldering iron/session.

## Stepped Plan

### Step 0 — Parts check ✅ done
- [x] Servo motors purchased
- [x] Arduino Uno R4 WiFi purchased
- [x] 3D printer available, printing skills in-house
- [x] Battery pack for servo power — already have one
- [x] 4 servos confirmed (one per leg — proper quadruped walk)

### Step 1 — Arduino first-time setup
- [ ] Install the Arduino IDE and the UNO R4 board package (see [arduino-guide.md](arduino-guide.md))
- [ ] Plug in the board, select it in the IDE, upload the built-in **Blink** example to confirm it all works
- [ ] Bonus: the board ships with a Tetris animation on its LED matrix — if it gets overwritten, the guide has the code to restore it

### Step 2 — First servo test (cardboard/breadboard version, no soldering)
Goal: get **one** servo sweeping back and forth under Arduino control before building anything permanent.

- [ ] Servo wiring, no soldering needed for the standard 3-pin connector:
  - Signal (orange/yellow) wire → Arduino digital pin (e.g. D9)
  - Power (red) wire → **do not use the Arduino 5V pin for this** — see power note below
  - Ground (brown/black) wire → Arduino GND, and tied to the battery's ground too
- [ ] If a connector genuinely needs joining (e.g. splicing an extension wire, or wiring the battery pack to a barrel jack) and there's no soldering iron to hand yet, solder-free options:
  - small screw-terminal block or "chocolate block" connector
  - Wago-style lever nuts (very kid-friendly, reusable, no tools)
  - a breadboard, if the wire ends are short enough to poke in
  - twist the wires tightly together, then cover with electrical tape or a heat-shrink tube (heat-shrink can be sealed with a hairdryer or lighter — no iron needed) — fine for testing, not a permanent fix
- [ ] Upload a basic sweep sketch (see [arduino-guide.md](arduino-guide.md) for example code) and confirm the servo moves smoothly
- [ ] **Do the cardboard version first**, as planned: hot-glue the servo to a scrap cardboard/cereal-box mount just to prove out the wiring and code, before committing to the 3D-printed parts

### Step 3 — All 4 servos + basic gait
- [ ] Repeat Step 2's wiring for all 4 servos (signal pins D9, D10, D11, D6 — see pinout in the guide for why these ones)
- [ ] All 4 servo power (red) wires → the battery pack we already have, not the Arduino 5V pin (see power rule in the guide)
- [ ] Write/adapt a simple walking gait sketch (moves legs in sequence rather than all at once)
- [ ] Test on the cardboard mock-up first

### Step 4 — 3D-printed chassis
- [ ] Find or design a 3D-printable quadruped chassis/leg set sized for the servos we bought (check Thingiverse/Printables, or design in CAD)
- [ ] Print a test leg first to confirm the servo horn fits before printing a full set
- [ ] Print full chassis + 4 legs
- [ ] Transplant the proven wiring/code from the cardboard version onto the printed body

### Step 5 — Ambitious extras (once it's walking reliably)
- [ ] **Eyes:** LED or small LCD eyes
  - Simplest: two individual LEDs (with resistors) for basic "on" eyes, or an RGB LED each for colour/mood
  - More expressive: a small OLED/LCD screen (e.g. SSD1306 128x64 I2C screen) showing simple cartoon eyes — the Uno R4 WiFi's I2C pins (A4/SDA, A5/SCL) are ready for this
  - Fun freebie: the board's own **built-in 12x8 LED matrix** could do a first "face" test before adding separate eye hardware — see the guide
- [ ] **Robot voice:** options from simplest to most ambitious
  - Simple beeps/tones: a small piezo speaker + the Arduino `tone()` function — easiest, no extra library
  - Recorded voice clips: a DFPlayer Mini MP3 module + speaker, triggered over serial — plays actual recorded "barks" or phrases
  - Text-to-speech: possible but more advanced (limited onboard resources); likely better to pre-record clips instead

## Open questions
- [ ] Decide: separate eyes (LED/LCD) vs. reusing the built-in LED matrix for a face
- [ ] Decide: beeps (piezo) vs. recorded voice (DFPlayer Mini) for step 5
