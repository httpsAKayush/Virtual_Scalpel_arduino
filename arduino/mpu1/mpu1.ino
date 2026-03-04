#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "Free";
const char* password = "hehehaha";

// Network
IPAddress staticIP(192, 168, 137, 50);
IPAddress gateway(192, 168, 137, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress pcIP(192, 168, 137, 1);  // Your PC's IP
//IPAddress broadcastIP(192, 168, 137, 255);
unsigned int port = 25668;        // Port for flex sensor data

WiFiUDP Udp;

const int flexPin = A0;
const int FLAT_VALUE = 470;
const int BEND_VALUE = 560;
const float SMOOTHING_FACTOR = 
.2;

void setup() {
  Serial.begin(115200);

  WiFi.config(staticIP, gateway, subnet);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("ESP IP: ");
  Serial.println(WiFi.localIP());

  Udp.begin(port);
}

void loop() {
  int rawValue = analogRead(flexPin);
  static float smoothedValue = analogRead(flexPin);
  
  smoothedValue = (1 - SMOOTHING_FACTOR) * smoothedValue + SMOOTHING_FACTOR * rawValue;

  int bendPercentage = map(smoothedValue, FLAT_VALUE, BEND_VALUE, 0,100);
  bendPercentage = constrain(bendPercentage, 0, 100);

  // Serial print debug
  Serial.print("Raw: ");
  Serial.print(rawValue);
  Serial.print(" | Smooth: ");
  Serial.print(smoothedValue, 1);
  Serial.print(" | Bend: ");
  Serial.print(bendPercentage);
  Serial.println("%");

  // Send over UDP
  char data[10];
  sprintf(data, "%d", bendPercentage);
  Udp.beginPacket(pcIP, port);
  Udp.write(data);
  Udp.endPacket();

  delay(50);
}
