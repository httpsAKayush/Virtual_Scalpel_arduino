// Include required libraries for WiFi and UDP support
#include <WiFi.h>                // Provides WiFi connectivity functions
#include <WiFiUdp.h>             // Provides support for UDP packet handling

// Define WiFi credentials and static network configuration
const char* ssid = "Ak";         // Your WiFi network name
const char* password = "12345678";  // Password for WiFi network
IPAddress staticIP(192, 168, 137, 50);  // Fixed local IP so receivers always know where to find this device
IPAddress gateway(192, 168, 137, 1);      // Gateway (router) address on your network
IPAddress subnet(255, 255, 255, 0);       // Subnet mask defining your network range

// Define broadcast target and port for UDP messages
IPAddress ipBroadCast(255, 255, 255, 255);  // Broadcast address sends data to all devices on the local subnet
unsigned int port = 25667;                  // Port number on which both sender and receiver will operate

// Create a WiFiUDP object for sending (and possibly receiving) UDP data
WiFiUDP Udp;    // This object handles assembling and sending UDP packets

// In the setup() function (executed once at start)
void setup() {
  // Configure the static IP before connecting
  WiFi.config(staticIP, gateway, subnet);
  // Begin connecting to your WiFi network using credentials
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  // Block until WiFi connection is established
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); // Wait half a second between checks
    Serial.print(".");
  }
  Serial.println("\nWiFi connected. IP: ");
  Serial.println(WiFi.localIP());

  // Start the UDP client on the defined port
  Udp.begin(port);   // Binds the UDP socket to 'port'; useful if later you need to receive as well

  // ... [Additional sensor initialization and calibration code would be here]
}

// In the loop() function, which repeats continuously
void loop() {
  // Assume sensor values (pitch, roll, yaw, and flex sensor values) have been computed
  // Format the sensor data into a CSV (comma separated values) string
  char buffer[128];
  sprintf(buffer, "%.2f,%.2f,%.2f,%d,%d,%d,%d", pitch, roll, yaw, bendValues[0], bendValues[1], bendValues[2], bendValues[3]);
  // 'buffer' now might look like: "15.23,3.67,-45.12,80,50,30,90"
  
  // Begin constructing a UDP packet destined to the broadcast IP and port
  Udp.beginPacket(ipBroadCast, port);
  // Write the formatted string into the packet payload as a stream of bytes
  Udp.write((const uint8_t*)buffer, strlen(buffer));
  // End and send the packet out over the network
  Udp.endPacket();

  // Optional: Print to Serial for debugging
  Serial.println(buffer);
  
  // Delay briefly to control the rate of UDP messages sent
  delay(10); // e.g., every 10 ms for near real-time updates
}
