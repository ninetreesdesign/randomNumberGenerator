// gaussian -- call rndGaussian() in a loop and print each sample
//
// Values are mean 0, standard deviation 1, roughly -6..+6.
// Teensy: waitForSerial() times out after 5 s if no monitor is attached.

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
  Serial.println(rndGaussian(), 4);
  delay(10);
}
