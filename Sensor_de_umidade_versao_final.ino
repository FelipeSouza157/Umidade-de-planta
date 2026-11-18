
#include <LiquidCrystal_I2C.h>

// ==========================================
// ADIÇÃO 1: CONFIGURAÇÃO DO DISPLAY LCD(ic2)
// ==========================================
// Foi adicionado um display LCD I2C para mostrar
// as informações de umidade e o estado da planta.
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ==========================================
// ADIÇÃO 2: SENSOR DE UMIDADE
// ==========================================
// O sensor realiza a leitura da umidade do solo.
// Os valores são convertidos para uma porcentagem
// de 0 a 100% para facilitar a visualização.
const int SENSOR_PIN = A0;
const int SENSOR_MIN_VALUE = 0;
const int SENSOR_MAX_VALUE = 600;

// ==========================================
// ADIÇÃO 3: LED RGB
// ==========================================
// Foi adicionado um LED RGB para representar
// visualmente o nível de umidade da planta:
// Verde = umidade alta
// Amarelo = umidade intermediária
// Vermelho = umidade baixa
const int RED_PIN = 11;
const int GREEN_PIN = 9;
const int BLUE_PIN = 10;

// ==========================================
// ADIÇÃO 4: LED INDICADOR PISCANTE
// ==========================================
// Um LED adicional foi utilizado para indicar
// situações de atenção, alterando sua velocidade
// de piscagem conforme o nível de umidade.
const int LED_PIN = 6;

// ==========================================
// ADIÇÃO 5: BUZZER DE ALERTA
// ==========================================
// Foi implementado um buzzer para emitir alertas
// sonoros quando a umidade estiver baixa ou crítica.
const int BUZZER_PIN = 5;

// ==========================================
// ADIÇÃO 6: SENSOR ULTRASSÔNICO HC-SR04
// ==========================================
// O sensor ultrassônico foi adicionado para detectar
// a presença de um objeto próximo ao sistema.
// Ele também controla quando os alertas sonoros
// podem ser acionados.
const int TRIG_PIN = 8;
const int ECHO_PIN = 7;
const int DISTANCIA_LIMITE = 30;

// ==========================================
// CONTROLE DE TEMPO
// ==========================================
// Foram utilizados temporizadores com millis()
// para controlar a piscagem dos LEDs e a atualização
// das cores do LED RGB.
unsigned long lastBlinkTime = 0;
unsigned long lastRgbUpdate = 0;

const int BLINK_SLOW = 1000;
const int BLINK_FAST = 200;
const int RGB_UPDATE_INTERVAL = 100;

bool ledState = false;

// ==========================================
// CONFIGURAÇÃO INICIAL
// ==========================================
void setup() {

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Inicialização do display LCD.
  lcd.init();
  lcd.backlight();

  // Mensagem de inicialização do projeto.
  lcd.setCursor(0, 0);
  lcd.print("Monitor Planta");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  delay(2000);
  lcd.clear();
}

// ==========================================
// EXECUÇÃO PRINCIPAL DO PROJETO
// ==========================================
void loop() {

  // ----------------------------------------
  // LEITURA E CONVERSÃO DA UMIDADE
  // ----------------------------------------
  // O valor analógico do sensor é lido e limitado
  // entre os valores mínimo e máximo calibrados.
  int sensorValue = analogRead(SENSOR_PIN);

  int constrainedSensorValue = constrain(
    sensorValue,
    SENSOR_MIN_VALUE,
    SENSOR_MAX_VALUE
  );

  // Conversão da leitura para porcentagem.
  int moisturePercent = map(
    constrainedSensorValue,
    SENSOR_MIN_VALUE,
    SENSOR_MAX_VALUE,
    0,
    100
  );

  moisturePercent = constrain(moisturePercent, 0, 100);

  unsigned long currentMillis = millis();

  // ----------------------------------------
  // ADIÇÃO 7: DETECÇÃO COM SENSOR ULTRASSÔNICO
  // ----------------------------------------
  // O HC-SR04 envia uma onda sonora e calcula
  // a distância pelo tempo que ela demora
  // para retornar ao sensor.
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

  // ----------------------------------------
  // ADIÇÃO 8: SISTEMA DE ALERTA SONORO
  // ----------------------------------------
  // O buzzer verifica a distância detectada
  // antes de emitir qualquer alerta.
  // Quando não existe detecção ou o objeto
  // está dentro do limite, o buzzer permanece
  // desligado.
  if (duracao == 0 || distancia <= DISTANCIA_LIMITE) {

    noTone(BUZZER_PIN);
    digitalWrite(BUZZER_PIN, LOW);

  }
  else {

    // Umidade crítica: som contínuo.
    if (moisturePercent < 10) {

      tone(BUZZER_PIN, 1000);

    }

    // Umidade baixa: som intermitente.
    else if (moisturePercent < 30) {

      tone(BUZZER_PIN, 1000);
      delay(400);

      noTone(BUZZER_PIN);
      delay(2000);

    }

    // Umidade suficiente: buzzer desligado.
    else {

      noTone(BUZZER_PIN);
      digitalWrite(BUZZER_PIN, LOW);

    }
  }

  // ----------------------------------------
  // ADIÇÃO 9: LED INDICADOR DE ATENÇÃO
  // ----------------------------------------
  // A velocidade de piscagem muda conforme
  // a umidade identificada pelo sensor.
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

  // ----------------------------------------
  // ADIÇÃO 10: INDICAÇÃO VISUAL COM LED RGB
  // ----------------------------------------
  // O LED RGB altera suas cores de acordo
  // com a porcentagem de umidade:
  // Verde: umidade elevada.
  // Amarelo: umidade intermediária.
  // Vermelho: umidade baixa.
  if (currentMillis - lastRgbUpdate >= RGB_UPDATE_INTERVAL) {

    lastRgbUpdate = currentMillis;

    int red = 0;
    int green = 0;
    int blue = 0;

    if (moisturePercent >= 50) {

      // Transição entre amarelo e verde.
      red = map(moisturePercent, 50, 100, 255, 0);
      green = 255;
      blue = 0;

    }
    else if (moisturePercent >= 25) {

      // Transição entre vermelho e amarelo.
      red = 255;
      green = map(moisturePercent, 25, 50, 0, 255);
      blue = 0;

    }
    else {

      // Umidade abaixo de 25%: vermelho.
      red = 255;
      green = 0;
      blue = 0;

    }

    analogWrite(RED_PIN, red);
    analogWrite(GREEN_PIN, green);
    analogWrite(BLUE_PIN, blue);
  }

  // ----------------------------------------
  // ADIÇÃO 11: INFORMAÇÕES NO DISPLAY LCD
  // ----------------------------------------
  // O display mostra a porcentagem de umidade
  // e uma mensagem correspondente à situação
  // atual da planta.
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
