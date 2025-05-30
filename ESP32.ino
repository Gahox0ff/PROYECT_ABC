#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BluetoothSerial.h>

// --- Pines ---
#define DHTPIN 32
#define DHTTYPE DHT11
#define LDR_PIN 33
#define LED1 26
#define LED2 23
#define LED3 18
#define LED4 19

#define PULSADOR_PIN 2
#define FRECUENCIA_PIN 4

#define OLED_SDA 21
#define OLED_SCL 22

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define ROJO_PIN 13
#define VERDE_PIN 14
#define AZUL_PIN 27

// Canales PWM
#define PWM_CH_ROJO 0
#define PWM_CH_VERDE 1
#define PWM_CH_AZUL 2
#define PWM_FREQ 5000
#define PWM_RES 8 // 8 bits = valores de 0 a 255

// --- Objetos ---
DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
BluetoothSerial SerialBT;

// --- Variables globales ---
volatile unsigned long cuentaFrecuencia = 0;
volatile unsigned long pulsosBoton = 0;
volatile bool esperandoBajada = false;

// Variables para el logo
bool mostrarLogoActiva = false;
unsigned long tiempoInicioLogo = 0;
const unsigned long DURACION_CADA_LOGO = 3000; // 3 segundos para cada imagen
const unsigned long DURACION_TOTAL_LOGOS = 6000; // 6 segundos total (2 imágenes)
int imagenActual = 1; // 1 = primera imagen, 2 = segunda imagen

float Vcc = 4.6;
float Rc = 10000;
float Rmin = 470;
float Rmax = 10000;

unsigned long tiempoAnterior = 0;
const unsigned long intervalo = 1000; // 1 segundo

// LEDs
bool estadoLED1 = false;
bool estadoLED2 = false;
bool estadoLED3 = false;
bool estadoLED4 = false;

// Variables para almacenar los últimos datos
float ultimaTemperatura = 0;
float ultimaHumedad = 0;
float ultimaLuz = 0;
unsigned long ultimaFrecuencia = 0;
unsigned long ultimosPulsos = 0;

// Definición de los logos en formato XBM
#define logo_width 128
#define logo_height 64
const unsigned char logo_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf8, 0x10, 0x00, 0x3f, 0xff, 0xff, 0xfe, 0x00, 0x10, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf8, 0x10, 0x00, 0x3f, 0xff, 0xff, 0xfe, 0x00, 0x10, 
	0x00, 0x00, 0x00, 0x03, 0xff, 0xcf, 0xff, 0xf8, 0x10, 0x00, 0x3f, 0xff, 0xff, 0xfe, 0x00, 0x10, 
	0x00, 0x00, 0x00, 0x03, 0xff, 0xcf, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x10, 
	0x00, 0x00, 0x00, 0x03, 0xff, 0xcf, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x10, 
	0x00, 0x00, 0x00, 0x03, 0xff, 0xcf, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x10, 
	0x00, 0x00, 0x00, 0x03, 0xff, 0xcf, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x10, 
	0x00, 0x00, 0x00, 0x03, 0xff, 0xcf, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x10, 
	0x00, 0x00, 0x00, 0x03, 0xff, 0xcf, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x10, 
	0x00, 0x00, 0x00, 0x03, 0xff, 0xcf, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x10, 
	0x00, 0x00, 0x00, 0x03, 0xff, 0xcf, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x10, 
	0x00, 0x00, 0xff, 0xe3, 0xff, 0xcf, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x10, 
	0x00, 0x00, 0xff, 0xe3, 0xff, 0xcf, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x10, 
	0x00, 0x00, 0xff, 0xe3, 0xff, 0xcf, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x10, 
	0x03, 0xf8, 0xff, 0xe3, 0xff, 0xcf, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x10, 
	0x03, 0xf8, 0xff, 0xe3, 0xff, 0xcf, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x10, 
	0x03, 0xf8, 0xff, 0xe3, 0xff, 0xcf, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x10, 
	0x03, 0xf8, 0xff, 0xe3, 0xff, 0xcf, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 
	0x03, 0xf8, 0xff, 0xe3, 0xff, 0xcf, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 
	0x03, 0xf8, 0xff, 0xe3, 0xff, 0xcf, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 
	0x03, 0xf8, 0xff, 0xe3, 0xff, 0xcf, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 
	0x03, 0xf8, 0xff, 0xe3, 0xff, 0xcf, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 
	0x03, 0xf8, 0xff, 0xe3, 0xff, 0xcf, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x81, 0xff, 0x03, 0xfe, 0x07, 0xff, 0xe0, 
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x1f, 0xc3, 0xff, 0x8f, 0xff, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x1f, 0xc3, 0xff, 0x8f, 0xff, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x1f, 0xc3, 0xff, 0x8f, 0xff, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x1f, 0xc3, 0xff, 0x8f, 0xff, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x1f, 0xc3, 0xff, 0x8f, 0xff, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x1f, 0xc3, 0xff, 0x8f, 0xff, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x1f, 0xc3, 0xff, 0x8f, 0xff, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x04, 0x1f, 0xc3, 0xff, 0x8f, 0xff, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x04, 0x00, 0x03, 0xff, 0x8f, 0xff, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x04, 0x00, 0x03, 0xff, 0x8f, 0xff, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x04, 0x00, 0x03, 0xff, 0x8f, 0xff, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x04, 0x00, 0x03, 0xff, 0x0f, 0xff, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0f, 0xff, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0f, 0xff, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0f, 0xff, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0f, 0xff, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0f, 0xff, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0f, 0xff, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0f, 0xff, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0f, 0xff, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };

