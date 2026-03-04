#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>
#include <WiFi.h>
#include <WiFiUdp.h>

Adafruit_MPU6050 mpu;

// Orientation variables
float pitch = 0, roll = 0, yaw = 0;
unsigned long lastTime = 0;
float gyroOffsetX = 0, gyroOffsetY = 0, gyroOffsetZ = 0;
const float alpha = 0.98;

// WiFi setup
const char* ssid = "Free";
const char* password = "hehehaha";
IPAddress staticIP(192, 168, 137, 50);
IPAddress gateway(192, 168, 137, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress pcIP(192, 168, 137, 1);
unsigned int port = 25666;
WiFiUDP Udp;

// Flex sensors (thumb on 32, index on 33)
const int FLEX_PINS[2] = {32, 33};
const int FLEX_FLAT[2] = {850, 870};  // Open hand values
const int FLEX_BEND[2] = {200, 200};  // Closed hand values
float smoothedRaw[2] = {0};
const float SMOOTHING_FACTOR = 0.2;

void calibrateGyro() {
  const int samples = 500;
  float sumX = 0, sumY = 0, sumZ = 0;
  sensors_event_t a, g, temp;

  Serial.println("Calibrating gyro...");
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

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  // WiFi connection
  WiFi.config(staticIP, gateway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Udp.begin(port);

  // MPU6050 initialization
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1) delay(10);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  delay(100);

  calibrateGyro();
  lastTime = micros();

  // Initialize flex readings
  for (int i = 0; i < 2; i++) {
    smoothedRaw[i] = analogRead(FLEX_PINS[i]);
  }
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Calculate time difference
  unsigned long currentTime = micros();
  float dt = (currentTime - lastTime) / 1000000.0;
  lastTime = currentTime;

  // Gyro calculations
  float gx = (g.gyro.x - gyroOffsetX) * 180.0 / PI;
  float gy = (g.gyro.y - gyroOffsetY) * 180.0 / PI;
  float gz = (g.gyro.z - gyroOffsetZ) * 180.0 / PI;

  // Accelerometer angles
  float accRoll = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI;
  float accPitch = atan2(-a.acceleration.x, 
                  sqrt(pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2)) * 180.0 / PI;

  // Complementary filter
  roll = alpha * (roll + gx * dt) + (1 - alpha) * accRoll;
  pitch = alpha * (pitch + gy * dt) + (1 - alpha) * accPitch;
  yaw += gz * dt;

  // Keep yaw between -180 and 180
  if (yaw > 180) yaw -= 360;
  else if (yaw < -180) yaw += 360;

  // Read flex sensors
  int bendValues[2];
  for (int i = 0; i < 2; i++) {
    int raw = analogRead(FLEX_PINS[i]);
    smoothedRaw[i] = (1 - SMOOTHING_FACTOR) * smoothedRaw[i] + SMOOTHING_FACTOR * raw;
    float normalized = ((smoothedRaw[i] - FLEX_FLAT[i]) / (FLEX_BEND[i] - FLEX_FLAT[i])) * 100.0;
    bendValues[i] = constrain(normalized, 0, 100);
  }

  // Create message: pitch,roll,yaw,thumb,index,index,index
  char buffer[64];
  sprintf(buffer, "%.1f,%.1f,%.1f,%d,%d,%d,%d", 
          pitch, roll, yaw,
          bendValues[0],  // Thumb
          bendValues[1],  // Index
          bendValues[1],  // Middle (same as index)
          bendValues[1]); // Ring (same as index)

  // Send UDP packet
  Udp.beginPacket(pcIP, port);
  Udp.write((const uint8_t*)buffer, strlen(buffer));
  Udp.endPacket();

  delay(10);
}