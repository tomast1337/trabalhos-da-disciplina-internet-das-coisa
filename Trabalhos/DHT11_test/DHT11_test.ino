#include <DHT.h>

#define D3  0

#define DHT_TYPE DHT11

DHT dht(D3, DHT_TYPE);

//Setup
void setup() {
    Serial.begin(115200); // Inicia porta serial
    dht.begin(); // Inicia o sensor DHT11
}

void loop() {
    float umidade = dht.readHumidity(); // Leitura da umidade
    float temperatura = dht.readTemperature(); // Leitura da temperatura
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.print(" *C");
    Serial.print("\n");
    Serial.print("Umidade: ");
    Serial.print(umidade);
    Serial.print(" %");
    Serial.print("\n");
    delay(1000); // Delay de 1 segundo
}
