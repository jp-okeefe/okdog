# Robot Dog Project — Handover Doc

**Started:** 21 August 2026, in a Gemini AI Mode conversation (source: https://share.google/aimode/5l21Pdvj4V0o2AcGi)
**Purpose of this doc:** carry the original planning conversation and our current status over into this project, so work can continue from here.

## Background

This started as a project to help a 10-year-old build a robot dog. The original Gemini conversation first suggested a beginner all-in-one kit (~£10–£20), but the goal was quickly redefined: **build it from scratch**, to teach real mechanics, electronics and programming rather than just snapping together a pre-made kit.

## Original plan (from the Gemini conversation)

Gemini's initial "from scratch" recommendation was a low-cost, no-3D-printer build: a cardboard-and-popsicle-stick chassis with micro servos, hot-glued together, wired to an Arduino. Key details worth keeping for reference:

### Original shopping list (cardboard version)
- **Brain:** 1x Arduino Uno R3 (or R4) — clones like ELEGOO work fine and are cheaper
- **Muscles:** 2x or 4x SG90 micro servo motors (move the legs)
- **Power:** 1x 4xAA battery holder with a DC barrel jack
- **Nervous system:** male-to-female jumper wires
- **Skeleton (original suggestion):** thick corrugated cardboard, wooden popsicle sticks, hot glue gun
- Total original estimated cost: ~£15–£25

### Original mechanical build steps (cardboard version, now superseded by 3D printing — see "Where we are now")
1. Cut a cardboard torso (~15cm x 8cm).
2. Hot-glue two servos flat against the front of the body and two at the back, gears pointing outward.
3. Glue wooden popsicle sticks onto the servo horns to form rigid legs (more rigid than cardboard alone).

Gemini also generated a printable A4 cardboard cutting-guide PDF (chassis ~15x8cm, 4x leg pieces ~11x2cm, head/face panel ~6x5cm, tail linkage ~8x1cm) — this is now obsolete since we're 3D printing the body instead, but the servo count and layout logic (4 legs, one servo per leg) still applies.

### Wiring plan (still applicable)
Each SG90 servo has 3 wires:
- **Brown = Ground (GND)**
- **Red = Power (5V)**
- **Orange = Signal**

Basic wiring:
- All servo Brown wires → common GND
- All servo Red wires → 5V (see power note below)
- Each servo's Orange signal wire → its own digital pin on the Arduino (e.g. Pins 9, 10, 11, 12 for four legs)

### Important power note (from the Elegoo Uno R3 discussion — also applies to the Uno R4 WiFi)
Driving four servos at once can draw more current than the Arduino's onboard regulator can safely supply, risking resets/glitches. The recommended fix:
- Servo **signal** wires → Arduino digital pins (e.g. 9, 10, 11, 12)
- Servo **red/power** wires → the external battery pack directly, **not** the Arduino's 5V pin
- Servo **ground** wires and the battery pack's ground → tied together and connected to the Arduino's GND pin (common ground so signal timing stays in sync with power)

### Elegoo vs. official Arduino
Gemini confirmed an Elegoo Uno R3 is 100% software/pin compatible with a genuine Arduino Uno, works fine with the free Arduino IDE, and has enough digital pins (14) for 4 leg servos, with clearly labelled pins that are easy for a kid to wire without soldering.

## Where we are now (current status)

- ✅ Servo motors purchased
- ✅ Arduino **Uno R4 WiFi** purchased (upgrade from the R3/Elegoo originally discussed — same wiring/pin logic applies; the R4 WiFi has the same power-sharing caution as noted above)
- ✅ We have a 3D printer and the skills to use it
- 🔄 **Decision made:** the dog's body/chassis and legs will be **3D printed**, replacing the original cardboard-and-popsicle-stick plan. The servo mounting points, leg-per-servo layout, and wiring/power plan from the original conversation still apply — only the chassis material and construction method have changed.

## Open questions / next steps

- [ ] Source or design a 3D-printable robot dog leg/chassis model sized to fit the SG90 servos (search options: Thingiverse, Printables, or a custom design in CAD)
- [ ] Confirm number of servos in use (the original plan flexed between 2 and 4 — recommend 4, one per leg, for a proper quadruped gait)
- [ ] Get the beginner Arduino sketch to sweep/test the servo legs (Gemini offered to provide this but it wasn't generated in the conversation yet)
- [ ] Decide on battery pack for the servo power rail (4xAA battery holder w/ barrel jack was the original suggestion — confirm this is still the plan, or check if the R4 WiFi's power needs change this)
- [ ] Optional: revisit Gemini's offer to add an ultrasonic sensor so the dog can avoid obstacles
- [ ] Optional: look at the "Albert Robot Dog Project" on GitHub, mentioned in the original conversation as open-source starter logic for a walking gait

## Source

Original conversation (Gemini AI Mode, 21 August 2026): https://share.google/aimode/5l21Pdvj4V0o2AcGi
