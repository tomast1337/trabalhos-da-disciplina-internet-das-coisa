---
title: "Desenvolvimento de aplicação no ESP8266 envolvendo Interrupções e Watchdog"
author: Nicolas Vycas Nery
output: pdf_document
---

# Desenvolvimento de aplicação no ESP8266 envolvendo Interrupções e Watchdog

### Professor: Carlos Otávio Schocair Mendes

### Aluno: Nicolas Vycas Nery

## Introdução

A todas as aplicações utilizando o ESP8266, precisão de um sistema de manutenção para que no caso de algum problema o sistema possa ser reiniciado, ou existir alguma função que de um tratamento a uma situação critica, independente do curso principal da aplicação. Para isso existe o Watchdog, e as interrupções.

## Oque é Watchdog?

Watchdog é um contador separado do clock principal ou seja se o programa principal travar o Watchdog continua rodando, e na finalização do watchdog o ESP8266 é reiniciado.

O caso de utilização do Watchdog é em caso de se caso o programa travar o ESP8266 é reiniciado, assim evitando uma falha maior, por exemplo um medidor de temperatura de um forno ou uma bomba d'água que se falharem algo pode pegar fogo ou uma caixa d'água pode transbordar.

Por padrão o ESP8266 ja tem um sistema que realiza a manutenção dos sistemas de rede e outras tarefas através do  Hardware Watchdog ,`HW WDT` , e que quando o sistema trava ele é reiniciado. em casos onde temos um função muito demorada esse irá reiniciar o sistema, então em funções demoradas temos que chamar a função `yield();` para manter a atualização das funcionalidades do `HW WDT`. A função `delay();` também tem um função parecida com a função `yield();`,ou seja ela mantém o `HW WDT`, com a diferença de que a função causa um delay, existem a função `delayMicroseconds();` que le causa um delay de microssegundos sem manter o `HW WDT`.  

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

## Oque são as Interrupções?

As interrupções não podem ser feitas em loops pois elas iriam acionar o Watchdog oq iriam reiniciar o ESP8266.  Dentro das interrupções não podemos utilizar funções que atualizam o Watchdog, isso sendo uma limitação do ESP8266.

Existem 3 tipos de interrupções:
- RISING: Interrupção que acontece quando o pino está em HIGH
- FALLING: Interrupção que acontece quando o pino está em LOW
- CHANGE: Interrupção que acontece quando o pino está em HIGH ou LOW

Exemplo utilizando interrupções:

```c
#define INTERRUPT_PIN D2
#define LED D1

long last_interrupt_time = 0;

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
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), interrupt_handler, RISING);
}

void loop(){
    delay(1000);
}
```

Com o exemplo acima, a função `interrupt_handler()` será chamada quando o pino D2 for pressionado, ou seja, quando o pino D2 for acionado, o LED será acionado. Observe que não precisamos definir nada dentro no nosso loop, para verificar o estado do pino D2, pois o ESP8266 irá fazer isso automaticamente.

O ICACHE_RAM_ATTR é uma flag que indica que a função é uma função de interrupção, ou seja, é uma função que é chamada quando o ESP8266 recebe uma interrupção.

# Conclusão