
#include <ESP8266WiFi.h> // Importa a Biblioteca ESP8266WiFi
#include <PubSubClient.h> // Importa a Biblioteca PubSubClient
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

// WIFI MANAGER
const char* SSID = "AcessoAoInferno"; // SSID / nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "er31j5yh"; // Senha da rede WI-FI que deseja se conectar

// MQTT
#define TOPICO_SUBSCRIBE_P1 "praticas-iot" //tópico MQTT de escuta luz 1

#define ID_MQTT  "id_unico" // ID unico no MQTT broker
#define USER_MQTT  "login"   // Usuario unico no MQTT broker
#define PASS_MQTT  "senha"  // senha no MQTT 

const char* BROKER_MQTT = "mqtt.eclipseprojects.io"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883; // Porta do Broker MQTT

// Função inicializa OTA - permite carga do novo programa via Wifi
void initOTA();

// Inicializa e conecta-se na rede WI-FI desejada
void initWiFi();

// Inicializa parâmetros de conexão MQTT(endereço do , porta e seta função de callback)
void initMQTT();

// Função de callback esta função é chamada toda vez que uma informação de um dos tópicos subescritos chega
void mqtt_callback(char* topic, byte* payload, unsigned int length);

// Reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
// em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito.
void reconnectMQTT();

// Reconecta-se ao WiFi
void reconectWiFi();

// Verifica o estado das conexões WiFI e ao broker MQTT.
// caso de desconexão (qualquer uma das duas), a conexão
// refeita.
void VerificaConexoesWiFIEMQTT();

//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient

void reconectWiFi()
{
  //se já está conectado a rede WI-FI, nada é feito.
  //Caso contrário, são efetuadas tentativas de conexão
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado com sucesso na rede: ");
  Serial.print(SSID);
  Serial.println();
  Serial.print("IP obtido: ");
  Serial.print(WiFi.localIP());  // mostra o endereço IP obtido via DHCP
  Serial.println();
  Serial.print("Endereço MAC: ");
  Serial.print(WiFi.macAddress()); // mostra o endereço MAC do esp8266
}

void initOTA()
{
  Serial.println();
  Serial.println("Iniciando OTA....");
  ArduinoOTA.setHostname("pratica-4"); // Define o nome da porta

  // No authentication by default
  ArduinoOTA.setPassword((const char *)"teste-ota"); // senha para carga via WiFi (OTA)
  // Senha
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


void initWiFi()
{
  delay(10);
  Serial.println("------Conexao WI-FI------");
  Serial.print("Conectando-se na rede: ");
  Serial.println(SSID);
  Serial.println("Aguarde");

  reconectWiFi();
}

void VerificaConexoesWiFIEMQTT(void)
{
    if (!MQTT.connected()) 
        reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
    
     reconectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

void setup() {
  Serial.begin(115200); // inicializa comunicação serial com baudrate 115200
  initWiFi();
  initOTA();
  initMQTT();
  // Caso necessario algo a mais para um programa expecifoco sera inserio aqui a baixo
}

void loop() {
  // Mantem conecção com o ota ativa
  ArduinoOTA.handle();
  // Funcionamento do MQTT
  VerificaConexoesWiFIEMQTT();
  //Mantem conecção com o MQTT
  MQTT.loop();
  // Caso necessario algo a mais para um programa expecifoco sera inserio aqui a baixo
}
