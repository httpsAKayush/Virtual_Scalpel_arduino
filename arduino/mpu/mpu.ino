#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>  // Required for atan2() and M_PI

Adafruit_MPU6050 mpu;

const int flexPin = A0;  // Flex sensor connected to A0
int flexValue = 0;        // Raw flex sensor value
float bendAngle = 0;      // Calculated angle from flex sensor

void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("MPU6050 + Flex Sensor Test!");

  // MPU6050 initialization
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) delay(10);
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

  delay(100);
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // ✅ MPU6050 Angle Calculation
  float roll_rad = atan2(a.acceleration.y, sqrt(a.acceleration.x * a.acceleration.x + a.acceleration.z * a.acceleration.z));
  float pitch_rad = atan2(-a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z));

  float roll_deg = roll_rad * 180.0 / M_PI;   // x-axis
  float pitch_deg = pitch_rad * 180.0 / M_PI; // y-axis

  // ✅ Flex Sensor Reading
  flexValue = analogRead(flexPin);

  // Map raw flex value to bend angle (Adjust these values based on your flex sensor range)
  bendAngle = map(flexValue, 300, 700, 0, 90);  // Example: 300 -> flat, 700 -> fully bent

  // ✅ Print Accelerometer, Gyro, and Flex Sensor Data
  Serial.println("--------- MPU6050 ---------");
  Serial.print("Accel X: "); Serial.print(a.acceleration.x); Serial.print(" m/s², ");
  Serial.print("Y: "); Serial.print(a.acceleration.y); Serial.print(" m/s², ");
  Serial.print("Z: "); Serial.print(a.acceleration.z); Serial.println(" m/s²");

  Serial.print("Roll: "); Serial.print(roll_deg); Serial.print("° | ");
  Serial.print("Pitch: "); Serial.print(pitch_deg); Serial.println("°");

  Serial.print("Gyro X: "); Serial.print(g.gyro.x * 57.2958); Serial.print("°/s, ");
  Serial.print("Y: "); Serial.print(g.gyro.y * 57.2958); Serial.print("°/s, ");
  Serial.print("Z: "); Serial.print(g.gyro.z * 57.2958); Serial.println("°/s");

  Serial.print("Temp: "); Serial.print(temp.temperature); Serial.println(" °C");

  Serial.println("--------- Flex Sensor ---------");
  Serial.print("Flex Raw Value: "); Serial.print(flexValue);
  Serial.print(" | Bend Angle: "); Serial.print(bendAngle); Serial.println("°");

  Serial.println("---------------------");
  delay(500);
<<<<<<< Updated upstream
}
=======
}
>>>>>>> Stashed changes
