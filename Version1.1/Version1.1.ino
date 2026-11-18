
#include <LiquidCrystal_I2C.h>

// ================================================
// MONITOR DE PLANTA IoT
// VERSAO 1.0.0 - SISTEMA INICIAL
// ================================================

// Biblioteca responsavel pelo funcionamento
// do display LCD utilizando comunicacao I2C.
LiquidCrystal_I2C lcd(0x27, 16, 2);


// ================================================
// CONFIGURACAO DOS COMPONENTES
// ================================================

// SENSOR DE UMIDADE
// O sensor realiza a leitura da umidade presente
// no solo e envia um valor analogico para o Arduino.
const int SENSOR_PIN = A0;

// Valores utilizados para converter a leitura
// analogica em uma porcentagem de umidade.
const int SENSOR_MIN_VALUE = 0;
const int SENSOR_MAX_VALUE = 500;


// LED RGB
// Utilizado para representar visualmente o estado
// da planta atraves das cores verde, amarelo e vermelho.
const int RED_PIN = 11;
const int GREEN_PIN = 9;
const int BLUE_PIN = 10;


// LED PISCANTE
// Utilizado como indicador visual de alerta.
const int LED_PIN = 6;


// BUZZER
// Responsavel por emitir alertas sonoros quando
// a umidade do solo estiver baixa.
const int BUZZER_PIN = 5;


// SENSOR ULTRASSONICO HC-SR04
// Detecta a presenca de um objeto atraves da
// distancia entre o sensor e o objeto detectado.
const int TRIG_PIN = 8;
const int ECHO_PIN = 7;

// Distancia utilizada para controlar o buzzer.
const int DISTANCIA_LIMITE = 30;


// ================================================
// VARIAVEIS DE CONTROLE
// ================================================

// Armazenam o ultimo momento em que os LEDs
// foram atualizados.
unsigned long lastBlinkTime = 0;
unsigned long lastRgbUpdate = 0;

// Intervalos de funcionamento dos LEDs.
const int BLINK_SLOW = 1000;
const int BLINK_FAST = 200;
const int RGB_UPDATE_INTERVAL = 100;

// Armazena o estado atual do LED piscante.
// TRUE = ligado / FALSE = desligado.
bool ledState = false;


// ================================================
// SETUP - CONFIGURACAO INICIAL
// ================================================

void setup() {

  // Configura os pinos do LED RGB como saidas.
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // Configura o LED e o buzzer como saidas.
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Configura o sensor ultrassonico.
  // TRIG envia o sinal e ECHO recebe o retorno.
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Inicializa o LCD e ativa sua iluminacao.
  lcd.init();
  lcd.backlight();

  // Exibe uma mensagem de inicializacao.
  lcd.setCursor(0, 0);
  lcd.print("Monitor Planta");

  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");

  // Aguarda dois segundos antes de iniciar
  // o monitoramento da planta.
  delay(2000);

  lcd.clear();
}


// ================================================
// LOOP - EXECUCAO PRINCIPAL
// ================================================

