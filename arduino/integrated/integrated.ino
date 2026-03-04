#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>

// MPU object
Adafruit_MPU6050 mpu;

// Orientation angles
float pitch = 0, roll = 0, yaw = 0;
unsigned long lastTime = 0;

// Gyroscope offsets
float gyroOffsetX = 0, gyroOffsetY = 0, gyroOffsetZ = 0;

// Complementary filter constant
const float alpha = 0.98;

// ====== Flex + WiFi UDP Communication ======
#include <WiFi.h>
#include <WiFiUdp.h>

// WiFi credentials
const char* ssid = "Free";
const char* password = "hehehaha";

// Static IP config
IPAddress staticIP(192, 168, 137, 50);
IPAddress gateway(192, 168, 137, 1);
IPAddress subnet(255, 255, 255, 0);

// Unity/PC IP and port
IPAddress pcIP(192, 168, 137, 1);
unsigned int port = 25666;
WiFiUDP Udp;

// Flex sensor pins (thumb, index, middle, ring)
const int FLEX_PINS[4] = {13, 32, 33, 4};

// Calibrated flex values
const int FLEX_FLAT[4] = {850, 870, 660, 840};  // Hand open
const int FLEX_BEND[4] = {200, 200, 210, 210};  // Hand closed

float smoothedRaw[4] = {0};
const float SMOOTHING_FACTOR = 0.2;

// ========== Gyroscope Calibration ==========
void calibrateGyro() {
  const int samples = 500;
  float sumX = 0, sumY = 0, sumZ = 0;
  sensors_event_t a, g, temp;

  Serial.println("Calibrating gyro... Keep sensor still.");
  for (int i = 0; i < samples; i++) {
    mpu.getEvent(&a, &g, &temp);
    sumX += g.gyro.x;
    sumY += g.gyro.y;
    sumZ += g.gyro.z;
    delay(5);
  }

  gyroOffsetX = sumX / samples;
  gyroOffsetY = sumY / samples;
  gyroOffsetZ = sumZ / samples;
  Serial.println("Gyro calibration complete.");
}

// ========== Setup ==========
void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  // WiFi connect
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

  // MPU init
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1) delay(10);
  }
  Serial.println("MPU6050 connected!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  delay(100);

  // Gyro calibration
  calibrateGyro();
  lastTime = micros();

  // Initialize smoothed flex values
  for (int i = 0; i < 4; i++) {
    smoothedRaw[i] = analogRead(FLEX_PINS[i]);
  }
}

