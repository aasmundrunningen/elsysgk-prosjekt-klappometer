
struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};

Button button1 = {33,0,false};

void IRAM_ATTR isr(){        // Denne kjøres ved "interrupt"
  button1.pressed = true;
}

void setup() {
  Serial.begin(115200);
  pinMode(button1.PIN,INPUT_PULLUP);
  attachInterrupt(button1.PIN,isr,FALLING);

}

void loop() {

}
 
//i eksempelet på nettsiden er det også en detach interrupt sak, men jeg tror ikke
//det er noen vits i å ha med? da slutter på en måte esp32-en å se etter interrupts fra knappen
//(altså pinnen knappen er satt på)
