#include <ESP32Servo.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Código 1
Servo servo1;                // Cria um objeto Servo
const int servoPin1 = 13;    // Pino onde o servo está conectado
const int buttonPin1 = 2;    // Pino onde o botão está conectado
int buttonState1 = 0;        // Variável para armazenar o estado do botão
bool servoAtInitialPosition1 = true; // Variável para verificar se o servo está na posição inicial
int servoPosition1 = 140;      // Variável para controlar a posição do servo

// Código 2
Servo servo2;                // Cria um objeto Servo
const int servoPin2 = 14;    // Pino onde o servo está conectado
const int buttonPin2_180 = 4;  // Pino onde o botão 2 está conectado
const int buttonPin2_0 = 18;    // Pino onde o botão 3 está conectado
int buttonState2_180 = 0;     // Variável para armazenar o estado do botão 2
int buttonState2_0 = 0;       // Variável para armazenar o estado do botão 3
int servoPosition2 = 0;       // Variável para controlar a posição do servo

void task1(void *parameter) {
  servo1.attach(servoPin1);  // Anexa o servo ao pino 13
  pinMode(buttonPin1, INPUT_PULLDOWN);  // Define o pino do botão como entrada com pull-down interno
  servo1.write(servoPosition1);  // Define a posição inicial do servo como 140 graus

  for (;;) {
    buttonState1 = digitalRead(buttonPin1);  // Lê o estado do botão

    if (Serial.available() > 0) { // Verifica se há dados disponíveis para leitura no serial
      int data = Serial.read(); // Lê o dado serial
      // Mova o servo para a posição recebida
      servo1.write(data);
    }

    if (buttonState1 == HIGH) {  // Verifica se o botão foi pressionado
      delay(50);  // Debounce do botão
      if (digitalRead(buttonPin1) == HIGH) {
        if (servoAtInitialPosition1) {
          for (int pos = servoPosition1; pos >= 0; pos -= 1) { // Move o servo gradualmente para 0 graus
            servo1.write(pos);
            delay(50); // Ajuste o atraso para controlar a velocidade do movimento
          }
        } else {
          for (int pos = servoPosition1; pos <= 140; pos += 1) { // Move o servo gradualmente para 140 graus
            servo1.write(pos);
            delay(50); // Ajuste o atraso para controlar a velocidade do movimento
          }
        }
        servoPosition1 = 140 - servoPosition1; // Inverte a posição do servo
        servoAtInitialPosition1 = !servoAtInitialPosition1; // Alterna o estado do servo
        while (digitalRead(buttonPin1) == HIGH) {}  // Aguarda até que o botão seja liberado
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS); // Pequeno delay para evitar a sobrecarga da CPU
  }
}

void task2(void *parameter) {
  servo2.attach(servoPin2);  // Anexa o servo ao pino 2
  pinMode(buttonPin2_180, INPUT_PULLDOWN);  // Define o pino do botão 2 como entrada com pull-down interno
  pinMode(buttonPin2_0, INPUT_PULLDOWN);    // Define o pino do botão 3 como entrada com pull-down interno
  servo2.write(servoPosition2);  // Define a posição inicial do servo como 0 graus

  for (;;) {
    buttonState2_180 = digitalRead(buttonPin2_180);  // Lê o estado do botão 2
    buttonState2_0 = digitalRead(buttonPin2_0);      // Lê o estado do botão 3

    if (buttonState2_180 == HIGH) {  // Verifica se o botão 2 foi pressionado
      Serial.println("Botão 2 (180º) pressionado"); // Mensagem de depuração
      if (servoPosition2 < 180) {
        servoPosition2 += 1;  // Incrementa a posição do servo
        servo2.write(servoPosition2);
        delay(25); // Ajuste o atraso para controlar a velocidade do movimento
      }
    }

    if (buttonState2_0 == HIGH) {  // Verifica se o botão 2 foi pressionado
      Serial.println("Botão 3 (0º) pressionado"); // Mensagem de depuração
      if (servoPosition2 > 0) {
        servoPosition2 -= 1;  // Decrementa a posição do servo
        servo2.write(servoPosition2);
        delay(25); // Ajuste o atraso para controlar a velocidade do movimento
      }
    }

    vTaskDelay(10 / portTICK_PERIOD_MS); // Pequeno delay para evitar a sobrecarga da CPU
  }
}

void setup() {
  Serial.begin(115200); // Inicia a comunicação serial

  // Cria as tarefas para cada código nos diferentes núcleos
  xTaskCreatePinnedToCore(
    task1,     // Função da tarefa
    "Task1",   // Nome da tarefa
    10000,     // Tamanho da pilha (em palavras)
    NULL,      // Parâmetro para passar para a tarefa
    1,         // Prioridade da tarefa
    NULL,      // Handle da tarefa (opcional)
    0);        // Núcleo onde a tarefa será executada

  xTaskCreatePinnedToCore(
    task2,     // Função da tarefa
    "Task2",   // Nome da tarefa
    10000,     // Tamanho da pilha (em palavras)
    NULL,      // Parâmetro para passar para a tarefa
    1,         // Prioridade da tarefa
    NULL,      // Handle da tarefa (opcional)
    1);        // Núcleo onde a tarefa será executada
}

void loop() {
  // Deixe o loop vazio, pois as tarefas estão sendo executadas nos núcleos
}
