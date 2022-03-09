#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>

#define PASS_OTA "1235"

#define LED_BUILTIN D4
#define InterruptPin D3
#define SECOND 1000000
long last_blink = 0;


// prototypes
void ICACHE_RAM_ATTR interrupt_handler(); // Interrupt handler function prototype
void init_wifi_manager();
void reconect_wiFi();
void initOTA();

// WIFI MANAGER
const char *myHostname = "ESP"; // Nome do host na rede
const char *SSID_WiFiManager = "interrupções e whatchdog";       // SSID / nome da rede WI-FI (AP) do WiFiManager
const char *PASSWORD_WiFiManager = "12345678";  // Senha da rede WI-FI (AP) do WiFiManager

void init_wifi_manager();
void reconect_wiFi();
void initOTA();

// função mostra inicialização do wifi manager
void init_wifi_manager() {
  delayMicroseconds(SECOND / 100);
  WiFi.hostname(myHostname);
  WiFiManager wifiManager;
  wifiManager.autoConnect(SSID_WiFiManager, PASSWORD_WiFiManager);
  Serial.println("\n------Conexao WI-FI Manager------\n");
  Serial.print("Conectando-se na rede: ");
  Serial.println(SSID_WiFiManager);
  Serial.print("Senha: ");
  Serial.println(PASSWORD_WiFiManager);
  reconect_wiFi();
  ESP.wdtFeed(); // Alimenta o watchdog
}

// Função verifica se a rede está conectada, se não estiver, ativa o WiFiManager
void reconect_wiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    ESP.wdtFeed();
    return;
  }

  WiFi.hostname(myHostname);
  WiFi.begin(SSID_WiFiManager, PASSWORD_WiFiManager);
  long last_time = millis();

  while (WiFi.status() != WL_CONNECTED) {
    delayMicroseconds(SECOND / 10);
    Serial.print(".");
    ESP.wdtFeed(); // Alimenta o watchdog
    if (millis() - last_time > 200) {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // Pisca o Led indicando que não conseguiu conectar
    }
  }
  Serial.println();
  Serial.print("Conectado com sucesso na rede: ");
  Serial.print(SSID_WiFiManager);
  Serial.println();
  Serial.print("IP obtido: ");
  Serial.print(WiFi.localIP());
  Serial.println();
  Serial.print("Endereço MAC: ");
  Serial.print(WiFi.macAddress());
}

//Função inicializa OTA - permite carga do novo programa via Wifi
void initOTA() {
  Serial.println();
  Serial.println("Iniciando OTA....");
  ArduinoOTA.setHostname("interrupções e whatchdog"); // Define o nome da porta
  ArduinoOTA.setPassword((const char *)PASS_OTA); // senha para carga via WiFi (OTA)
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  ESP.wdtFeed(); // Alimenta o watchdog
}

void ICACHE_RAM_ATTR interrupt_handler() {
  digitalWrite(LED_BUILTIN, HIGH);
  // piscar led 10 vezes
  for (int i = 0; i < 20; i++) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delayMicroseconds(1.5 * SECOND); // Inverter estado do LED
    ESP.wdtFeed(); // Alimenta o watchdog
    Serial.println("Interrupt");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando...");
  // IO Pins
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(InterruptPin, INPUT);
  // Interrupt attach
  attachInterrupt(digitalPinToInterrupt(InterruptPin), interrupt_handler, RISING);
  init_wifi_manager();
  initOTA();

  ESP.wdtDisable(); // Desativa o watchdog
  ESP.wdtFeed(); // Alimenta o watchdog
}
void loop() {
  long time = millis();
  if (time - last_blink > 100) {
    ESP.wdtFeed();
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // Piscar led
    Serial.println("Curso normal");
    last_blink = time;
  }

  reconect_wiFi();
  ArduinoOTA.handle();
}
