#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <DHT.h>

//Mapeamento de pinos do NodeMCU
#define D3    0

#define ID_MQTT  "mqtt_aula_nicolas_vycas_1"//id mqtt (para identificação de sessão deve ser unico)
#define USER_MQTT  "login_nicolas_nery"   // usuario no MQTT
#define PASS_MQTT  "987654321"  // senha no MQTT 

#define PASS_OTA "senha-ota"

#define DHT_TYPE DHT11

#define INTERVALO_MEDICAO_DHT 10*1000 // 10 segundos

int ultima_medicao_dht = 0;

DHT dht(D3, DHT_TYPE);

WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient

// MQTT
const char* BROKER_MQTT = "mqtt.eclipseprojects.io"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883; // Porta do Broker MQTT

// WIFI MANAGER
const char *myHostname = "nodeMCU"; // Nome do host na rede
const char *SSID_WiFiManager = "RedeIOT";       // SSID / nome da rede WI-FI (AP) do WiFiManager
const char *PASSWORD_WiFiManager = "12345678";  // Senha da rede WI-FI (AP) do WiFiManager

//Prototypes
void init_wifi_manager();
void reconect_wiFi();
void initOTA();

void initMQTT();
void VerificaConexoesMQTT();
void enviaDHT();

// função mostra inicialização do wifi manager
void init_wifi_manager() {
    delay(10);
    WiFi.hostname(myHostname);
    WiFiManager wifiManager;
    wifiManager.autoConnect(SSID_WiFiManager, PASSWORD_WiFiManager);
    Serial.println("\n------Conexao WI-FI Manager------\n");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID_WiFiManager);
    Serial.print("Senha: ");
    Serial.println(PASSWORD_WiFiManager);
    reconect_wiFi();
}

// Função verifica se a rede está conectada, se não estiver, ativa o WiFiManager
void reconect_wiFi(){
    if (WiFi.status() == WL_CONNECTED){  // se conectado retorna
        return;
    }
    WiFi.hostname(myHostname); // define o nome do dispositivo na rede 
    WiFi.begin(SSID_WiFiManager, PASSWORD_WiFiManager); // Conecta na rede WI-FI
    while (WiFi.status() != WL_CONNECTED) { // entra em loop aguardando conexão
        delay(100); // Aguarda 100 ms
        Serial.print(".");
    }
    // Conectado com sucesso
    Serial.println();
    Serial.print("Conectado com sucesso na rede: ");
    Serial.print(SSID_WiFiManager);
    Serial.println();
    Serial.print("IP obtido: ");
    Serial.print(WiFi.localIP());  // mostra o endereço IP obtido via DHCP
    Serial.println();
    Serial.print("Endereço MAC: ");
    Serial.print(WiFi.macAddress()); // mostra o endereço MAC do esp8266
}

//Função inicializa OTA - permite carga do novo programa via Wifi
void initOTA(){
  Serial.println();
  Serial.println("Iniciando OTA....");
  ArduinoOTA.setHostname("pratica-4"); // Define o nome da porta
  // No authentication by default
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
}

void initMQTT(){
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
}

void VerificaConexoesMQTT(){
    if (!MQTT.connected()) { //se não há conexão com o Broker, a conexão é refeita
        while (!MQTT.connected()){
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        //        if (MQTT.connect(ID_MQTT, USER_MQTT,PASS_MQTT)) // parameros usados para broker proprietário
        if (MQTT.connect(ID_MQTT)) {
          Serial.println("Conectado com sucesso ao broker MQTT!");
        } else {
          Serial.println("Falha ao reconectar no broker.");
          Serial.println("Havera nova tentatica de conexao em 2s");
          delay(2000);
        }
      }
    }
}

void enviaDHT(){
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (isnan(h) || isnan(t)) {
      Serial.println("Falha ao ler o sensor DHT11");
    } else {
      Serial.print("Temperatura: ");
      Serial.print(t);
      Serial.print(" *C, ");
      Serial.print("Umidade: ");
      Serial.print(h);
      Serial.println("%");
      MQTT.publish("UMIDADE/DHT11","%f", h);
      MQTT.publish("TEMPERATURA/DHT11","%f", t);
    }
}

void setup(){
    Serial.begin(115200); // Inicia porta serial
    init_wifi_manager(); // Inicia o WiFiManager
    initOTA(); // Inicia OTA
    initMQTT();
    dht.begin(); // Inicia o sensor DHT11
}

void loop() {
    reconect_wiFi(); // Verifica se a rede está conectada
    ArduinoOTA.handle(); // keep-alive da comunicação OTA
    MQTT.loop(); //keep-alive da comunicação com broker MQTT
    if (millis() - ultima_medicao_dht >= INTERVALO_MEDICAO_DHT) {
      enviaDHT();
      ultima_medicao_dht = millis();
    }
}
