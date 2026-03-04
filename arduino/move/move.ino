#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
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
IPAddress ipBroadCast(255, 255, 255, 255);
unsigned int port = 25667;
WiFiUDP Udp;

// Flex sensor pins (thumb, index, middle, ring)
const int FLEX_PINS[4] = {32, 33, 34, 35};
const int JOY_X_PIN = 36; // VP - X-axis
const int JOY_Y_PIN = 39; // VN - Y-axis



// Calibrated flex values
const int FLEX_FLAT[4] = {700, 780, 700, 620};  // Hand open
const int FLEX_BEND[4] = {500, 440, 350, 110};  // Hand closed

// MPU6050 setup
Adafruit_MPU6050 mpu;

// Gyroscope offsets
float gyroOffsetX = 0, gyroOffsetY = 0, gyroOffsetZ = 0;

// Complementary filter constants
const float alpha = 0.98;

// Orientation angles
float pitch = 0, roll = 0, yaw = 0;
unsigned long lastTime = 0;

// Smoothing for flex sensors
float smoothedRaw[4] = {0};
const float SMOOTHING_FACTOR = 0.2;

void calibrateGyro() {
  const int samples = 500;
  float sumX = 0, sumY = 0, sumZ = 0;
  sensors_event_t a, g, temp;

  Serial.println("Calibrating gyro... Keep the sensor still.");
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
  Serial.println("Calibration complete.");
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

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

  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1) delay(10);
  }
  Serial.println("MPU6050 connected!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  delay(100);

  calibrateGyro();
  lastTime = micros();
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  unsigned long currentTime = micros();
  float dt = (currentTime - lastTime) / 1000000.0;
  lastTime = currentTime;

  // Gyro readings (rad/s → deg/s) minus offset
  float gx = (g.gyro.x - gyroOffsetX) * 180.0 / PI;
  float gy = (g.gyro.y - gyroOffsetY) * 180.0 / PI;
  float gz = (g.gyro.z - gyroOffsetZ) * 180.0 / PI;

  // Accelerometer angle estimation
  float accRoll = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI;
  float accPitch = atan2(-a.acceleration.x, 
                  sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180.0 / PI;

  // Complementary filter: combine gyro and accelerometer
  roll = alpha * (roll + gx * dt) + (1 - alpha) * accRoll;
  pitch = alpha * (pitch + gy * dt) + (1 - alpha) * accPitch;

  // Yaw from integrated gyro only
  yaw += gz * dt;
  if (yaw > 180) yaw -= 360;
  if (yaw < -180) yaw += 360;

  // Read flex sensor values
  int bendValues[4];
  for (int i = 0; i < 4; i++) {
    int raw = analogRead(FLEX_PINS[i]);
    smoothedRaw[i] = (1 - SMOOTHING_FACTOR) * smoothedRaw[i] + SMOOTHING_FACTOR * raw;
    float normalized = ((smoothedRaw[i] - FLEX_FLAT[i]) / (FLEX_BEND[i] - FLEX_FLAT[i])) * 100.0;
    normalized = constrain(normalized, 0.0, 100.0);
    bendValues[i] = (int)normalized;
   // Serial.println(smoothedRaw[i]);
  }
  int xValue = analogRead(JOY_X_PIN);  // Read X-axis
  int yValue = analogRead(JOY_Y_PIN);  // Read Y-axis

  // Prepare the message: "PITCH,ROLL,YAW,THUMB,INDEX,MIDDLE,RING"
  char buffer[128];
  sprintf(buffer, "%.2f,%.2f,%.2f,%d,%d,%d,%d,%d,%d", pitch, roll, yaw, bendValues[0], bendValues[1], bendValues[2], bendValues[3],xValue, yValue);

  // Send the data over UDP
  Udp.beginPacket(ipBroadCast, port);
  Udp.write((const uint8_t*)buffer, strlen(buffer));
  Udp.endPacket();

  // Debugging output (Optional)
  Serial.println(buffer);

  // Small delay to regulate packet rate
  delay(10);
}
// // Define analog input pins for joystick
// const int JOY_X_PIN = 36; // VP - X-axis
// const int JOY_Y_PIN = 39; // VN - Y-axis


// void setup() {
//   Serial.begin(115200);

//   pinMode(JOY_SW_PIN, INPUT_PULLUP); // Use internal pull-up for button
// }

// void loop() {
//   int xValue = analogRead(JOY_X_PIN);  // Read X-axis
//   int yValue = analogRead(JOY_Y_PIN);  // Read Y-axis
 


//   delay(200);
// }
