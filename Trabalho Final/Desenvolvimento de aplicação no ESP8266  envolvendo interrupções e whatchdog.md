---
title: "Desenvolvimento de aplicação no ESP8266 envolvendo Interrupções e Watchdog"
author: Nicolas Vycas Nery
output: pdf_document
margin: 1cm
---

# Desenvolvimento de aplicação no ESP8266 envolvendo Interrupções e Watchdog

### Professor: Carlos Otávio Schocair Mendes

### Aluno: Nicolas Vycas Nery

### Conteúdo:

* Introdução
* Watchdog
* Interrupções
* Programa utilizando o Watchdog, Interrupção , Wifimanager e OTA
* Bibliografia

## Introdução

A todas as aplicações utilizando o ESP8266, precisão de um sistema de manutenção para que no caso de algum problema o sistema possa ser reiniciado, ou existir alguma função que de um tratamento a uma situação critica, independente do curso principal da aplicação. Para isso existe o Watchdog, e as interrupções.

## Watchdog

Watchdog é um contador separado do clock principal ou seja se o programa principal travar o Watchdog reinicia o dispositivo.

O caso de utilização do Watchdog é em caso de se caso o programa travar o ESP8266 é reiniciado, assim evitando uma falha maior, por exemplo um medidor de temperatura de um forno ou uma bomba d'água que se falharem algo pode pegar fogo ou uma caixa d'água pode transbordar.

Por padrão o ESP8266 ja tem um sistema que realiza a manutenção dos sistemas de rede e outras tarefas através do Hardware Watchdog ,`HW WDT` , e que quando o sistema trava ele é reiniciado. em casos onde temos um função muito demorada esse irá reiniciar o sistema, então em funções demoradas temos que chamar a função `yield();` para manter a atualização das funcionalidades do `HW WDT`. A função `delay();` também tem um função parecida com a função `yield();`,ou seja ela mantém o `HW WDT`, com a diferença de que a função causa um delay, existem a função `delayMicroseconds();` que le causa um delay de microssegundos sem manter o `HW WDT`.

Observe os dois códigos abaixo e note duas diferenças, um utiliza o `delay();` e o outro utiliza o `delayMicroseconds();`.

### 1) Código com `delay();`

```cpp
#define LED D4

#define SECOND 1000

void setup(){
    Serial.begin(115200);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, 1);
    delay(SECOND);
    for (int i = 0; i < 10; i++) {
        digitalWrite(LED, !digitalRead(LED));
        delay(2*SECOND);
    }
}

void loop() {
    digitalWrite(LED, !digitalRead(LED));
    delay(SECOND/10);
}
```

### 2) Código com `delayMicroseconds();`

```cpp
#define LED D4

#define SECOND 1000

void setup(){
    Serial.begin(115200);
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
```

Ao serem executados o primeiro, 1, código irar piscar o LED 5 vezes, delay de 2 segundos, dentro do `void setup()` e depois entra em `void loop()` onde o LED pisca rapidamente, lento, delay de um decimo de segundos. O segundo, 2, código irá piscar o LED infinitamente lento e nunca sairá de `void setup()` porque o ESP8266 sempre estará sendo reiniciando pelo watchdog, ou seja o watchdog sempre irá reiniciar o ESP8266.

Vamos agora adicionar o watchdog manualmente, utilizando as funções `ESP.wdtDisable();` para desativar o watchdog e `ESP.wdtFeed();` para alimentar o watchdog, assim reiniciando o timer do watchdog e evitando que o ESP8266 seja reiniciado.

```cpp
#define LED D4

#define SECOND 1000

void setup(){
    Serial.begin(115200);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, 1);
    ESP.wdtDisable(); // Desativa o watchdog
    delayMicroseconds(SECOND);
    ESP.wdtFeed(); // Alimenta o watchdog
    for (int i = 0; i < 10; i++) {
        digitalWrite(LED, !digitalRead(LED));
        delayMicroseconds(2*SECOND*1000);
        ESP.wdtFeed(); // Alimenta o watchdog
    }
}

void loop() {
    digitalWrite(LED, !digitalRead(LED));
    delayMicroseconds(SECOND*100);
    ESP.wdtFeed(); // Alimenta o watchdog
}
```

