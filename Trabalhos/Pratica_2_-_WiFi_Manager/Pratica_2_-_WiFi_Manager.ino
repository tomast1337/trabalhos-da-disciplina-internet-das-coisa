
#include <ESP8266WiFi.h> // Inclui a biblioteca
#include <WiFiManager.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

// Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient

// WIFI
const char *myHostname = "nodeMCU"; // Nome do host na rede
const char *SSID = "RedeITO";       // SSID / nome da rede WI-FI (AP) do WiFiManager
const char *PASSWORD = "12345678";  // Senha da rede WI-FI (AP) do WiFiManager

void init_WifiAp()
{
    WiFi.hostname(myHostname);
    WiFiManager wifiManager;

    wifiManager.autoConnect(SSID, PASSWORD);
    Serial.print("Conectado com sucesso na rede via WifiManager na rede: ");
    Serial.println(WiFi.SSID());
    Serial.println();
    Serial.print("IP obtido: ");
    Serial.print(WiFi.localIP()); // mostra o endereço IP obtido via DHCP
    Serial.println();
    Serial.print("Endereço MAC: ");
    Serial.print(WiFi.macAddress()); // mostra o endereço MAC do esp8266
}

void setup()
{
    pinMode(D0, OUTPUT); // Configura o pino D0 como saída
    
    Serial.begin(115200); // observe a velocidade de conexão com o terminal
    init_WifiAp();        // inicia o WiFiManager
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED){
        digitalWrite(D0,HIGH); // Conectado, acende o LED    
        return;
    }
        
    digitalWrite(D0,LOW); // Não conectado apaga o LED
    
    WiFi.hostname(myHostname);
    WiFi.begin(); // Conecta na rede WI-FI

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(100);
        Serial.print(".");
        Serial.println();
        Serial.print("Conectado com sucesso na rede: ");
        Serial.print(SSID);
        Serial.println();
        Serial.print("IP obtido: ");
        Serial.print(WiFi.localIP()); // mostra o endereço IP obtido via DHCP
        Serial.println();
        Serial.print("Endereço MAC: ");
        Serial.print(WiFi.macAddress()); // mostra o endereço MAC do esp8266
    }
}
