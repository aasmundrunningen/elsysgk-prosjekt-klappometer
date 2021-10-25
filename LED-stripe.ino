
#include "FastLED.h"
#define NUM_LEDS 3
#define LED_PIN 2

CRGB leds[NUM_LEDS];


void setup() { 
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS); 
  }
  

void loop() {
  int level = 2;          // Level er hvor mange LED-lys som skal lyse.
  ChangeLedLevel(level);  // Skriv inn antall LED som skal lyse som argument her.
}


void ChangeLedLevel(int level) {
  for (int i; i<level; i++) {
      leds[i] = CRGB::Red; FastLED.show(); delay(30);
  }
}
