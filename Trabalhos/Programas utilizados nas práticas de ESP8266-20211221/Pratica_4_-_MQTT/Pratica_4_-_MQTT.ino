#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>

//defines - mapeamento de pinos do NodeMCU
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

// WIFI MANAGER
const char *myHostname = "nodeMCU"; // Nome do host na rede
const char *SSID_WiFiManager = "RedeITO";       // SSID / nome da rede WI-FI (AP) do WiFiManager
const char *PASSWORD_WiFiManager = "12345678";  // Senha da rede WI-FI (AP) do WiFiManager

// WIFI
const char* SSID = "AcessoAoInferno"; // SSID / nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "er31j5yh"; // Senha da rede WI-FI que deseja se conectar

// MQTT
const char* BROKER_MQTT = "mqtt.eclipseprojects.io"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883; // Porta do Broker MQTT


//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient


//Prototypes"
void initSerial();
void initWiFi();
void initOTA();
void initMQTT();
void reconectWiFi();
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void VerificaConexoesWiFIEMQTT(void);
void InitOutput(void);


void setup(){
  //inicializações:
  Serial.begin(115200);
  initWiFi();
  initOTA();
  initMQTT();

  // Iniciar pins
  pinMode(D0, OUTPUT); // Ultilisaremos esse pin para ligar o LED
  digitalWrite(D0, LOW); //Baixo no pin D0
}


//Função: inicializa e conecta-se na rede WI-FI desejada
//Parâmetros: nenhum
//Retorno: nenhum
void initWiFi(){
    WiFi.hostname(myHostname);
    WiFiManager wifiManager;

    wifiManager.autoConnect(SSID, PASSWORD);
    delay(10);
    Serial.print("Conectado com sucesso na rede via WifiManager na rede: ");
    Serial.println(WiFi.SSID());
    Serial.println();
    Serial.print("IP obtido: ");
    Serial.print(WiFi.localIP()); // mostra o endereço IP obtido via DHCP
    Serial.println();
    Serial.print("Endereço MAC: ");
    Serial.print(WiFi.macAddress()); // mostra o endereço MAC do esp8266
    reconectWiFi();
}

//Função inicializa OTA - permite carga do novo programa via Wifi
void initOTA()
{
  Serial.println();
  Serial.println("Iniciando OTA....");
  ArduinoOTA.setHostname("pratica-4"); // Define o nome da porta

  // No authentication by default
  ArduinoOTA.setPassword((const char *)"teste-ota"); // senha para carga via WiFi (OTA)
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

//Função: inicializa parâmetros de conexão MQTT(endereço do
//        broker, porta e seta função de callback)
//Parâmetros: nenhum
//Retorno: nenhum
void initMQTT()
{
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
  MQTT.setCallback(mqtt_callback);            //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}

//Função: função de callback
//        esta função é chamada toda vez que uma informação de
//        um dos tópicos subescritos chega)
//Parâmetros: nenhum
//Retorno: nenhum
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
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


//Função: reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
//        em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito.
//Parâmetros: nenhum
//Retorno: nenhum
void reconnectMQTT()
{
  while (!MQTT.connected())
  {
    Serial.print("* Tentando se conectar ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    //        if (MQTT.connect(ID_MQTT, USER_MQTT,PASS_MQTT)) // parameros usados para broker proprietário
    // ID do MQTT, login do usuário, senha do usuário

    if (MQTT.connect(ID_MQTT)) {
      Serial.println("Conectado com sucesso ao broker MQTT!");
      MQTT.subscribe(TOPICO_SUBSCRIBE_P1);

    }
    else {
      Serial.println("Falha ao reconectar no broker.");
      Serial.println("Havera nova tentatica de conexao em 2s");
      delay(2000);
    }
  }
}


//Função: reconecta-se ao WiFi
//Parâmetros: nenhum
//Retorno: nenhum
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


//Função: verifica o estado das conexões WiFI e ao broker MQTT.
//        Em caso de desconexão (qualquer uma das duas), a conexão
//        é refeita.
//Parâmetros: nenhum
//Retorno: nenhum
void VerificaConexoesWiFIEMQTT(void) {
  if (!MQTT.connected())
    reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
}

//programa principal
void loop() {
  // keep-alive da comunicação OTA
  ArduinoOTA.handle();
  //garante funcionamento das conexões WiFi e ao broker MQTT
  VerificaConexoesWiFIEMQTT();
  //keep-alive da comunicação com broker MQTT
  MQTT.loop();
}
