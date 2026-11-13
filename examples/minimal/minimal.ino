// minimal -- call rnd() in a loop and print each sample
//
// Teensy: waitForSerial() gives the USB monitor time to open and times out
// after 5 s, so the sketch still runs when no monitor is attached.

#include <ds_random.h>

// Teensy only (USB CDC). On AVR the test is always true and returns at once.
static void waitForSerial(uint32_t timeout_ms = 5000) {
  unsigned long start_ms = millis();
  while (!Serial && (millis() - start_ms) < timeout_ms) {
    // spin
  }
}

void setup() {
  Serial.begin(115200);
  waitForSerial();
}

void loop() {
  Serial.println(rnd());
  delay(10);
}
