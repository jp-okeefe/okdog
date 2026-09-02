/*
 * dog_legs.ino — 4-leg servo driver for the robot dog
 * Board:  Arduino Uno R4 WiFi
 * Servos: 4 × SG90 / MG90S (one per leg, fore/aft swing)
 *
 * POWER: do NOT run four servos off the R4's 5V pin. Use a separate 5–6 V
 * supply (2 A+), tie its GND to the Arduino GND, and feed only the signal
 * wires from the pins below.
 *
 * Serial commands @115200 (newline-terminated):
 *   c            centre all legs and hold
 *   d            detach (servos go limp — safest resting state)
 *   s            sweep test, one leg at a time
 *   t            trot gait  (diagonal pairs)
 *   w            wave gait  (one leg at a time, slower, more stable)
 *   x            stop gait, return to centre
 *   l <n> <deg>  drive leg n (0-3) to an absolute angle, e.g. "l 2 105"
 *   a <deg>      set gait amplitude in degrees (default 25)
 *   p <ms>       set gait period in ms (default 1200)
 *   r            reverse the last-addressed leg's direction (calibration aid)
 *   ?            print status
 */

#include <Servo.h>

// ─────────────────────────── CONFIG ───────────────────────────
// Legs are indexed: 0 = front-left, 1 = front-right, 2 = back-left, 3 = back-right
// mapped in that order to D6, D9, D10, D11. If your wiring runs in a different
// order, reorder the PINS array below rather than rewiring.
const uint8_t NUM_LEGS = 4;
const uint8_t PINS[NUM_LEGS] = { 6, 9, 10, 11 };   // D6, D9, D10, D11

// SG90/MG90S pulse limits. Widen carefully — going past the mechanical stop
// will stall the servo and cook it. Start narrow, open up once calibrated.
const int PULSE_MIN_US = 600;    // ≈ 0°
const int PULSE_MAX_US = 2400;   // ≈ 180°

// Per-leg zero trim, in degrees, applied to the 90° centre. Adjust these until
// all four legs hang vertically with the command "c".
float trim[NUM_LEGS] = { 0.0, 0.0, 0.0, 0.0 };

// +1 or -1. Flip a leg if it swings backwards relative to its mirror.
int8_t dir[NUM_LEGS] = { +1, -1, +1, -1 };

// Soft limits around centre — the servo is never commanded outside this.
const float ANGLE_MIN = 45.0;
const float ANGLE_MAX = 135.0;

// Gait defaults
float gaitAmplitudeDeg = 25.0;    // peak swing either side of centre
unsigned long gaitPeriodMs = 1200; // one full stride

// Slew limit: max degrees per update. Keeps startup and mode changes gentle.
const float MAX_STEP_DEG = 2.0;
const unsigned long UPDATE_MS = 20;   // 50 Hz control loop

// ──────────────────────── STATE ────────────────────────
enum Mode { MODE_DETACHED, MODE_HOLD, MODE_SWEEP, MODE_TROT, MODE_WAVE, MODE_MANUAL };
Mode mode = MODE_DETACHED;

Servo servos[NUM_LEGS];
float current[NUM_LEGS];   // where we believe the horn is
float target[NUM_LEGS];    // where we want it
bool  attached = false;

unsigned long lastUpdate = 0;
unsigned long gaitStart  = 0;
int lastLeg = 0;

// Phase offsets (fraction of a stride) for each gait.
// Trot: diagonal pairs move together — FL+BR, then FR+BL.
const float PHASE_TROT[NUM_LEGS] = { 0.0, 0.5, 0.5, 0.0 };
// Wave: one leg at a time, 1/4 stride apart.
const float PHASE_WAVE[NUM_LEGS] = { 0.0, 0.5, 0.75, 0.25 };

// ──────────────────────── HELPERS ────────────────────────
float clampAngle(float a) {
  if (a < ANGLE_MIN) return ANGLE_MIN;
  if (a > ANGLE_MAX) return ANGLE_MAX;
  return a;
}