// ========== Main Loop ==========
void loop() {
  // Get sensor data
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  unsigned long currentTime = micros();
  float dt = (currentTime - lastTime) / 1000000.0;
  lastTime = currentTime;

  // Gyro deg/s with calibration offset
  float gx = (g.gyro.x - gyroOffsetX) * 180.0 / PI;
  float gy = (g.gyro.y - gyroOffsetY) * 180.0 / PI;
  float gz = (g.gyro.z - gyroOffsetZ) * 180.0 / PI;

  // Accel angles
  float accRoll = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI;
  float accPitch = atan2(-a.acceleration.x,
                  sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180.0 / PI;

  // Complementary filter
  roll = alpha * (roll + gx * dt) + (1 - alpha) * accRoll;
  pitch = alpha * (pitch + gy * dt) + (1 - alpha) * accPitch;

  // Integrate yaw from gyro only
  yaw += gz * dt;
  if (yaw > 180) yaw -= 360;
  if (yaw < -180) yaw += 360;

  // Read and process flex sensor values
  int bendValues[4];
  for (int i = 0; i < 4; i++) {
    int raw = analogRead(FLEX_PINS[i]);
    smoothedRaw[i] = (1 - SMOOTHING_FACTOR) * smoothedRaw[i] + SMOOTHING_FACTOR * raw;
    float normalized = ((smoothedRaw[i] - FLEX_FLAT[i]) / (FLEX_BEND[i] - FLEX_FLAT[i])) * 100.0;
    normalized = constrain(normalized, 0.0, 100.0);
    bendValues[i] = (int)normalized;
  }

  // Create output buffer (pitch,roll,yaw,thumb,index,middle,ring)
  char buffer[64];
  sprintf(buffer, "%.1f,%.1f,%.1f,%d,%d,%d,%d", pitch, roll, yaw,
          bendValues[0], bendValues[1], bendValues[2], bendValues[3]);

  // Send via UDP to Unity
  Udp.beginPacket(pcIP, port);
  Udp.write((const uint8_t*)buffer, strlen(buffer));
  Udp.endPacket();

  // Debug output (optional)
  Serial.println(buffer);

  delay(10);
}

// #include <Adafruit_MPU6050.h>
// #include <Adafruit_Sensor.h>
// #include <Wire.h>
// #include <math.h>
// #include <WiFi.h>
// #include <WiFiUdp.h>

// // MPU object
// Adafruit_MPU6050 mpu;

// // Orientation angles
// float pitch = 0, roll = 0, yaw = 0;
// unsigned long lastTime = 0;

// // Gyroscope offsets
// float gyroOffsetX = 0, gyroOffsetY = 0, gyroOffsetZ = 0;

// // Complementary filter constant
// const float alpha = 0.98;

// // WiFi credentials
// const char* ssid = "Free";
// const char* password = "hehehaha";

// // Static IP config
// IPAddress staticIP(192, 168, 137, 50);
// IPAddress gateway(192, 168, 137, 1);
// IPAddress subnet(255, 255, 255, 0);

// // Unity/PC IP and port
// IPAddress pcIP(192, 168, 137, 1);
// unsigned int port = 25666;
// WiFiUDP Udp;

// // Flex sensor pins (ADC1 only for WiFi compatibility)
// const int FLEX_PINS[4] = {32, 33, 34, 35};  // All ADC1 pins

// // Calibrated flex values (open and closed)
// const int FLEX_FLAT[4] = {850, 870, 660, 840};  // Hand open
// const int FLEX_BEND[4] = {200, 200, 210, 210};  // Hand closed

// float smoothedRaw[4] = {0};
// const float SMOOTHING_FACTOR = 0.2;

// // ========== Gyroscope Calibration ==========
// void calibrateGyro() {
//   const int samples = 500;
//   float sumX = 0, sumY = 0, sumZ = 0;
//   sensors_event_t a, g, temp;

//   Serial.println("Calibrating gyro... Keep sensor still.");
//   for (int i = 0; i < samples; i++) {
//     mpu.getEvent(&a, &g, &temp);
//     sumX += g.gyro.x;
//     sumY += g.gyro.y;
//     sumZ += g.gyro.z;
//     delay(5);
//   }

//   gyroOffsetX = sumX / samples;
//   gyroOffsetY = sumY / samples;
//   gyroOffsetZ = sumZ / samples;
//   Serial.println("Gyro calibration complete.");
// }

// // ========== Setup ==========
// void setup() {
//   Serial.begin(115200);
//   analogReadResolution(12); // 12-bit for ESP32

//   // WiFi connect
//   WiFi.config(staticIP, gateway, subnet);
//   WiFi.begin(ssid, password);
//   Serial.print("Connecting to WiFi");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\nWiFi connected. IP: ");
//   Serial.println(WiFi.localIP());
//   Udp.begin(port);

//   // MPU init
//   if (!mpu.begin()) {
//     Serial.println("MPU6050 not found!");
//     while (1) delay(10);
//   }
//   Serial.println("MPU6050 connected!");
//   mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
//   mpu.setGyroRange(MPU6050_RANGE_500_DEG);
//   mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
//   delay(100);

//   // Gyro calibration
//   calibrateGyro();
//   lastTime = micros();

//   // Initialize smoothed flex values
//   for (int i = 0; i < 4; i++) {
//     smoothedRaw[i] = analogRead(FLEX_PINS[i]);
//   }
// }

// // ========== Main Loop ==========
// void loop() {
//   // Get sensor data
//   sensors_event_t a, g, temp;
//   mpu.getEvent(&a, &g, &temp);

//   unsigned long currentTime = micros();
//   float dt = (currentTime - lastTime) / 1000000.0;
//   lastTime = currentTime;

//   // Gyro deg/s with calibration offset
//   float gx = (g.gyro.x - gyroOffsetX) * 180.0 / PI;
//   float gy = (g.gyro.y - gyroOffsetY) * 180.0 / PI;
//   float gz = (g.gyro.z - gyroOffsetZ) * 180.0 / PI;

//   // Accel angles
//   float accRoll = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI;
//   float accPitch = atan2(-a.acceleration.x,
//                   sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180.0 / PI;

//   // Complementary filter
//   roll = alpha * (roll + gx * dt) + (1 - alpha) * accRoll;
//   pitch = alpha * (pitch + gy * dt) + (1 - alpha) * accPitch;

//   // Integrate yaw from gyro only
//   yaw += gz * dt;
//   if (yaw > 180) yaw -= 360;
//   if (yaw < -180) yaw += 360;

//   // Read and process flex sensor values
//   int bendValues[4];
//   for (int i = 0; i < 4; i++) {
//     int raw = analogRead(FLEX_PINS[i]);
//     smoothedRaw[i] = (1 - SMOOTHING_FACTOR) * smoothedRaw[i] + SMOOTHING_FACTOR * raw;
//     float normalized = ((smoothedRaw[i] - FLEX_FLAT[i]) / (FLEX_BEND[i] - FLEX_FLAT[i])) * 100.0;
//     normalized = constrain(normalized, 0.0, 100.0);
//     bendValues[i] = (int)normalized;
//   }

//   // Create output buffer (pitch,roll,yaw,thumb,index,middle,ring)
//   char buffer[64];
//   sprintf(buffer, "%.1f,%.1f,%.1f,%d,%d,%d,%d", pitch, roll, yaw,
//           bendValues[0], bendValues[1], bendValues[2], bendValues[3]);

//   // Send via UDP to Unity
//   Udp.beginPacket(pcIP, port);
//   Udp.write((const uint8_t*)buffer, strlen(buffer));
//   Udp.endPacket();

//   // Debug output (optional)
//   Serial.println(buffer);

//   delay(10);
// }
