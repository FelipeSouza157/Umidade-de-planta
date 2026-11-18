
#include <LiquidCrystal_I2C.h>

// ================================================
// MONITOR DE PLANTA IoT
// VERSAO 1.2.0 - PRECISAO DOS SENSORES
// ================================================

LiquidCrystal_I2C lcd(0x27, 16, 2);


// ================================================
// CONFIGURACAO DO SENSOR
// ================================================

const int SENSOR_PIN = A0;

// Valores de calibracao ilustrativos.
// SENSOR_SECO representa o valor medido no solo seco.
// SENSOR_UMIDO representa o valor medido no solo umido.
const int SENSOR_SECO = 500;
const int SENSOR_UMIDO = 200;


// ================================================
// CONFIGURACAO DOS COMPONENTES
// ================================================

const int RED_PIN = 11;
const int GREEN_PIN = 9;
const int BLUE_PIN = 10;

const int LED_PIN = 6;
const int BUZZER_PIN = 5;

const int TRIG_PIN = 8;
const int ECHO_PIN = 7;

const int DISTANCIA_LIMITE = 30;


// ================================================
// TEMPORIZADORES
// ================================================

unsigned long lastBlinkTime = 0;
unsigned long lastRgbUpdate = 0;
unsigned long lastBuzzerTime = 0;

const unsigned long BLINK_SLOW = 1000;
const unsigned long BLINK_FAST = 200;
const unsigned long RGB_UPDATE_INTERVAL = 100;

const unsigned long BUZZER_ON_TIME = 400;
const unsigned long BUZZER_OFF_TIME = 2000;

bool ledState = false;
bool buzzerState = false;


// ================================================
// SETUP
// ================================================

void setup() {

  // Configura os componentes como saidas.
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Configura o sensor ultrassonico.
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Inicializa o display.
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Monitor Planta");

  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");

  delay(2000);
  lcd.clear();
}


// ================================================
// LEITURA COM MEDIA
// ================================================

// Realiza dez leituras do sensor.
// A soma das leituras e dividida por dez.
//
// Isso ajuda a reduzir variacoes momentaneas
// causadas por pequenas oscilacoes do sensor.

int lerUmidade() {

  long soma = 0;

  for (int i = 0; i < 10; i++) {

    soma += analogRead(SENSOR_PIN);

    // Pequeno intervalo entre as leituras.
    delay(5);
  }

  return soma / 10;
}


// ================================================
// CALIBRACAO DA UMIDADE
// ================================================

// Recebe a media das leituras e converte
// o resultado para uma porcentagem.
//
// 0% representa o solo seco.
// 100% representa o solo umido.

int calcularUmidade(int leitura) {

  int umidade = map(
    leitura,
    SENSOR_SECO,
    SENSOR_UMIDO,
    0,
    100
  );

  // Evita valores abaixo de 0% ou acima de 100%.
  return constrain(umidade, 0, 100);
}


// ================================================
// CONTROLE DO BUZZER
// ================================================

void atualizarBuzzer(int umidade, long duracao, int distancia) {

  unsigned long agora = millis();

  // Desliga quando a leitura do ultrassonico
  // nao for valida ou detectar objeto proximo.
  if (duracao == 0 || distancia <= DISTANCIA_LIMITE) {

    noTone(BUZZER_PIN);

    buzzerState = false;
    lastBuzzerTime = agora;

    return;
  }

  // Umidade critica: som continuo.
  if (umidade < 10) {

    tone(BUZZER_PIN, 1000);
    buzzerState = true;

  }

  // Umidade baixa: som intermitente.
  else if (umidade < 30) {

    unsigned long intervalo = buzzerState
      ? BUZZER_ON_TIME
      : BUZZER_OFF_TIME;

    if (agora - lastBuzzerTime >= intervalo) {

      lastBuzzerTime = agora;
      buzzerState = !buzzerState;

      if (buzzerState) {
        tone(BUZZER_PIN, 1000);
      } else {
        noTone(BUZZER_PIN);
      }
    }

  }

  // Umidade adequada: buzzer desligado.
  else {

    noTone(BUZZER_PIN);

    buzzerState = false;
    lastBuzzerTime = agora;

  }
}


// ================================================
// LOOP PRINCIPAL
// ================================================

void loop() {

  // 1. REALIZA A MEDIA DAS LEITURAS.
  int sensorValue = lerUmidade();

  // 2. CONVERTE A MEDIA PARA PORCENTAGEM.
  int moisturePercent = calcularUmidade(sensorValue);

  unsigned long currentMillis = millis();


  // ----------------------------------------------
  // SENSOR ULTRASSONICO
  // ----------------------------------------------

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duracao = pulseIn(ECHO_PIN, HIGH, 30000);

  int distancia = 0;

  if (duracao > 0) {
    distancia = duracao * 0.034 / 2;
  }


  // ----------------------------------------------
  // BUZZER
  // ----------------------------------------------

  atualizarBuzzer(moisturePercent, duracao, distancia);


  // ----------------------------------------------
  // LED PISCANTE
  // ----------------------------------------------

  if (moisturePercent < 20) {

    if (currentMillis - lastBlinkTime >= BLINK_FAST) {

      lastBlinkTime = currentMillis;
      ledState = !ledState;

      digitalWrite(LED_PIN, ledState);
    }

  }

  else if (moisturePercent < 75) {

    if (currentMillis - lastBlinkTime >= BLINK_SLOW) {

      lastBlinkTime = currentMillis;
      ledState = !ledState;

      digitalWrite(LED_PIN, ledState);
    }

  }

  else {

    digitalWrite(LED_PIN, LOW);
    ledState = false;

  }


  // ----------------------------------------------
  // LED RGB
  // ----------------------------------------------

  if (currentMillis - lastRgbUpdate >= RGB_UPDATE_INTERVAL) {

    lastRgbUpdate = currentMillis;

    int red = 0;
    int green = 0;
    int blue = 0;

    if (moisturePercent >= 50) {

      red = map(moisturePercent, 50, 100, 255, 0);
      green = 255;

    }

    else if (moisturePercent >= 25) {

      red = 255;
      green = map(moisturePercent, 25, 50, 0, 255);

    }

    else {

      red = 255;

    }

    analogWrite(RED_PIN, red);
    analogWrite(GREEN_PIN, green);
    analogWrite(BLUE_PIN, blue);
  }


  // ----------------------------------------------
  // LCD
  // ----------------------------------------------

  lcd.setCursor(0, 0);
  lcd.print("Umidade: ");
  lcd.print(moisturePercent);
  lcd.print("%   ");

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
