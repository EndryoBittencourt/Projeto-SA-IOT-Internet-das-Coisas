// 1. Inclusão das Bibliotecas
#include <Wire.h> 
#include <LiquidCrystal_I2C.h> 
#include <DHT.h> 

// 2. Definição dos Pinos e Constantes

// Sensor DHT11
#define PINO_DHT 2 
#define TIPO_DHT DHT11 

// Pinos dos LEDs e Buzzer (Pinos digitais)
const int ledVerde = 9;
const int ledAmarelo = 10;
const int ledVermelho = 11;
const int buzzerPin = 8;

// Sensor de Chuva
const int pinoChuva = A0;

// Limiares para o sensor de chuva (Ajuste esses valores!)
const int LIMIAR_CHUVINHA = 200; // Valor abaixo disso = "Chovendo um pouco"
const int LIMIAR_TEMPESTADE = 325; // Valor abaixo disso = "Chovendo muito"

// 3. Criação dos Objetos
LiquidCrystal_I2C lcd(0x27, 16, 2); 
DHT dht(PINO_DHT, TIPO_DHT);

// 4. Configuração Inicial (setup)

void setup() {
  // Inicialização do Serial Monitor a 9600 baud rate (Taxa de transmissão)
  Serial.begin(9600);
  Serial.println("--- Estacao Meteorologica Inicializada ---");
  Serial.println("----------------------------------------");

  // Configuração dos pinos digitais como SAÍDA
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  // Inicializa o LCD
  lcd.init();
  lcd.backlight(); 
  lcd.clear(); 
  lcd.setCursor(0, 0);
  lcd.print("Estacao Meteoro");
  lcd.setCursor(0, 1);
  lcd.print("Inicializando...");

  // Inicializa o sensor DHT
  dht.begin();

  // Garante que todos os LEDs e o Buzzer estejam desligados no início
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledAmarelo, LOW);
  digitalWrite(ledVermelho, LOW);
  digitalWrite(buzzerPin, LOW);
  
  delay(2000); 
}

// 5. Loop Principal (loop)

void loop() {
  // O sensor DHT11 só deve ser lido a cada 2 segundos no mínimo
  delay(2000); 

  // --- LEITURA DOS SENSORES ---
  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();
  int valorChuva = analogRead(pinoChuva);

  // Variável para armazenar o status da chuva para o Serial Monitor
  String statusChuva = "";

  // Verifica falha na leitura do DHT
  if (isnan(umidade) || isnan(temperatura)) {
    Serial.println(">>> ERRO: Falha na leitura do sensor DHT! <<<");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erro DHT!");
    return; 
  }

  // --- EXIBIÇÃO NO LCD ---
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperatura, 1); 
  lcd.print((char)223); 
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Umid: ");
  lcd.print(umidade, 1); 
  lcd.print("%");


  // --- LÓGICA DE ALERTA DE CHUVA ---
  
  // Desliga todos os LEDs e o Buzzer para resetar
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledAmarelo, LOW);
  digitalWrite(ledVermelho, LOW);
  digitalWrite(buzzerPin, LOW);
  
  if (valorChuva < LIMIAR_CHUVINHA) {
    // 1. NÃO ESTÁ CHOVENDO 
    digitalWrite(ledVerde, HIGH);
    lcd.setCursor(10, 1);
    lcd.print(" SECO");
    statusChuva = "SECO (Led Verde)";

  } else if (valorChuva < LIMIAR_TEMPESTADE) {
    // 2. CHOVENDO POUCO 
    digitalWrite(ledAmarelo, HIGH);
    lcd.setCursor(10, 1);
    lcd.print(" CHUVA");
    statusChuva = "CHUVA LEVE (Led Amarelo)";

  } else {
    // 3. CHOVENDO MUITO - ALARME DE TEMPESTADE
    digitalWrite(ledVermelho, HIGH);
    tone(buzzerPin, 1000, 200); 

    lcd.setCursor(10, 1);
    lcd.print(" ALERTA!");
    lcd.setCursor(10, 0);
    lcd.print(" RISCO!");
    statusChuva = "TEMP.: RISCO! (Led Vermelho + Buzzer)";

    delay(250); 
    noTone(buzzerPin); 
  }
  
  // --- EXIBIÇÃO NO MONITOR SERIAL ---
  Serial.println("----------------------------------------");
  Serial.print("Leitura Sensor Chuva (0-1023): ");
  Serial.println(valorChuva);
  Serial.print("Status do Alerta: ");
  Serial.println(statusChuva);
  Serial.print("Temperatura:");
  Serial.print(temperatura, 1);
  Serial.println(" C");
  Serial.print("Umidade: ");
  Serial.print( umidade, 1);
  Serial.println("%");
  Serial.println("----------------------------------------");
}