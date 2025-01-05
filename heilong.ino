#include <Wire.h>
#include <Battery_Shield.h>

#define PIN_SONIC_A_TRIG 9
#define PIN_SONIC_A_ECHO 10


Battery_Shield pwrBank(0x35);  //0x35 || 0x75

void setup() {
  // Setup code here, to run once
  Serial.begin(9600);
  pwrBank.begin(&Wire, 0.0128f);

  // Set TrigPin as output and EchoPin as input
  pinMode(PIN_SONIC_A_TRIG, OUTPUT);
  pinMode(PIN_SONIC_A_ECHO, INPUT);
}

void loop() {
  // Main code here, to run repeatedly

  // Sonar distance measurement
  // Send a 10-microsecond pulse to trigger pin
  digitalWrite(PIN_SONIC_A_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_SONIC_A_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_SONIC_A_TRIG, LOW);

  // Measure the duration of the echo signal
  auto duration = pulseIn(PIN_SONIC_A_ECHO, HIGH);

  // Calculate distance in centimeters
  float distance = duration * 0.034 / 2;

  // Battery
  auto capacity = pwrBank.getLevel();
  if (capacity > 0) {
    Serial.println(F("-------------------------"));
    Serial.println((String) "Battery: " + pwrBank.voltmeter(BATTERY) + "V, " + pwrBank.amperemeter(BATTERY) + "A");  // battery volts/amperes
    Serial.println((String) "Output: " + pwrBank.voltmeter(OUTPUT) + "V, " + pwrBank.amperemeter(OUTPUT) + "A");     // output volts/amperes
    switch (pwrBank.getState()) {
      case CHARGING_IDLE: Serial.println(F("Status: NOT charging (idle)")); break;
      case CHARGING_TK: Serial.println(F("Status: charging with minimal current")); break;
      case CHARGING_CC: Serial.println(F("Status: charging with constant current (CC)")); break;
      case CHARGING_CV: Serial.println(F("Status: charging with constant voltage (CV)")); break;
      case CHARGING_TO: Serial.println(F("Status: NOT charging (charge time exceeded)")); break;
      default: Serial.println(F("Unknown charging mode")); break;
    }
    Serial.println((String) "Battery capacity " + capacity + "%");
    Serial.println(F("-------------------------"));
    if (pwrBank.amperemeter(OUTPUT) > 0.7) {  // consumption exceeds 700 mA
      Serial.println(F("Consumption overcurrent! Shutting the battery down..."));
      pwrBank.off();  // shut down
    }
    if (capacity < 10) {  // 10% or less capacity remains
      Serial.println(F("Battery has less than 10% of it's capacity. Shut down to protect the battery"));
      pwrBank.off();  // shut down
    }
  } else {
    Serial.println(F("Battery is OFF, not available via I2C, or completely discharged"));
  }

  // Print the distance to the serial monitor
  Serial.println((String) "Distance: " + distance + "cm");

  delay(5000);  // sleep
}
