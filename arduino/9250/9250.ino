#include <MPU9250_asukiaaa.h>
#include <Wire.h>

MPU9250_asukiaaa mySensor;

float pitch = 0, roll = 0, yaw = 0;
float gyroOffsetX = 0, gyroOffsetY = 0, gyroOffsetZ = 0;
const float alpha = 0.98;
const float alphaYaw = 0.995;

unsigned long lastTime = 0;

void calibrateGyro() {
  const int samples = 500;
  float sumX = 0, sumY = 0, sumZ = 0;

  Serial.println("Calibrating gyro...");
  for (int i = 0; i < samples; i++) {
    mySensor.accelUpdate();
    mySensor.gyroUpdate();
    sumX += mySensor.gyroX();
    sumY += mySensor.gyroY();
    sumZ += mySensor.gyroZ();
    delay(5);
  }

  gyroOffsetX = sumX / samples;
  gyroOffsetY = sumY / samples;
  gyroOffsetZ = sumZ / samples;
  Serial.println("Gyro calibration done.");
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mySensor.setWire(&Wire);
  mySensor.beginAccel();
  mySensor.beginGyro();

  delay(100);
  calibrateGyro();
  lastTime = micros();
}

void loop() {
  mySensor.accelUpdate();
  mySensor.gyroUpdate();

  float ax = mySensor.accelX();
  float ay = mySensor.accelY();
  float az = mySensor.accelZ();

  float gx = mySensor.gyroX() - gyroOffsetX;
  float gy = mySensor.gyroY() - gyroOffsetY;
  float gz = mySensor.gyroZ() - gyroOffsetZ;

  gx *= 180.0 / PI;
  gy *= 180.0 / PI;
  gz *= 180.0 / PI;

  unsigned long currentTime = micros();
  float dt = (currentTime - lastTime) / 1000000.0;
  lastTime = currentTime;

  float accRoll = atan2(ay, az) * 180.0 / PI;
  float accPitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / PI;

  roll = alpha * (roll + gx * dt) + (1 - alpha) * accRoll;
  pitch = alpha * (pitch + gy * dt) + (1 - alpha) * accPitch;

  // Yaw correction only when az (Z-acceleration) is strong enough
  const float accThreshold = 0.1;  // adjust this as needed
  if (abs(az) > accThreshold) {
    float pseudoYaw = atan2(ax, ay) * 180.0 / PI;
    yaw = alphaYaw * (yaw + gz * dt) + (1 - alphaYaw) * pseudoYaw;
  }

  if (yaw > 180) yaw -= 360;
  if (yaw < -180) yaw += 360;

  Serial.print("Pitch: "); Serial.print(pitch, 2);
  Serial.print("  Roll: "); Serial.print(roll, 2);
  Serial.print("  Yaw: "); Serial.println(yaw, 2);

  delay(5);
}