void loop() {

  // ----------------------------------------------
  // 1. LEITURA DO SENSOR DE UMIDADE
  // ----------------------------------------------

  // Realiza a leitura analogica do sensor.
  int sensorValue = analogRead(SENSOR_PIN);

  // Limita o valor recebido para evitar que
  // ultrapasse os limites definidos.
  int constrainedSensorValue = constrain(
    sensorValue,
    SENSOR_MIN_VALUE,
    SENSOR_MAX_VALUE
  );

  // Converte o valor analogico para porcentagem.
  // O resultado representa o nivel de umidade.
  int moisturePercent = map(
    constrainedSensorValue,
    SENSOR_MIN_VALUE,
    SENSOR_MAX_VALUE,
    0,
    100
  );

  // Garante que o resultado fique entre 0 e 100%.
  moisturePercent = constrain(moisturePercent, 0, 100);

  // Captura o tempo atual de funcionamento.
  unsigned long currentMillis = millis();


  // ----------------------------------------------
  // 2. SENSOR ULTRASSONICO HC-SR04
  // ----------------------------------------------

  // Envia um pulso inicial para preparar o sensor.
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Envia um pulso de 10 microssegundos.
  // Esse sinal inicia a medicao da distancia.
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  // Finaliza o envio do sinal.
  digitalWrite(TRIG_PIN, LOW);

  // Mede quanto tempo o sinal demora para retornar.
  long duracao = pulseIn(ECHO_PIN, HIGH, 30000);

  // Variavel utilizada para armazenar a distancia.
  int distancia = 0;

  // Converte o tempo de retorno em centimetros.
  if (duracao > 0) {
    distancia = duracao * 0.034 / 2;
  }


  // ----------------------------------------------
  // 3. SISTEMA DE ALERTA SONORO - BUZZER
  // ----------------------------------------------

  // O buzzer sera desligado quando nao houver
  // uma leitura valida ou quando o objeto estiver
  // a 30 centimetros ou menos.
  if (duracao == 0 || distancia <= DISTANCIA_LIMITE) {

    noTone(BUZZER_PIN);
    digitalWrite(BUZZER_PIN, LOW);

  }

  else {

    // Abaixo de 10%: alerta sonoro continuo.
    if (moisturePercent < 10) {

      tone(BUZZER_PIN, 1000);

    }

    // Abaixo de 30%: alerta sonoro intermitente.
    else if (moisturePercent < 30) {

      tone(BUZZER_PIN, 1000);
      delay(400);

      noTone(BUZZER_PIN);
      delay(2000);

    }

    // Acima de 30%: buzzer desligado.
    else {

      noTone(BUZZER_PIN);
      digitalWrite(BUZZER_PIN, LOW);

    }
  }


  // ----------------------------------------------
  // 4. LED PISCANTE
  // ----------------------------------------------

  // Quando a umidade estiver abaixo de 20%,
  // o LED piscara rapidamente para indicar alerta.
  if (moisturePercent < 20) {

    if (currentMillis - lastBlinkTime >= BLINK_FAST) {

      lastBlinkTime = currentMillis;

      // Inverte o estado atual do LED.
      ledState = !ledState;

      digitalWrite(LED_PIN, ledState);
    }

  }

  // Entre 20% e 74%, o LED pisca lentamente.
  else if (moisturePercent < 75) {

    if (currentMillis - lastBlinkTime >= BLINK_SLOW) {

      lastBlinkTime = currentMillis;
      ledState = !ledState;

      digitalWrite(LED_PIN, ledState);
    }

  }

  // A partir de 75%, o LED permanece desligado.
  else {

    digitalWrite(LED_PIN, LOW);
    ledState = false;

  }


  // ----------------------------------------------
  // 5. LED RGB - INDICADOR DE UMIDADE
  // ----------------------------------------------

  // Atualiza as cores a cada 100 milissegundos.
  if (currentMillis - lastRgbUpdate >= RGB_UPDATE_INTERVAL) {

    lastRgbUpdate = currentMillis;

    // Valores iniciais das tres cores.
    int red = 0;
    int green = 0;
    int blue = 0;

    // A partir de 50%:
    // A cor varia entre verde e amarelo.
    if (moisturePercent >= 50) {

      red = map(moisturePercent, 50, 100, 255, 0);
      green = 255;
      blue = 0;

    }

    // Entre 25% e 49%:
    // A cor varia entre vermelho e amarelo.
    else if (moisturePercent >= 25) {

      red = 255;
      green = map(moisturePercent, 25, 50, 0, 255);
      blue = 0;

    }

    // Abaixo de 25%:
    // O LED permanece vermelho, indicando baixa umidade.
    else {

      red = 255;
      green = 0;
      blue = 0;

    }

    // Envia os valores das cores para o LED RGB.
    analogWrite(RED_PIN, red);
    analogWrite(GREEN_PIN, green);
    analogWrite(BLUE_PIN, blue);
  }


  // ----------------------------------------------
  // 6. DISPLAY LCD
  // ----------------------------------------------

  // Primeira linha: mostra a porcentagem de umidade.
  lcd.setCursor(0, 0);
  lcd.print("Umidade: ");
  lcd.print(moisturePercent);
  lcd.print("%   ");

  // Segunda linha: mostra uma mensagem conforme
  // a quantidade de umidade identificada no solo.
  lcd.setCursor(0, 1);

  if (moisturePercent <= 10) {
    lcd.print("SECA CRITICA   ");
  }

  else if (moisturePercent <= 25) {
    lcd.print("PRECISO DE AGUA");
  }

  else if (moisturePercent <= 45) {
    lcd.print("ME REGUE, MAIS ");
  }

  else if (moisturePercent <= 65) {
    lcd.print("UMIDADE BOA    ");
  }

  else {
    lcd.print("UMIDADE IDEAL  ");
  }

}