#define logo2_width 128
#define logo2_height 64
const unsigned char logo2_bits[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xfc, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x7f, 0xff, 
	0xfe, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x0e, 0xff, 
	0xfb, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x03, 0xbf, 
	0xee, 0x00, 0x7f, 0xff, 0x80, 0x00, 0x01, 0xff, 0xff, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0xe7, 
	0x3c, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x39, 
	0xf0, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x1f, 
	0xe7, 0xff, 0xff, 0x80, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xff, 0xff, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xcf, 0x00, 0x07, 0x07, 0xcf, 0xff, 0xff, 0xf3, 0xf0, 0x00, 0x00, 0xc3, 0xff, 0xff, 
	0xff, 0xff, 0x3e, 0x00, 0xe3, 0xbf, 0xc7, 0xff, 0xff, 0xe3, 0xdc, 0xe0, 0x00, 0x7c, 0xff, 0xff, 
	0xff, 0xfc, 0xfc, 0x00, 0x03, 0xdf, 0xe7, 0xff, 0xff, 0xe7, 0xfb, 0xc0, 0x00, 0x3f, 0x3f, 0xff, 
	0xff, 0xe3, 0xfc, 0x00, 0x03, 0xef, 0xef, 0xff, 0xff, 0xe7, 0xf7, 0xc0, 0x00, 0x3f, 0xcf, 0xff, 
	0xff, 0xcc, 0x7e, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x7f, 0x33, 0xff, 
	0xff, 0xff, 0xc0, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xe1, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x7f, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x8f, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xf9, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xf9, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 0xf3, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xe7, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0xff, 0xe7, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x7f, 0xff, 0xcf, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x7f, 0xff, 0xff, 0xcf, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xfc, 0x7c, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

void setup() {
  SerialBT.begin("ESP32-GAHO");
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  pinMode(PULSADOR_PIN, INPUT_PULLUP);
  pinMode(FRECUENCIA_PIN, INPUT);

  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  attachInterrupt(digitalPinToInterrupt(PULSADOR_PIN), contarPulsador, CHANGE);
  attachInterrupt(digitalPinToInterrupt(FRECUENCIA_PIN), contarFrecuencia, RISING);

  // Configuración PWM para cada canal
  ledcAttach(PWM_CH_ROJO, PWM_FREQ, PWM_RES);
  ledcAttach(PWM_CH_VERDE, PWM_FREQ, PWM_RES);
  ledcAttach(PWM_CH_AZUL, PWM_FREQ, PWM_RES);

  // Asignar cada canal a su pin

Serial.begin(115200);
  dht.begin();
}

void contarPulsador() {
  bool estado = digitalRead(PULSADOR_PIN);

  if (!esperandoBajada && estado == HIGH) {
    esperandoBajada = true;
  } else if (esperandoBajada && estado == LOW) {
    pulsosBoton++;
    esperandoBajada = false;
  }
}

void contarFrecuencia() {
  cuentaFrecuencia++;
}

void mostrarLogo() {
  display.clearDisplay();
  
  // Mostrar la primera imagen
  if (imagenActual == 1) {
    display.drawBitmap(0, 0, logo_bits, logo_width, logo_height, SSD1306_WHITE);
  } 
  // Mostrar la segunda imagen
  else if (imagenActual == 2) {
    display.drawBitmap(0, 0, logo2_bits, logo2_width, logo2_height, SSD1306_WHITE);
  }
  
  display.display();
  
  // Solo inicializar el timer si no estaba activo
  if (!mostrarLogoActiva) {
    mostrarLogoActiva = true;
    tiempoInicioLogo = millis();
    imagenActual = 1; // Empezar siempre con la primera imagen
  }
}

void actualizarPantallaDatos() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(35, 0);
  display.print("--DATOS--");
  display.setCursor(0, 10);
  display.print("Temperatura: ");
  display.print(ultimaTemperatura);
  display.println(" C");
  display.setCursor(0, 20);
  display.print("Humedad: ");
  display.print(ultimaHumedad);
  display.println(" %");
  display.setCursor(0, 30);
  display.print("Luz: ");
  display.print(ultimaLuz);
  display.println(" %");
  display.setCursor(0, 40);
  display.print("Pulsos: ");
  display.println(ultimosPulsos);
  display.setCursor(0, 50);
  display.print("Frecuencia: ");
  display.print(ultimaFrecuencia);
  display.println(" Hz");
  display.display();
}

