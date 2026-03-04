#include <WiFi.h>
#include <WiFiUdp.h>

// WiFi credentials
const char* ssid = "Galaxy";
const char* password = "chaandsitare";

// Static IP config
IPAddress staticIP(192, 168, 137, 50);
IPAddress gateway(192, 168, 137, 1);
IPAddress subnet(255, 255, 255, 0);

// Unity/PC IP and port
//IPAddress pcIP(192, 168, 137, 1);
IPAddress ipBroadCast(255, 255, 255, 255);
unsigned int port = 25666;
WiFiUDP Udp;

// Flex sensor pins (thumb, index, middle, ring)
const int FLEX_PINS[4] = {32, 33, 34, 35};

// Calibrated flex values
const int FLEX_FLAT[4] = {800,800, 660, 840};  // Hand open
const int FLEX_BEND[4] = {500, 200, 310, 410};  // Hand closed

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
  Serial.println("\nWiFi connected. IP: ");
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
    smoothedRaw[i] = (1 - SMOOTHING_FACTOR) * smoothedRaw[i] + SMOOTHING_FACTOR * raw;
    float normalized = ((smoothedRaw[i] - FLEX_FLAT[i]) / (FLEX_BEND[i] - FLEX_FLAT[i])) * 100.0;
    normalized = constrain(normalized, 0.0, 100.0);
    bendValues[i] = (int)normalized;
  }
  for(int i=0; i<4; i++){
    Serial.println(smoothedRaw[i]);
  }

  // Create UDP packet (thumb,index,middle,ring)
  char buffer[32];
  sprintf(buffer, "%d,%d,%d,%d", bendValues[0], bendValues[1], bendValues[2], bendValues[3]);

  Udp.beginPacket(ipBroadCast, port);
  Udp.write((const uint8_t*)buffer, strlen(buffer));
  Udp.endPacket();

  Serial.println(buffer); // Optional debug
  delay(10);
}
