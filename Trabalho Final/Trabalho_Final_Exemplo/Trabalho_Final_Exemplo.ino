// pin D4
#define LED D4

#define SECOND 1000

void setup(){
    Serial.begin(115200); // Inicia porta serial

    pinMode(LED, OUTPUT);
    digitalWrite(LED, 1);
    delayMicroseconds(SECOND);
    for (int i = 0; i < 10; i++) {
        digitalWrite(LED, !digitalRead(LED));
        delayMicroseconds(2*SECOND*1000);
    }
}

void loop() {
    digitalWrite(LED, !digitalRead(LED));
    delay(SECOND/10);
}