void writeLeg(uint8_t i, float angleDeg) {
  float a = clampAngle(angleDeg);
  int us = (int)(PULSE_MIN_US + (a / 180.0) * (PULSE_MAX_US - PULSE_MIN_US));
  servos[i].writeMicroseconds(us);
}

void attachAll() {
  if (attached) return;
  for (uint8_t i = 0; i < NUM_LEGS; i++) {
    servos[i].attach(PINS[i], PULSE_MIN_US, PULSE_MAX_US);
    // Assume we're starting from centre; the slew limiter walks us there gently.
    current[i] = 90.0 + trim[i];
    target[i]  = current[i];
    writeLeg(i, current[i]);
  }
  attached = true;
}

void detachAll() {
  for (uint8_t i = 0; i < NUM_LEGS; i++) servos[i].detach();
  attached = false;
  mode = MODE_DETACHED;
  Serial.println(F("detached — servos limp"));
}

void centreAll() {
  attachAll();
  for (uint8_t i = 0; i < NUM_LEGS; i++) target[i] = 90.0 + trim[i];
  mode = MODE_HOLD;
  Serial.println(F("centring"));
}

void startGait(Mode m) {
  attachAll();
  gaitStart = millis();
  mode = m;
  Serial.print(F("gait: "));
  Serial.print(m == MODE_TROT ? F("trot") : F("wave"));
  Serial.print(F("  amp=")); Serial.print(gaitAmplitudeDeg, 1);
  Serial.print(F("  period=")); Serial.println(gaitPeriodMs);
}

// ──────────────────────── GAIT ────────────────────────
void updateGait() {
  const float *phase = (mode == MODE_TROT) ? PHASE_TROT : PHASE_WAVE;
  float t = (float)((millis() - gaitStart) % gaitPeriodMs) / (float)gaitPeriodMs;

  for (uint8_t i = 0; i < NUM_LEGS; i++) {
    float ph = t + phase[i];
    while (ph >= 1.0) ph -= 1.0;
    float swing = sin(ph * 2.0 * PI) * gaitAmplitudeDeg;
    target[i] = 90.0 + trim[i] + dir[i] * swing;
  }
}

// Sweep test: walk each leg to one soft limit, then the other, then centre.
void updateSweep() {
  static uint8_t legIdx = 0;
  static uint8_t stage  = 0;
  static unsigned long stageStart = 0;

  if (stageStart == 0) stageStart = millis();

  float centre = 90.0 + trim[legIdx];
  float lo = clampAngle(centre - gaitAmplitudeDeg);
  float hi = clampAngle(centre + gaitAmplitudeDeg);

  for (uint8_t i = 0; i < NUM_LEGS; i++)
    if (i != legIdx) target[i] = 90.0 + trim[i];

  if      (stage == 0) target[legIdx] = lo;
  else if (stage == 1) target[legIdx] = hi;
  else                 target[legIdx] = centre;

  // Advance when settled, or after a timeout.
  bool settled = fabs(current[legIdx] - target[legIdx]) < 0.5;
  if (settled || millis() - stageStart > 2000) {
    stage++;
    stageStart = millis();
    if (stage > 2) {
      stage = 0;
      Serial.print(F("swept leg ")); Serial.println(legIdx);
      legIdx++;
      if (legIdx >= NUM_LEGS) {
        legIdx = 0;
        mode = MODE_HOLD;
        Serial.println(F("sweep complete"));
      }
    }
  }
}