void loop() {
  unsigned long tiempoActual = millis();
  
  // Gestión de las imágenes del logo
  if (mostrarLogoActiva) {
    unsigned long tiempoTranscurrido = tiempoActual - tiempoInicioLogo;
    
    // Cambiar a la segunda imagen después de 3 segundos
    if (tiempoTranscurrido >= DURACION_CADA_LOGO && imagenActual == 1) {
      imagenActual = 2;
      display.clearDisplay();
      display.drawBitmap(0, 0, logo2_bits, logo2_width, logo2_height, SSD1306_WHITE);
      display.display();
    }
    
    // Terminar secuencia de logos después de 6 segundos
    if (tiempoTranscurrido >= DURACION_TOTAL_LOGOS) {
      mostrarLogoActiva = false;
      imagenActual = 1; // Reset para la próxima vez
      // Actualizar inmediatamente la pantalla con los datos actuales
      actualizarPantallaDatos();
    }
  }
  
  // Actualización de sensores cada segundo
  if (tiempoActual - tiempoAnterior >= intervalo) {
    tiempoAnterior = tiempoActual;
    
    // Capturar y reiniciar contadores (con interrupciones bloqueadas)
    noInterrupts();
    unsigned long frecuencia = cuentaFrecuencia;
    cuentaFrecuencia = 0;
    unsigned long pulsos = pulsosBoton;
    interrupts();

    // Sensor LDR
    int valorBruto = analogRead(LDR_PIN);
    float Vin = (valorBruto / 4095.0) * Vcc;
    float luz = map(valorBruto, 0, 4095, 100, 0); // Luz % invertido
    luz = constrain(luz, 0.0, 100.0);

    // Sensor DHT
    float temperatura = dht.readTemperature();
    float humedad = dht.readHumidity();

    // Actualizar variables globales con los últimos datos
    ultimaTemperatura = temperatura;
    ultimaHumedad = humedad;
    ultimaLuz = luz;
    ultimaFrecuencia = frecuencia;
    ultimosPulsos = pulsos;

    // Solo mostrar datos en pantalla si no se está mostrando el logo
    if (!mostrarLogoActiva) {
      actualizarPantallaDatos();
    }

    // Enviar datos por Bluetooth
    String datos = 
    "Temperatura: " + String(temperatura) + " C, " +
    "Humedad: " + String(humedad) + " %, " +
    "Luz: " + String(luz) + " %, " +
    "Frecuencia: " + String(frecuencia) + " Hz, " +
    "Pulsos: " + String(pulsos);
    SerialBT.println(datos);
  }

  // Procesar comandos por Bluetooth
  if (SerialBT.available()) {
    String comando = SerialBT.readStringUntil('\n');
    comando.trim(); 
    if (comando.startsWith("PMW:")) {
      int valor = comando.substring(4).toInt(); // extrae número tras "PMW:"
      valor = constrain(valor, 0, 255);
      ledcWrite(PWM_CH_ROJO, valor);
      ledcWrite(PWM_CH_VERDE, valor);
      ledcWrite(PWM_CH_AZUL, valor);
    }
    if (comando.equalsIgnoreCase("LOGO")) {
      mostrarLogo();
    } else if (comando == "LED1") {
      estadoLED1 = !estadoLED1;
      digitalWrite(LED1, estadoLED1 ? HIGH : LOW);
    } else if (comando == "LED2") {
      estadoLED2 = !estadoLED2;
      digitalWrite(LED2, estadoLED2 ? HIGH : LOW);
    } else if (comando == "LED3") {
      estadoLED3 = !estadoLED3;
      digitalWrite(LED3, estadoLED3 ? HIGH : LOW);
    } else if (comando == "LED4") {
      estadoLED4 = !estadoLED4;
      digitalWrite(LED4, estadoLED4 ? HIGH : LOW);
    }
    Serial.println(comando); 
  }
}
