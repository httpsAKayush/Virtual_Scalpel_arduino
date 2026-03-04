#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "Free";
const char* password = "hehehaha";

// Network
IPAddress staticIP(192, 168, 137, 50);
IPAddress gateway(192, 168, 137, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress pcIP(192, 168, 137, 1);  // Your PC's IP
unsigned int port = 25668;        // Port for flex sensor data

WiFiUDP Udp;

void setup(){

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
void loop(){
int left_data[5];
int right_data[5];
int flex_data[10];

for (int i =0;i<5 ;i++){
  left_data[i] = random(0,100);
  right_data[i] = random(0,100);
}

/*
correct this code lines
*/
Serial.println("left_data" );
for (int i =0 ;i<5;i++){
  Serial.printf("%d ,", left_data[i]);
}
 Serial.print("\n");

Serial.println("right_data");
for (int i =0 ;i<5;i++){
  Serial.printf("%d ,", right_data[i]);
}
  Serial.print("\n");


for (int i=0; i <5;i++){
  flex_data[i]= left_data[i];
  flex_data[i+5]= right_data[i];
}

char data[64];
snprintf(data, sizeof(data), "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
         flex_data[0],flex_data[1],flex_data[2],flex_data[3],flex_data[4],
         flex_data[5],flex_data[6],flex_data[7],flex_data[8],flex_data[9]);

Serial.print("sending:");
Serial.println(data);

Udp.beginPacket(pcIP,port);
Udp.write(data);
Udp.endPacket();

delay(7000);
}