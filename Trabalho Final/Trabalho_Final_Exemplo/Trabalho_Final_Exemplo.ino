// pin D4
#define LED D4

#define SECOND 1000

void setup(){
    Serial.begin(115200); // Inicia porta serial

    pinMode(LED, OUTPUT);
    digitalWrite(LED, 1);
    delayMicroseconds(SECOND);
    yield();
    for (int i = 0; i < 10; i++) {
        digitalWrite(LED, !digitalRead(LED));
        
        yield();
        delayMicroseconds(2*SECOND*1000);
        yield();
    
    }
}

void loop() {
    digitalWrite(LED, !digitalRead(LED));
    delayMicroseconds(SECOND*100);
    yield();
}