Agora o código irar se comportar similar ao primeiro código que utilizava o `delay();`, o ESP8266 não sera reiniciado pelo watchdog. Com isso podemos controlar o watchdog manualmente, e podemos definir pontos de checagem em nosso programa, onde caso ele não seja alcançado a tempo o Watchdog ira reiniciar o ESP8266.

### Interrupções

Interrupções é uma condição que faz com que o microprocessador trabalhe em uma tarefa diferente do que está sendo executado, temporariamente, retornando ao processador principal, quando a interrupção e concluída. As interrupções podem ser internas, software, ou externas ,hardware.
As interrupções de hardware são causadas por uma requisição de I/O, por uma falha de hardware, e não causam a parada imediata do programa principal.
As interrupções de software são causadas por funções INT, no momento da interrupção, o processado para para imediatamente o programa principal e pula para o código de manuseio da interrupção, apos isso o processo volta ao programa principal.
No ESP8266 s interrupções não podem ser feitas em loops pois elas iriam acionar o Watchdog oq iriam reiniciar o dispositivo. Dentro das interrupções não podemos utilizar funções que alimentam o Watchdog, isso sendo uma limitação do ESP8266.

Existem 3 tipos de interrupções:

- RISING: Interrupção que acontece quando o pino está em HIGH
- FALLING: Interrupção que acontece quando o pino está em LOW
- CHANGE: Interrupção que acontece quando o pino está em HIGH ou LOW

##### Exemplo utilizando interrupção externa:

```cpp
#define INTERRUPT_PIN D2

#define LED D1

// Tempo desde o ultimo pulso/interrupção
long last_interrupt_time = 0;

// Função que será chamada quando a interrupção for disparada
void ICACHE_RAM_ATTR interrupt_handler() { 
  long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 1000) {
    digitalWrite(LED, !digitalRead(LED));
    last_interrupt_time = interrupt_time;
  }
}

void setup() {
  pinMode(INTERRUPT_PIN, INPUT);
  pinMode(LED, OUTPUT);
  // Configura a interrupção do pino D2/INTERRUPT_PIN como RISING (LOW -> HIGH)
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), interrupt_handler, RISING);
}

void loop(){
    delay(1000);
}
```

Com o exemplo acima, a função `interrupt_handler()` será chamada quando o pino D2 for pressionado, ou seja, quando o pino D2 for acionado, o LED será acionado. Observe que não precisamos definir nada dentro no nosso loop, para verificar o estado do pino D2, pois o ESP8266 irá fazer isso automaticamente.

O `ICACHE_RAM_ATTR` é uma flag que indica que a função é uma função de interrupção, e que ela é chamada a partir da memoria RAM, existe também `ICACHE_FLASH_ATTR` que indica que a função de interrupção é chamada a partir da memoria Flash.

### Programa utilizando o Watchdog, Interrupção , Wifimanager e OTA


Vamos começar utilizando o programa abaxio, que utiliza o Watchdog, OTA e Wifimanager.
```cpp
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>

// WIFI MANAGER
const char *myHostname = "nodeMCU"; // Nome do host na rede
const char *SSID_WiFiManager = "RedeIOT";       // SSID / nome da rede WI-FI (AP) do WiFiManager
const char *PASSWORD_WiFiManager = "12345678";  // Senha da rede WI-FI (AP) do WiFiManager

void init_wifi_manager();
void reconect_wiFi();
void initOTA();

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
    if (WiFi.status() == WL_CONNECTED)
        return;
    WiFi.hostname(myHostname);
    WiFi.begin(SSID_WiFiManager, PASSWORD_WiFiManager);
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
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
void initOTA(){
  Serial.println();
  Serial.println("Iniciando OTA....");
  ArduinoOTA.setHostname("pratica-4"); // Define o nome da porta
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
    if (error == OTA_AUTH_ERROR) Serial.println("Auth FaiLED");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin FaiLED");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect FaiLED");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive FaiLED");
    else if (error == OTA_END_ERROR) Serial.println("End FaiLED");
  });
  ArduinoOTA.begin();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando...");
  init_wifi_manager();
  initOTA();
}
void loop() {
  reconect_wiFi();
  ArduinoOTA.handle();
}
```

