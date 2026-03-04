#include <WiFi.h>
#include <WiFiUdp.h>

#define FLEX_PIN1 13
#define FLEX_PIN2 25
#define FLEX_PIN3 26
#define FLEX_PIN4 14

const char* ssid = "Free";
const char* password = "hehehaha";

// Unity Server IP and Port
IPAddress serverIP(192, 168, 137, 1);  // No quotes and commas replaced by periods
const int serverPort = 8080;

WiFiUDP udp;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);  // ESP32 ADC resolution (0–4095)

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Read flex sensor values
  float flex1 = analogRead(FLEX_PIN1);
  float flex2 = analogRead(FLEX_PIN2);
  float flex3 = analogRead(FLEX_PIN3);
  float flex4 = analogRead(FLEX_PIN4);

  // Send as float array with a start marker
  float sensorData[5] = { 1.0, flex1, flex2, flex3, flex4 };

  // Debug print
  Serial.printf("Flex: [%.1f, %.1f, %.1f, %.1f]\n", flex1, flex2, flex3, flex4);

  // Send UDP packet
  udp.beginPacket(serverIP, serverPort);
  udp.write((uint8_t*)sensorData, sizeof(sensorData));
  udp.endPacket();

  Serial.println("Data Sent!");
  delay(3000);  // Adjust delay as needed
}
