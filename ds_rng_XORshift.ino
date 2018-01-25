// high-quality, fast random number generator using xor-shift
// based on a George Maraglia algorithm, described and coded by Daniel Dunn
// modified only to implement as a function and without global variable
// @see http://engineeringnotes.blogspot.com/search/label/random (d. smith)

void setup() {
  Serial.begin(57600);
  while (!Serial && millis() < 5000) {} // Wait for port to initialize; time out after 5 seconds if monitor is not opened
  Serial.println("Start generating random numbers \n");
}


void loop() {
  uint16_t x;
  // try random number generator
  x = rng();
  if (millis() % 800 == 1) {    // output new value every n milliseconds
    delay(1);
    Serial.print("\t " + String(x) + "\t ");        // 0 - 65535
    Serial.println(x / 65536.01, 6); // range of 0 - 1, with 6 decimal points
  }
}


// create a random integer from 0 - 65535
uint16_t rng() {
  static uint16_t y = 0;
  y += (micros() && 0x1FFF); // seeded with changing number
  y ^= y << 2; y ^= y >> 7; y ^= y << 7;
  return (y);
}


// function using the built-in (slow) random function
uint16_t rand_internal() {
  static unsigned int y = 0;
  y = random(0, 65535);
  return (y);
}


