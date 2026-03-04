// #include <ESP8266WiFi.h>
// #include <WiFiUdp.h>
// #include <Wire.h>

// const char* ssid = "Free";
// const char* password = "hehehaha";

// // Network config
// IPAddress staticIP(192, 168, 137, 50);
// IPAddress gateway(192, 168, 137, 1);
// IPAddress subnet(255, 255, 255, 0);
// IPAddress pcIP(192, 168, 137, 1);  // Replace with your PC's IP
// unsigned int port = 25665;

// WiFiUDP Udp;

// #define MPU_addr 0x68
// #define AC_NUM_TO_AVG 3

// float acReadings[3][AC_NUM_TO_AVG + 1];
// int acAvgIndex = 0;
// float acInst[3] = {0,0,0};
// float acAvg[3] = {0,0,0};
// float acPR[2] = {0,0};    
// float gyDeltaPRY[3] = {0,0,0};
// float acGyOffset[6] = {-247.69, -133.50, -68.48, 905.85, -151.32, -110.68};
// float gyAcMix = 0.95;
// float pry[3] = {0,0,0};

// float timeNow = 0;
// float timePrev = 0;
// float timeDelta = 0;

// float rad2deg(float rad){
//   return (rad*180.0)/PI;
// }

// void updateAcReadings(){
//   Wire.beginTransmission(MPU_addr);
//   Wire.write(0x3B);
//   Wire.endTransmission(false);
//   Wire.requestFrom(MPU_addr, 6, true);
//   int16_t xAcRaw = (Wire.read() << 8 | Wire.read());
//   int16_t yAcRaw = (Wire.read() << 8 | Wire.read());
//   int16_t zAcRaw = (Wire.read() << 8 | Wire.read());

//   acInst[0] = (float)xAcRaw + acGyOffset[0];
//   acInst[1] = (float)yAcRaw + acGyOffset[1];
//   acInst[2] = (float)zAcRaw + acGyOffset[2];
// }

// void updateAcAvg(){
//   for(int i = 0; i < 3; i++){
//     acReadings[i][AC_NUM_TO_AVG] -= acReadings[i][acAvgIndex];
//     acReadings[i][acAvgIndex] = acInst[i];
//     acReadings[i][AC_NUM_TO_AVG] += acReadings[i][acAvgIndex];
//     acAvg[i] = acReadings[i][AC_NUM_TO_AVG] / AC_NUM_TO_AVG;
//   }
//   acAvgIndex = (acAvgIndex + 1) % AC_NUM_TO_AVG;
// }

// void calculateAcRP(){
//   acPR[0] = rad2deg(atan(acAvg[0]/acAvg[2]));
//   acPR[1] = rad2deg(atan(acAvg[1]/acAvg[2]));
// }

// void updateGyReadings(){
//   Wire.beginTransmission(MPU_addr);
//   Wire.write(0x43);
//   Wire.endTransmission(false);
//   Wire.requestFrom(MPU_addr, 6, true);
//   int16_t xGyRaw = Wire.read() << 8 | Wire.read();
//   int16_t yGyRaw = Wire.read() << 8 | Wire.read();
//   int16_t zGyRaw = Wire.read() << 8 | Wire.read();

//   gyDeltaPRY[1] = ((float)xGyRaw + acGyOffset[3])/65.5;
//   gyDeltaPRY[0] = -((float)yGyRaw + acGyOffset[4])/65.5;
//   gyDeltaPRY[2] = ((float)zGyRaw + acGyOffset[5])/65.5;
// }

// void setup() {
//   Serial.begin(115200);
//   Wire.begin();

//   WiFi.config(staticIP, gateway, subnet);
//   WiFi.begin(ssid, password);

//   Serial.print("Connecting to WiFi");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\nWiFi connected");
//   Serial.print("ESP IP address: ");
//   Serial.println(WiFi.localIP());

//   Udp.begin(port);

//   // MPU6050 setup
//   Wire.beginTransmission(MPU_addr);
//   Wire.write(0x6B); Wire.write(0x00);
//   Wire.endTransmission(true);
//   delay(50);

//   Wire.beginTransmission(MPU_addr);
//   Wire.write(0x1B); Wire.write(0b00001000); // Gyro config: 500 dps
//   Wire.endTransmission(true);

//   Wire.beginTransmission(MPU_addr);
//   Wire.write(0x1C); Wire.write(0b00010000); // Accel config: 4g
//   Wire.endTransmission(true);

//   // Init accel avg buffer
//   for(int x = 0; x < 3; x++){
//     for(int y = 0; y < AC_NUM_TO_AVG + 1; y++){
//       acReadings[x][y] = 0;
//     }
//   }
// }

// void loop() {
//   updateAcReadings();
//   updateAcAvg();
//   calculateAcRP();
//   updateGyReadings();

//   timeNow = micros()/1000000.0;
//   timeDelta = timeNow - timePrev;

//   pry[0] = gyAcMix * (pry[0] + gyDeltaPRY[0] * timeDelta ) + (1-gyAcMix) * (acPR[0]);
//   pry[1] = gyAcMix * (pry[1] + gyDeltaPRY[1] * timeDelta) + (1-gyAcMix) * (acPR[1]);
//   pry[2] = pry[2] + gyDeltaPRY[2] * timeDelta;

//   timePrev = timeNow;

//   // Send data to Unity
//   char data[50];
//   sprintf(data, "%.2f,%.2f,%.2f", pry[0], pry[1], pry[2]);
//   Udp.beginPacket(pcIP, port);
//   Udp.write(data);
//   Udp.endPacket();

//   Serial.println(data);
//   delay(10);
// }