// ──────────────────────── SERIAL ────────────────────────
void printStatus() {
  Serial.print(F("mode="));
  switch (mode) {
    case MODE_DETACHED: Serial.print(F("detached")); break;
    case MODE_HOLD:     Serial.print(F("hold"));     break;
    case MODE_SWEEP:    Serial.print(F("sweep"));    break;
    case MODE_TROT:     Serial.print(F("trot"));     break;
    case MODE_WAVE:     Serial.print(F("wave"));     break;
    case MODE_MANUAL:   Serial.print(F("manual"));   break;
  }
  Serial.print(F("  amp=")); Serial.print(gaitAmplitudeDeg, 1);
  Serial.print(F("  period=")); Serial.print(gaitPeriodMs);
  Serial.print(F("  angles="));
  for (uint8_t i = 0; i < NUM_LEGS; i++) {
    Serial.print(current[i], 1);
    Serial.print(i < NUM_LEGS - 1 ? ',' : '\n');
  }
}

void handleCommand(char *line) {
  char cmd = line[0];
  switch (cmd) {
    case 'c': centreAll(); break;
    case 'd': detachAll(); break;
    case 's': attachAll(); mode = MODE_SWEEP; Serial.println(F("sweep test")); break;
    case 't': startGait(MODE_TROT); break;
    case 'w': startGait(MODE_WAVE); break;
    case 'x': centreAll(); break;
    case '?': printStatus(); break;

    case 'l': {
      int n; float deg;
      if (sscanf(line + 1, "%d %f", &n, &deg) == 2 && n >= 0 && n < NUM_LEGS) {
        attachAll();
        mode = MODE_MANUAL;
        lastLeg = n;
        target[n] = clampAngle(deg);
        Serial.print(F("leg ")); Serial.print(n);
        Serial.print(F(" -> ")); Serial.println(target[n], 1);
      } else {
        Serial.println(F("usage: l <0-3> <deg>"));
      }
      break;
    }
    case 'a': {
      float v;
      if (sscanf(line + 1, "%f", &v) == 1) {
        gaitAmplitudeDeg = constrain(v, 0.0, 45.0);
        Serial.print(F("amp=")); Serial.println(gaitAmplitudeDeg, 1);
      }
      break;
    }
    case 'p': {
      long v;
      if (sscanf(line + 1, "%ld", &v) == 1) {
        gaitPeriodMs = constrain(v, 300L, 5000L);
        Serial.print(F("period=")); Serial.println(gaitPeriodMs);
      }
      break;
    }
    case 'r':
      dir[lastLeg] = -dir[lastLeg];
      Serial.print(F("leg ")); Serial.print(lastLeg);
      Serial.print(F(" direction now ")); Serial.println(dir[lastLeg]);
      break;

    case '\0': break;
    default: Serial.println(F("unknown — see header for commands"));
  }
}

void readSerial() {
  static char buf[32];
  static uint8_t len = 0;
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (len > 0) { buf[len] = '\0'; handleCommand(buf); len = 0; }
    } else if (len < sizeof(buf) - 1) {
      buf[len++] = c;
    }
  }
}

// ──────────────────────── MAIN ────────────────────────
void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println(F("robot dog — 4 leg servo driver"));
  Serial.println(F("servos start DETACHED. 'c' to centre, '?' for status."));
  for (uint8_t i = 0; i < NUM_LEGS; i++) { current[i] = 90.0; target[i] = 90.0; }
}

void loop() {
  readSerial();

  unsigned long now = millis();
  if (now - lastUpdate < UPDATE_MS) return;
  lastUpdate = now;

  if (!attached) return;

  if      (mode == MODE_TROT || mode == MODE_WAVE) updateGait();
  else if (mode == MODE_SWEEP)                     updateSweep();

  // Slew-limited move toward target — no snapping, no brownouts.
  for (uint8_t i = 0; i < NUM_LEGS; i++) {
    float err = clampAngle(target[i]) - current[i];
    if (err >  MAX_STEP_DEG) err =  MAX_STEP_DEG;
    if (err < -MAX_STEP_DEG) err = -MAX_STEP_DEG;
    current[i] += err;
    writeLeg(i, current[i]);
  }
}
