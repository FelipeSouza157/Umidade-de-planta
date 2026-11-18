
#include <LiquidCrystal_I2C.h>

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Sensor de umidade
const int SENSOR_PIN = A0;
const int SENSOR_MIN_VALUE = 0;
const int SENSOR_MAX_VALUE = 500;

// LED RGB
const int RED_PIN = 11;
const int GREEN_PIN = 9;
const int BLUE_PIN = 10;

// LED piscante
const int LED_PIN = 6;

// Buzzer
const int BUZZER_PIN = 5;

// Sensor ultrassonico HC-SR04
const int TRIG_PIN = 8;
const int ECHO_PIN = 7;
const int DISTANCIA_LIMITE = 30;

// Temporizadores
unsigned long lastBlinkTime = 0;
unsigned long lastRgbUpdate = 0;

const int BLINK_SLOW = 1000;
const int BLINK_FAST = 200;
const int RGB_UPDATE_INTERVAL = 100;

bool ledState = false;

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Monitor Planta");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  delay(2000);
  lcd.clear();
}

void loop() {

  // Leitura do sensor de umidade
  int sensorValue = analogRead(SENSOR_PIN);

  int constrainedSensorValue = constrain(
    sensorValue,
    SENSOR_MIN_VALUE,
    SENSOR_MAX_VALUE
  );

  int moisturePercent = map(
    constrainedSensorValue,
    SENSOR_MIN_VALUE,
    SENSOR_MAX_VALUE,
    0,
    100
  );

  moisturePercent = constrain(moisturePercent, 0, 100);

  unsigned long currentMillis = millis();

  // --------------------------------
  // SENSOR ULTRASSONICO
  // --------------------------------

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

  // --------------------------------
// BUZZER
// --------------------------------

// Se nao detectar a bolinha OU ela estiver a ate 1 metro
if (duracao == 0 || distancia <= DISTANCIA_LIMITE) {

  noTone(BUZZER_PIN);
  digitalWrite(BUZZER_PIN, LOW);

}
else {

  // Umidade critica: som continuo
  if (moisturePercent < 10) {

    tone(BUZZER_PIN, 1000);

  }

  // Umidade baixa: som intermitente
  else if (moisturePercent < 30) {

    tone(BUZZER_PIN, 1000);
    delay(400);

    noTone(BUZZER_PIN);
    delay(2000);

  }

  // Umidade suficiente: buzzer desligado
  else {

    noTone(BUZZER_PIN);
    digitalWrite(BUZZER_PIN, LOW);

  }
}

  // --------------------------------
  // LED PISCANTE
  // --------------------------------

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

  // --------------------------------
  // LED RGB
  // --------------------------------

  if (currentMillis - lastRgbUpdate >= RGB_UPDATE_INTERVAL) {

    lastRgbUpdate = currentMillis;

    int red = 0;
    int green = 0;
    int blue = 0;

    if (moisturePercent >= 50) {

      // Verde em 100% e amarelo em 50%
      red = map(moisturePercent, 50, 100, 255, 0);
      green = 255;
      blue = 0;

    }
    else if (moisturePercent >= 25) {

      // Amarelo em 50% e vermelho em 25%
      red = 255;
      green = map(moisturePercent, 25, 50, 0, 255);
      blue = 0;

    }
    else {

      // Vermelho abaixo de 25%
      red = 255;
      green = 0;
      blue = 0;

    }

    analogWrite(RED_PIN, red);
    analogWrite(GREEN_PIN, green);
    analogWrite(BLUE_PIN, blue);
  }

  // --------------------------------
  // LCD
  // --------------------------------

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
