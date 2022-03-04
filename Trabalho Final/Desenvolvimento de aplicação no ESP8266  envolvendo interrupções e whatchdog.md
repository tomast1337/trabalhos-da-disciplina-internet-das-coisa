---
title: "Desenvolvimento de aplicação no ESP8266 envolvendo interrupções e Whatchdog"
author: Nicolas Vycas Nery
output: pdf_document
---

# Desenvolvimento de aplicação no ESP8266 envolvendo interrupções e Whatchdog

### Professor: Carlos Otávio Schocair Mendes

### Aluno: Nicolas Vycas Nery

## Introdução


## Oque são as interrupções?

Durante o nosso curso nos vimos diversas 

## Oque é Watchdog?

Watchdog é um contador separado do clock principal ou seja se o programa principal falhar o Watchdog continua rodando, e na finalização do watchdog o ESP8266 é reiniciado. O caso de utilização do Watchdog é em caso de se caso o programa falhar o ESP8266 é reiniciado, assim evitando uma falha maior, por exemplo um medidor de temperatura de um forno ou uma bomba d'água que se falharem algo pode pegar fogo ou uma caixa d'água pode transbordar.
 
Por padrão o ESP8266 ja tem um sistema que realiza a manutenção dos sistemas de rede e outras tarefas, e que quando o sistema falha ele é reiniciado. em casos onde temos um função muito demorada esse irá reiniciar o sistema, então em funções demoradas temos que chamar a função `yield();` para manter a atualização das funcionalidades vitais do esp8266. A função `delay();` também tem um função parecida com a função `yield();` com a diferença de que le causa um delay, existem a função `delayMicroseconds();` que le causa um delay de microssegundos sem manter as funções vitais do ESP8266. 

Observe os dois códigos abaixo e note duas diferenças, um utiliza o `delay();` e o outro utiliza o `delayMicroseconds();`.

### 1) Código com `delay();`

```c
#define LED D4

#define SECOND 1000

void setup(){
    Serial.begin(115200); // Inicia porta serial
    
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

```c
#define LED D4

#define SECOND 1000

void setup(){
    Serial.begin(115200); // Inicia porta serial

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

Ao serem executados o primeiro, 1, código irar piscar o led 5 vezes, delay de 2 segundos, dentro do `void setup()` e depois entra em `void loop()` onde o led pisca rapidamente, lento, delay de um decimo de segundos. O segundo, 2, código irá piscar o led infinitamente lento e nunca sairá de `void setup()` porque o ESP8266 sempre estará sendo reiniciando pelo watchdog, ou seja o watchdog sempre irá reiniciar o ESP8266.  

## Exemplo de utilização do Watchdog 



