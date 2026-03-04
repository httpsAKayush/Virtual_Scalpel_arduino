// Pin Definitions
#define POTENTIOMETER_PIN A0 // ADC pin for reading the potentiometer

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200); // Set baud rate
  while (!Serial) {
    ; // Wait for Serial port to connect
  }
  
  // Analog pin setup
  pinMode(POTENTIOMETER_PIN, INPUT);
}

void loop() {
  // Read the analog value from the potentiometer
  int potValue = analogRead(POTENTIOMETER_PIN);

  // Print the value to the Serial Monitor
  Serial.print("Potentiometer Value: ");
  Serial.println(potValue);

  // Delay for readability
  delay(500); // Adjust as needed
}
