
#include <LiquidCrystal_I2C.h>

// ================================================
// MONITOR DE PLANTA IoT
// VERSAO 1.1.0 - OTIMIZACAO
// ================================================

LiquidCrystal_I2C lcd(0x27, 16, 2);

// SENSOR DE UMIDADE
const int SENSOR_PIN = A0;
const int SENSOR_MIN_VALUE = 0;
const int SENSOR_MAX_VALUE = 500;

// LED RGB
const int RED_PIN = 11;
const int GREEN_PIN = 9;
const int BLUE_PIN = 10;

// LED PISCANTE
const int LED_PIN = 6;

// BUZZER
const int BUZZER_PIN = 5;

// SENSOR ULTRASSONICO
const int TRIG_PIN = 8;
const int ECHO_PIN = 7;
const int DISTANCIA_LIMITE = 30;


// ================================================
// CONTROLE DE TEMPO
// ================================================

// Armazena o ultimo momento de atualizacao.
unsigned long lastBlinkTime = 0;
unsigned long lastRgbUpdate = 0;
unsigned long lastBuzzerTime = 0;

// Intervalos dos LEDs.
const unsigned long BLINK_SLOW = 1000;
const unsigned long BLINK_FAST = 200;
const unsigned long RGB_UPDATE_INTERVAL = 100;

// Intervalos do buzzer.
// Ele permanece ligado por 400 ms e desligado
// por 2000 ms quando a umidade estiver baixa.
const unsigned long BUZZER_ON_TIME = 400;
const unsigned long BUZZER_OFF_TIME = 2000;

// Estados dos componentes.
bool ledState = false;
bool buzzerState = false;


// ================================================
// SETUP
// ================================================

void setup() {

  // Configuracao dos pinos.
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Inicializacao do LCD.
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
// FUNCAO DE CONTROLE DO BUZZER
// ================================================

// Esta funcao controla os alertas sonoros
// sem utilizar pausas longas no programa.
//
// Parametros:
// umidade = porcentagem de umidade do solo
// duracao = tempo de retorno do ultrassonico
// distancia = distancia calculada em centimetros

void atualizarBuzzer(int umidade, long duracao, int distancia) {

  unsigned long agora = millis();

  // Verifica se existe uma leitura valida
  // e se o objeto esta fora da distancia limite.
  if (duracao == 0 || distancia <= DISTANCIA_LIMITE) {

    noTone(BUZZER_PIN);

    buzzerState = false;
    lastBuzzerTime = agora;

    return;
  }

  // Umidade critica: buzzer continuo.
  if (umidade < 10) {

    tone(BUZZER_PIN, 1000);
    buzzerState = true;

  }

  // Umidade baixa: buzzer intermitente.
  else if (umidade < 30) {

    // Escolhe o intervalo conforme o estado atual.
    unsigned long intervalo = buzzerState
      ? BUZZER_ON_TIME
      : BUZZER_OFF_TIME;

    // Verifica se o tempo necessario ja passou.
    if (agora - lastBuzzerTime >= intervalo) {

      lastBuzzerTime = agora;

      // Alterna entre ligado e desligado.
      buzzerState = !buzzerState;

      if (buzzerState) {
        tone(BUZZER_PIN, 1000);
      } else {
        noTone(BUZZER_PIN);
      }
    }

  }

  // Umidade suficiente: desliga o buzzer.
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

  // 1. LEITURA DA UMIDADE
  int sensorValue = analogRead(SENSOR_PIN);

  int constrainedSensorValue = constrain(
    sensorValue,
    SENSOR_MIN_VALUE,
    SENSOR_MAX_VALUE
  );

  // Conversao da leitura para porcentagem.
  int moisturePercent = map(
    constrainedSensorValue,
    SENSOR_MIN_VALUE,
    SENSOR_MAX_VALUE,
    0,
    100
  );

  moisturePercent = constrain(moisturePercent, 0, 100);

  unsigned long currentMillis = millis();


  // 2. LEITURA DO ULTRASSONICO
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


  // 3. ATUALIZACAO DO BUZZER
  // Agora a funcao trabalha com millis(),
  // evitando os delays de 400 e 2000 ms.
  atualizarBuzzer(moisturePercent, duracao, distancia);


  // 4. LED PISCANTE
  if (moisturePercent < 20) {

    // Pisca rapidamente quando a umidade e critica.
    if (currentMillis - lastBlinkTime >= BLINK_FAST) {

      lastBlinkTime = currentMillis;
      ledState = !ledState;

      digitalWrite(LED_PIN, ledState);
    }

  }

  else if (moisturePercent < 75) {

    // Pisca lentamente quando a umidade e intermediaria.
    if (currentMillis - lastBlinkTime >= BLINK_SLOW) {

      lastBlinkTime = currentMillis;
      ledState = !ledState;

      digitalWrite(LED_PIN, ledState);
    }

  }

  else {

    // Desliga quando a umidade e suficiente.
    digitalWrite(LED_PIN, LOW);
    ledState = false;

  }


  // 5. LED RGB
  if (currentMillis - lastRgbUpdate >= RGB_UPDATE_INTERVAL) {

    lastRgbUpdate = currentMillis;

    int red = 0;
    int green = 0;
    int blue = 0;

    // Verde e amarelo.
    if (moisturePercent >= 50) {

      red = map(moisturePercent, 50, 100, 255, 0);
      green = 255;

    }

    // Amarelo e vermelho.
    else if (moisturePercent >= 25) {

      red = 255;
      green = map(moisturePercent, 25, 50, 0, 255);

    }

    // Vermelho.
    else {

      red = 255;

    }

    analogWrite(RED_PIN, red);
    analogWrite(GREEN_PIN, green);
    analogWrite(BLUE_PIN, blue);
  }


  // 6. LCD
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
