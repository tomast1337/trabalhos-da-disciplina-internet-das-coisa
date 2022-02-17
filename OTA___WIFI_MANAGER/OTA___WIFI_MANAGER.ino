#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>

//Mapeamento de pinos do NodeMCU
#define D0    16
#define D1    5
#define D2    4
#define D3    0
#define D4    2
#define D5    14
#define D6    12
#define D7    13
#define D8    15
#define D9    3
#define D10   1

#define TOPICO_SUBSCRIBE_P1 "ESP8266_LED/LED"     //tópico MQTT de escuta luz 1
#define ID_MQTT  "mqtt_aula_nicolas_vycas_1"//id mqtt (para identificação de sessão deve ser unico)
#define USER_MQTT  "login"   // usuario no MQTT
#define PASS_MQTT  "12345"  // senha no MQTT 

#define PASS_OTA "teste-ota"

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
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void VerificaConexoesMQTT();

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
  MQTT.setCallback(mqtt_callback);            //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}

void mqtt_callback(char* topic, byte* payload, unsigned int length){
    String msg;
    //Obtendo a mensagem
    for (int i = 0; i < length; i++) {
      char c = (char)payload[i];
      msg += c;
    }
    Serial.print(msg);
    Serial.print("\n");
    if (msg.equals("LED - TOGGLE")) { // CASO A MENSAGEM SEJA "LED - TOGGLE"
      Serial.print("toggled");
      Serial.print("\n");
      digitalWrite(D0, !digitalRead(D0)); //Alterna o estado do LED
    }
}

void VerificaConexoesMQTT(){
    if (!MQTT.connected()) { //se não há conexão com o Broker, a conexão é refeita
        while (!MQTT.connected()){
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        //        if (MQTT.connect(ID_MQTT, USER_MQTT,PASS_MQTT)) // parameros usados para broker proprietário
        // ID do MQTT, login do usuário, senha do usuário
    
        if (MQTT.connect(ID_MQTT)) {
          Serial.println("Conectado com sucesso ao broker MQTT!");
          MQTT.subscribe(TOPICO_SUBSCRIBE_P1);
        } else {
          Serial.println("Falha ao reconectar no broker.");
          Serial.println("Havera nova tentatica de conexao em 2s");
          delay(2000);
        }
      }
    }
}

void setup(){
    Serial.begin(115200); // Inicia porta serial
    init_wifi_manager(); // Inicia o WiFiManager
    initOTA(); // Inicia OTA

    initMQTT();

    // Iniciar pins
    pinMode(D0, OUTPUT); // Ultilisaremos esse pin para ligar o LED
    digitalWrite(D0, LOW); //Baixo no pin D0
}

void loop(){
    reconect_wiFi(); // Verifica se a rede está conectada
    ArduinoOTA.handle(); // keep-alive da comunicação OTA
    MQTT.loop(); //keep-alive da comunicação com broker MQTT
}
