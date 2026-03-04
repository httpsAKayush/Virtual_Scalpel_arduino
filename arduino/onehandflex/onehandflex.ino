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

WiFiUDP Udp;

// Flex sensor pins (excluding pinky)
const int FLEX_PINS[4] = {15, 32, 33, 34}; // Index, Middle, Ring, Thumb

// Calibrated flat and bend values for each finger
const int FLEX_FLAT[4] = {850, 870, 860, 840}; // Straight position
const int FLEX_BEND[4] = {300, 320, 310, 290}; // Fully bent position

float smoothedRaw[4] = {0};
const float SMOOTHING_FACTOR = 0.2;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

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

  // Initialize smoothed values
  for (int i = 0; i < 4; i++) {
    smoothedRaw[i] = analogRead(FLEX_PINS[i]);
  }
}

void loop() {
  int bendValues[4];

  for (int i = 0; i < 4; i++) {
    int raw = analogRead(FLEX_PINS[i]);

    // Exponential smoothing
    smoothedRaw[i] = (1 - SMOOTHING_FACTOR) * smoothedRaw[i] + SMOOTHING_FACTOR * raw;

    // Normalize to 0–100
    float normalized = ((smoothedRaw[i] - FLEX_FLAT[i]) / (FLEX_BEND[i] - FLEX_FLAT[i])) * 100.0;
    normalized = constrain(normalized, 0.0, 100.0);
    bendValues[i] = (int)normalized;

    // Debug output
    Serial.print("Finger ");
    Serial.print(i);
    Serial.print(" - Raw: ");
    Serial.print(raw);
    Serial.print(" | Smoothed: ");
    Serial.print(smoothedRaw[i], 1);
    Serial.print(" -> Normalized: ");
    Serial.println(normalized, 1);
  }

  // Prepare buffer like "45,67,89,12"
  char buffer[32];
  sprintf(buffer, "%d,%d,%d,%d", bendValues[0], bendValues[1], bendValues[2], bendValues[3]);

  // Send over UDP
  Udp.beginPacket(pcIP, port);
  Udp.write((const uint8_t*)buffer, strlen(buffer));
  Udp.endPacket();

  delay(200);
}
