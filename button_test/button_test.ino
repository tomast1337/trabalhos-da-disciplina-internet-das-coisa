#define LED_BUILTIN D4
#define BUTTON_PIN D3

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, !digitalRead(BUTTON_PIN));
  delay(1000);
}
