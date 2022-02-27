
#include <ESP8266WiFi.h>  // Inclui a biblioteca

//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient

// WIFI
const char* myHostname = "escolha_um_nome"; // Nome do host na rede
const char* SSID = "TESTE"; // SSID / nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "teste123"; // Senha da rede WI-FI que deseja se conectar
 
void initWiFi() 
{
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
    
    reconectWiFi();
}


void reconectWiFi() 
{
    //se já está conectado a rede WI-FI, nada é feito. 
    //Caso contrário, são efetuadas tentativas de conexão
    if (WiFi.status() == WL_CONNECTED)  // se conectado retorna
        return;
    
    WiFi.hostname(myHostname); // define o nome do dispositivo na rede 
                               // caso não seja definido o host assumirá o nome padrão do dispositivo   
    WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
    
    while (WiFi.status() != WL_CONNECTED)  // entra no loop do while até se conectar
    {
        delay(100); // Aguarda 100 ms
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


void setup() {
Serial.begin(115200); // observe a velocidade de conexão com o terminal
initWiFi();
}

void loop() {
  
reconectWiFi(); //se não há conexão com o WiFI, a conexão é refeita

}
