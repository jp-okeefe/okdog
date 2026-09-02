# Shopping List

*Living list — tick items off as bought, add new ones as the plan grows. Links are UK Amazon where possible; check current price/stock before ordering, and any similar-spec item is fine — these aren't must-be-this-exact-listing.*

**Last updated:** 29 August 2026

## Now needed — proof-of-concept veroboard build (Step 4.5 in PLAN.md)

We're still at proof-of-concept, so this is a hand-wired **veroboard/stripboard** build rather than a manufactured PCB — cheap, no design software needed, and good enough to prove the wiring before committing to a "real" PCB later.

- [ ] **Veroboard/stripboard** — a board (or small pack, so there's a spare for mistakes) big enough for a power rail + 4 servo headers + Arduino jack wiring:
  - [20pcs Various Sizes Prototype Veroboard/Stripboard pack](https://www.amazon.co.uk/20pcs-Prototype-Veroboard-Stripboard-Various/dp/B0D9K1T8LF) — good option since it includes several sizes, useful while we're still figuring out the layout
  - Alternative if we'd rather have one bigger board: [20×10cm PCB Veroboard Prototype Stripboard](https://www.amazon.co.uk/20x10-Veroboard-Prototype-Stripboard-breadboard/dp/B085MZH28F)

- [ ] **Male header pins (breakaway strip, 2.54mm pitch)** — solder these onto the veroboard to create a socket for each servo; the servo's own female JR connector then just plugs straight on, no soldering the servo wires themselves and easy to unplug for repairs:
  - [50pcs Single Row 40-Pin 2.54mm Male Header Pins (breakaway)](https://www.amazon.co.uk/Single-2-54mm-Male-Header-Pins/dp/B0FF1TCTMV)

- [ ] **Servo extension cables (3-pin, male-to-female, JR/Futaba style)** — only needed if a servo's own wire is too short to reach the veroboard once it's mounted in the body; skip if the built-in wires reach fine:
  - [AyeVision Pack of 10, 30cm, Male to Female](https://www.amazon.co.uk/AyeVision-Extension-Female-Futaba-Connector/dp/B08Q86CJG8)
  - Longer option if needed: [POFET 10pcs, 100cm](https://www.amazon.co.uk/POFET-Extension-Cables-Control-Aircraft/dp/B08728B95B)

- [ ] **DC barrel jack pigtail (male plug, 5.5×2.1mm)** — solder onto the veroboard's power rail so it can plug straight into the Arduino's own barrel jack socket, if we want the board powered that way rather than over USB-C:
  - [5 Pairs 5.5mm x 2.1mm DC Power Pigtail Cable and 5 Pairs 12V 5A Male & Female Connectors Power Adapter Connectors](https://www.amazon.co.uk/YIXISI-Connectors-Security-Surveillance-Lighting-5-pairs-cable-connector/dp/B0CMK6ZC7X/ref=sr_1_1_sspa) (comes with both male and female ends, so it covers other DC-jack needs too)

- [ ] **USB-C female breakout board (no cable/cord — a bare connector block)** — a small PCB with a USB-C socket on one side and header pins on the other, so it can be soldered straight onto the veroboard itself as a built-in "female USB-C jack"; the battery's USB A-to-C cable then just plugs straight into the finished board:
  - [USB Type-C Breakout Female Socket, 24-pin, PCB board (SB Components)](https://www.amazon.co.uk/Breakout-Female-Socket-Connector-Board/dp/B0C7KN8VK7) — exposes 5V, GND, D+/D-, CC1/CC2, SBU1/SBU2 on a 0.1"/2.54mm pin header (same pitch as the servo header pins, so it solders onto veroboard the same way); we only need the 5V and GND pins for this. Rated 1.5A, which is on the tight side of our 4-servo current estimate (see arduino-guide.md) — fine for a single-servo proof of concept, worth re-checking headroom once all 4 are wired in
  - Avoid "PD trigger" boards (ones that mention QC/PD and adjustable 9V/12V/20V output) — we just want the plain default 5V, not a negotiated higher voltage

## Already have
- [x] 4× SG90 servo motors
- [x] Arduino UNO R4 WiFi
- [x] 4×AA battery holder
- [x] USB lithium-ion power bank(s)
- [x] 3D printer

## Later — not needed yet
- Custom manufactured PCB (once the veroboard proves the design out) — components TBD, likely the same parts list above plus a proper connector footprint
- Eyes: small I2C OLED/LCD screen (e.g. SSD1306 128×64) — research specific part when we get to Step 5
- Voice: piezo speaker (simple) or DFPlayer Mini MP3 module (recorded clips) — research specific part when we get to Step 5

## Notes
- All servo connectors are the standard 3-pin JR/Futaba type, so anything sold as "servo extension" or "RC servo connector" will fit our SG90s.
- The header-pin approach means we likely don't need separate "servo connectors" beyond the pins themselves — the servo's existing female connector does the job.
