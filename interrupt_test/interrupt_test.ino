#define LED_BUILTIN D4
#define InterruptPin D3
#define SECOND 1000000

// prototypes
void ICACHE_RAM_ATTR
interrupt_handler(); // Interrupt handler function prototype

void ICACHE_RAM_ATTR interrupt_handler() {
  digitalWrite(LED_BUILTIN, HIGH);
  // piscar led 10 vezes
  for (int i = 0; i < 20; i++) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delayMicroseconds(1.5 * SECOND); // Inverter estado do LED
    ESP.wdtFeed();                   // Alimenta o watchdog
    Serial.println("Interrupt");
  }
}

void setup() {
  ESP.wdtDisable(); // Desativa o watchdog
  Serial.begin(115200);
  Serial.println("Iniciando...");

  // IO Pins
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(InterruptPin, INPUT);

  attachInterrupt(digitalPinToInterrupt(InterruptPin), interrupt_handler,
                  RISING);
  ESP.wdtFeed(); // Alimenta o watchdog
}
long last_blink = 0;
void loop() {
  long time = millis();
  if (time - last_blink > 100) {
    ESP.wdtFeed();
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // Piscar led
    Serial.println("Curso normal");
    last_blink = time;
  }
}
