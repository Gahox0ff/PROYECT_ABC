#include <DHT.h>

// Pines y constantes
#define DHTPIN A15
#define DHTTYPE DHT11
#define LDR_PIN A14
#define LED1 22
#define LED2 23
#define LED3 24
#define LED4 25
#define PulsadorPin 2 // Se usa la interrupción para contar pulsos

// Inicialización de objetos
DHT dht(DHTPIN, DHTTYPE);

// Variables globales
volatile unsigned long cuenta = 0;
volatile bool flancoDetectado = false;
unsigned long ultimaTransicion = 0;
bool estadoAnteriorPulsador = LOW;

// Luz
float Vcc = 5.0;
float Rc = 10000;
float Rmin = 470;
float Rmax = 10000;

// Tiempos
unsigned long tiempoAnteriorEnvio = 0;
const unsigned long intervaloEnvio = 1000; // 1 segundo

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(PulsadorPin, INPUT_PULLUP);

  Serial1.begin(9600);
  attachInterrupt(digitalPinToInterrupt(PulsadorPin), contarPulso, CHANGE);

  dht.begin();
}

void contarPulso() {
  bool estadoActual = digitalRead(PulsadorPin);

  // Antirebote + flanco completo (bajo-alto-bajo)
  if (estadoAnteriorPulsador == LOW && estadoActual == HIGH) {
    unsigned long tiempoActual = millis();
    if (tiempoActual - ultimaTransicion > 50) { // Debounce 50ms
      flancoDetectado = true;
      ultimaTransicion = tiempoActual;
    }
  }
  estadoAnteriorPulsador = estadoActual;
}

void loop() {
  if (flancoDetectado) {
    cuenta++;
    flancoDetectado = false;
  }

  unsigned long tiempoActual = millis();
  if (tiempoActual - tiempoAnteriorEnvio >= intervaloEnvio) {
    tiempoAnteriorEnvio = tiempoActual;

    // LUZ
    float ldrValor = analogRead(LDR_PIN);
    float Vin = (ldrValor / 1023.0) * Vcc;
    float Rldr = (Vin * Rc) / (Vcc - Vin);
    float luz = (Rmax - Rldr) / (Rmax - Rmin) * 100.0;
    luz = constrain(luz, 0.0, 100.0);

    // TEMPERATURA Y HUMEDAD
    float temperatura = dht.readTemperature();
    float humedad = dht.readHumidity();

    // ENVÍO SERIAL
    Serial1.print("Frecuencia: ");
Serial1.print(cuenta);
Serial1.println(" Hz");

Serial1.print("Temperatura: ");
Serial1.print(temperatura);
Serial1.println(" C");

Serial1.print("Humedad: ");
Serial1.print(humedad);
Serial1.println(" %");

Serial1.print("Luz: ");
Serial1.print(luz);
Serial1.println(" %");

Serial1.print("Conteo: ");
Serial1.println(cuenta);
  }

  // Control por comandos desde Serial1 (opcional)
  if (Serial1.available()) {
    String comando = Serial1.readStringUntil('\n');
    comando.trim();

    if (comando == "LED1") digitalWrite(LED1, HIGH);
    else if (comando == "LED1OFF") digitalWrite(LED1, LOW);
    else if (comando == "LED2") digitalWrite(LED2, HIGH);
    else if (comando == "LED2OFF") digitalWrite(LED2, LOW);
    else if (comando == "LED3") digitalWrite(LED3, HIGH);
    else if (comando == "LED3OFF") digitalWrite(LED3, LOW);
     else if (comando == "LED4") digitalWrite(LED4, HIGH);
    else if (comando == "LED4OFF") digitalWrite(LED4, LOW);
  }
}