Vamos agora adicionar essas definições no arquivo e a função `void ICACHE_RAM_ATTR interrupt_handler()` ao programa, vamos criar também a variável global `long last_blink = 0;` que ira armazenar o tempo do ultimo piscar do LED, no curso principal do programa.
```cpp

#define LED_BUILTIN D4
#define InterruptPin D3
#define SECOND 1000000

long last_blink = 0;
long state = 100;

// prototypes
void ICACHE_RAM_ATTR interrupt_handler(); // Interrupt handler function prototype
void init_wifi_manager();
void reconect_wiFi();
void initOTA();

void ICACHE_RAM_ATTR interrupt_handler() {
  digitalWrite(LED_BUILTIN, HIGH);
  // piscar LED 20 vezes
  for (int i = 0; i < 20; i++) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delayMicroseconds(1.5 * SECOND); // Inverter estado do LED
    ESP.wdtFeed(); // Alimenta o watchdog
    Serial.println("Interrupt");
  }
}

```

Vamos alterar as funções `void setup()` e `void loop()` para adicionar a definição da função de interrupção e adicionar o WatchDog Manual.

```cpp
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
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // Piscar LED
    Serial.println("Curso normal");
    last_blink = time;
  }

  reconect_wiFi();
  ArduinoOTA.handle();
}
```

E adicionar o WatchDog as funções `void init_wifi_manager();`, `void reconect_wiFi();` e `void initOTA();`.

```cpp
void init_wifi_manager() {
    delayMicroseconds(SECOND/100);
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

void reconect_wiFi(){
    if (WiFi.status() == WL_CONNECTED){
        ESP.wdtFeed();
        return;
    }
        
    WiFi.hostname(myHostname);
    WiFi.begin(SSID_WiFiManager, PASSWORD_WiFiManager);
    while (WiFi.status() != WL_CONNECTED) {
        delayMicroseconds(SECOND/10);
        Serial.print(".");
        ESP.wdtFeed(); // Alimenta o watchdog
        if (millis() - last_time > 200) {
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));// Pisca o Led indicando que não conseguiu conectar
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

void initOTA(){
  Serial.println();
  Serial.println("Iniciando OTA....");
  ArduinoOTA.setHostname("pratica-4"); // Define o nome da porta
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
    if (error == OTA_AUTH_ERROR) Serial.println("Auth FaiLED");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin FaiLED");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect FaiLED");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive FaiLED");
    else if (error == OTA_END_ERROR) Serial.println("End FaiLED");
  });
  ArduinoOTA.begin();
  ESP.wdtFeed(); // Alimenta o watchdog
}
```
O programa tem o seguinte comportamento, o wifi manager e iniciado e a pois conectado a uma redo o programa entrara em loop, onde ele pisca o LED e um ritmo rápido e verifica se o wifi esta conectado, se não estiver ele entra em um loop para tentar reconectar e enquanto ele estiver tentando reconectar ele pisca o LED em um ritmo mais acelerado, e durante qualquer momento do programa pode ocorrer uma interrupção externa onde o led piscara 20 vezes lento. Esse programa tempo o potencial de ser expandido para um dispositivo IOT talvez adicionado algum censor ao pino de interrupção que desencadeia um envio de mensagens par ao broker além de poder rodar outra tarefa no loop principal, talvez um controle de ladão e bomba de caixa d'água. 

### Bibliografia

##### Watchdog

- https://en.wikibooks.org/wiki/Embedded_Systems/Watchdog_Timer
- https://en.wikibooks.org/wiki/Embedded_Control_Systems_Design/Design_Patterns#Watchdog_timer

##### Interrupções

- https://en.wikibooks.org/wiki/Microprocessor_Design/Interrupts
- https://www.geeksforgeeks.org/difference-between-hardware-interrupt-and-software-interrupt/
- https://en.wikibooks.org/wiki/Embedded_Systems/Interrupts
- https://en.wikibooks.org/wiki/X86_Assembly/X86_Interrupts
