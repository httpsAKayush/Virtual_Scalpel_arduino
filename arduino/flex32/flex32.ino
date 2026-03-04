#include <WiFi.h>
#include <WiFiUdp.h>

// WiFi credentials
const char* ssid = "Free";
const char* password = "hehehaha";

// Static IP configuration
IPAddress staticIP(192, 168, 137, 50);
IPAddress gateway(192, 168, 137, 1);
IPAddress subnet(255, 255, 255, 0);

// Unity (PC) IP and port
IPAddress pcIP(192, 168, 137, 1);
unsigned int port = 25668;

// Flex sensor setup
#define FLEX_PIN 32
const int FLEX_MIN = 850;
const int FLEX_MAX = 300;
const float SMOOTHING_FACTOR = 0.2;  // α for EMA (between 0 and 1)

WiFiUDP Udp;

float smoothedRaw = 0;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);  // 12-bit ADC (0–4095)

  // Connect to WiFi
  WiFi.config(staticIP, gateway, subnet);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  Udp.begin(port);

  // Initialize the smoothed value with the first read
  smoothedRaw = analogRead(FLEX_PIN);
}

void loop() {
  int raw = analogRead(FLEX_PIN);

  // Apply exponential smoothing
  smoothedRaw = (1 - SMOOTHING_FACTOR) * smoothedRaw + SMOOTHING_FACTOR * raw;

  // Normalize the smoothed value to a 0–100 range
  float normalized = ((smoothedRaw - FLEX_MIN) / (FLEX_MAX - FLEX_MIN)) * 100.0;
  normalized = constrain(normalized, 0.0, 100.0);

  // Debug output
  Serial.print("Raw: ");
  Serial.print(raw);
  Serial.print(" | Smoothed: ");
  Serial.print(smoothedRaw, 1);
  Serial.print(" -> Normalized: ");
  Serial.println(normalized, 1);

  // Send over UDP
  int bendInt = (int)normalized;
  char buffer[10];
  sprintf(buffer, "%d", bendInt);
  Udp.beginPacket(pcIP, port);
  Udp.write((const uint8_t*)buffer, strlen(buffer));
  Udp.endPacket();

  delay(200);
}
