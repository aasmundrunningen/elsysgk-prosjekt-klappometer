#include <Arduino.h>

#define micPin 32

void setup() {
  pinMode(micPin, INPUT);
  pinMode(25, INPUT);
  Serial.begin(9600);
}

void loop() {
  int a = analogRead(micPin);
  Serial.println(String(float(a)/4096.0 * 5.0));
  delay(100);
}