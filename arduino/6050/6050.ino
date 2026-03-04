// #include <Adafruit_MPU6050.h>
// #include <Adafruit_Sensor.h>
// #include <Wire.h>

// Adafruit_MPU6050 mpu;

// float pitch = 0, roll = 0, yaw = 0;
// unsigned long lastTime = 0;

// float gyroOffsetZ = 0;

// void calibrateGyro() {
//   const int samples = 500;
//   float sumZ = 0;
//   sensors_event_t a, g, temp;

//   Serial.println("Calibrating... Keep sensor still.");
//   for (int i = 0; i < samples; i++) {
//     mpu.getEvent(&a, &g, &temp);
//     sumZ += g.gyro.z;
//     delay(5);
//   }

//   gyroOffsetZ = sumZ / samples;
//   Serial.println("Calibration done.");
// }

// void setup() {
//   Serial.begin(115200);
//   while (!Serial) delay(10);

//   if (!mpu.begin()) {
//     Serial.println("MPU6050 not found!");
//     while (1) delay(10);
//   }
//   Serial.println("MPU6050 ready!");

//   mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
//   mpu.setGyroRange(MPU6050_RANGE_500_DEG);
//   mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
//   delay(100);

//   calibrateGyro();

//   lastTime = micros();
// }

// void loop() {
//   sensors_event_t a, g, temp;
//   mpu.getEvent(&a, &g, &temp);

//   unsigned long currentTime = micros();
//   float dt = (currentTime - lastTime) / 1000000.0;
//   lastTime = currentTime;

//   // Convert radians/sec to degrees/sec
//   float gz = (g.gyro.z - gyroOffsetZ) * 180.0 / PI;

//   // Integrate yaw angle in degrees
//   yaw += gz * dt;

//   // Optional: normalize yaw between -180 to 180
//   if (yaw > 180) yaw -= 360;
//   if (yaw < -180) yaw += 360;

//   Serial.print("Yaw: ");
//   Serial.println(yaw);

//   delay(5);
// }

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>

Adafruit_MPU6050 mpu;

// Orientation angles
float pitch = 0, roll = 0, yaw = 0;
unsigned long lastTime = 0;

// Gyroscope offsets
float gyroOffsetX = 0, gyroOffsetY = 0, gyroOffsetZ = 0;

// Complementary filter constant
const float alpha = 0.98;

// --- Calibrate gyro when stationary ---
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

  // Output values
  Serial.print("Pitch: "); Serial.print(pitch);
  Serial.print("  Roll: "); Serial.print(roll);
  Serial.print("  Yaw: "); Serial.println(yaw);

  delay(5);
}

