/**
 * Blind Assistance System
 * 
 * An ultrasonic-based navigation system for visually impaired individuals
 * Created by: Your Name
 * GitHub: https://github.com/username/blind-assistance
 * License: MIT
 * 
 * Required Libraries (install via Arduino Library Manager):
 * - NewPing (https://github.com/microflo/NewPing)
 * - Tone Library (https://github.com/bhagman/Tone)
 * 
 * Note: The avr/sleep.h and avr/power.h libraries are part of the Arduino core
 * and should already be available in your Arduino installation.
 */

// Include necessary libraries
#include <NewPing.h>    // For ultrasonic sensors
#include <Tone.h>       // For better tone generation
// Arduino core libraries for power management
#include <avr/sleep.h>  
#include <avr/power.h>

// Pin definitions for ultrasonic sensors
#define TRIGGER_PIN_1 2
#define ECHO_PIN_1 3
#define TRIGGER_PIN_2 4
#define ECHO_PIN_2 5
#define TRIGGER_PIN_3 6
#define ECHO_PIN_3 7

// Pin definitions for output and input devices
#define BUZZER_PIN 8
#define EMERGENCY_BTN 9
#define BATTERY_PIN A0  // To monitor battery level

// Configuration parameters
#define MAX_DISTANCE 400       // Maximum distance to ping (cm)
#define BATTERY_THRESHOLD 3.3  // Low battery threshold in volts

// Tone frequencies for different distances (Hz)
#define FAR_TONE 2000
#define MEDIUM_TONE 1500
#define CLOSE_TONE 1000
#define VERY_CLOSE_TONE 500
#define SOS_TONE 2500

// Distance thresholds (cm)
#define VERY_CLOSE_DISTANCE 30
#define CLOSE_DISTANCE 60
#define MEDIUM_DISTANCE 120

// Timing parameters (ms)
#define TONE_DURATION 100
#define PAUSE_DURATION 50
#define SCAN_DELAY 200
#define EMERGENCY_PRESS_TIME 3000
#define BATTERY_CHECK_INTERVAL 30000

// Other configuration
#define DEBUG true  // Set to false to disable debug output

// Create NewPing objects for each sensor
NewPing sonar1(TRIGGER_PIN_1, ECHO_PIN_1, MAX_DISTANCE);  // Left sensor
NewPing sonar2(TRIGGER_PIN_2, ECHO_PIN_2, MAX_DISTANCE);  // Center sensor
NewPing sonar3(TRIGGER_PIN_3, ECHO_PIN_3, MAX_DISTANCE);  // Right sensor

// Create Tone object
Tone buzzer;

// Global variables
unsigned long lastBatteryCheck = 0;
unsigned long emergencyPressStart = 0;
bool emergencyButtonPressed = false;
bool batteryLow = false;

void setup() {
  // Initialize serial communication for debugging
  if (DEBUG) {
    Serial.begin(9600);
    Serial.println(F("Blind Assistance System initializing..."));
  }
  
  // Initialize buzzer
  buzzer.begin(BUZZER_PIN);
  
  // Initialize emergency button with pull-up resistor
  pinMode(EMERGENCY_BTN, INPUT_PULLUP);
  
  // Battery monitoring
  pinMode(BATTERY_PIN, INPUT);
  
  // Startup sound to indicate system is ready
  playStartupSound();
  
  if (DEBUG) {
    Serial.println(F("System initialized and ready!"));
  }
}

void loop() {
  // Check battery level periodically
  checkBattery();
  
  // Check emergency button
  checkEmergencyButton();
  
  // If in emergency mode or battery is low, don't perform normal scanning
  if (emergencyButtonPressed || batteryLow) {
    return;
  }
  
  // Get distances from all sensors with median filtering
  unsigned int distance1 = getFilteredDistance(sonar1);
  unsigned int distance2 = getFilteredDistance(sonar2);
  unsigned int distance3 = getFilteredDistance(sonar3);
  
  // Log distances if debugging is enabled
  if (DEBUG) {
    Serial.print(F("Distances (cm): Left="));
    Serial.print(distance1);
    Serial.print(F(" Center="));
    Serial.print(distance2);
    Serial.print(F(" Right="));
    Serial.println(distance3);
  }
  
  // Process each sensor's data
  processSensor(distance1, 1);  // Left
  processSensor(distance2, 2);  // Center
  processSensor(distance3, 3);  // Right
  
  // Small delay between scanning cycles to save power
  delay(SCAN_DELAY);
}

// Get median filtered distance from sensor to reduce false readings
unsigned int getFilteredDistance(NewPing &sonar) {
  unsigned int distances[3];
  for (int i = 0; i < 3; i++) {
    distances[i] = sonar.ping_cm();
    delay(10);
  }
  
  // Simple bubble sort to find median
  if (distances[0] > distances[1]) swap(distances[0], distances[1]);
  if (distances[1] > distances[2]) swap(distances[1], distances[2]);
  if (distances[0] > distances[1]) swap(distances[0], distances[1]);
  
  return distances[1];  // Return median value
}

// Utility function to swap values
void swap(unsigned int &a, unsigned int &b) {
  unsigned int temp = a;
  a = b;
  b = temp;
}

void processSensor(unsigned int distance, int sensorNum) {
  // Skip if no valid reading
  if (distance == 0) return;
  
  // Determine tone based on distance
  int toneFreq;
  if (distance < VERY_CLOSE_DISTANCE) {
    toneFreq = VERY_CLOSE_TONE;
  } else if (distance < CLOSE_DISTANCE) {
    toneFreq = CLOSE_TONE;
  } else if (distance < MEDIUM_DISTANCE) {
    toneFreq = MEDIUM_TONE;
  } else {
    toneFreq = FAR_TONE;
  }
  
  // Skip audio feedback for objects that are too far away
  if (distance > MAX_DISTANCE / 2) return;
  
  // Play tone with different patterns based on sensor number
  switch(sensorNum) {
    case 1: // Left sensor - one beep
      buzzer.play(toneFreq, TONE_DURATION);
      delay(TONE_DURATION + PAUSE_DURATION);
      break;
      
    case 2: // Center sensor - two beeps
      buzzer.play(toneFreq, TONE_DURATION);
      delay(TONE_DURATION + PAUSE_DURATION);
      buzzer.play(toneFreq, TONE_DURATION);
      delay(TONE_DURATION + PAUSE_DURATION);
      break;
      
    case 3: // Right sensor - three beeps
      buzzer.play(toneFreq, TONE_DURATION);
      delay(TONE_DURATION + PAUSE_DURATION);
      buzzer.play(toneFreq, TONE_DURATION);
      delay(TONE_DURATION + PAUSE_DURATION);
      buzzer.play(toneFreq, TONE_DURATION);
      delay(TONE_DURATION + PAUSE_DURATION);
      break;
  }
}

void checkEmergencyButton() {
  // Read emergency button state (active low due to pull-up)
  bool buttonState = !digitalRead(EMERGENCY_BTN);
  
  // Button press started
  if (buttonState && !emergencyButtonPressed && emergencyPressStart == 0) {
    emergencyPressStart = millis();
  }
  
  // Button released before threshold
  if (!buttonState && emergencyPressStart > 0) {
    emergencyPressStart = 0;
  }
  
  // Long press detected
  if (buttonState && emergencyPressStart > 0 && 
      millis() - emergencyPressStart > EMERGENCY_PRESS_TIME) {
    
    if (!emergencyButtonPressed) {
      emergencyButtonPressed = true;
      if (DEBUG) {
        Serial.println(F("EMERGENCY MODE ACTIVATED"));
      }
    }
    
    // Play SOS pattern
    playSOS();
    
    // Check if button is released to cancel emergency mode
    if (!buttonState) {
      emergencyButtonPressed = false;
      emergencyPressStart = 0;
      if (DEBUG) {
        Serial.println(F("Emergency mode cancelled"));
      }
    }
  }
}

void playSOS() {
  // Standard SOS pattern: 3 short, 3 long, 3 short
  // Short beep
  for (int i = 0; i < 3; i++) {
    buzzer.play(SOS_TONE, 200);
    delay(250);
  }
  
  delay(300);
  
  // Long beep
  for (int i = 0; i < 3; i++) {
    buzzer.play(SOS_TONE, 500);
    delay(550);
  }
  
  delay(300);
  
  // Short beep
  for (int i = 0; i < 3; i++) {
    buzzer.play(SOS_TONE, 200);
    delay(250);
  }
  
  delay(1000);  // Pause before repeating
}

void checkBattery() {
  // Check battery every BATTERY_CHECK_INTERVAL milliseconds
  if (millis() - lastBatteryCheck > BATTERY_CHECK_INTERVAL) {
    lastBatteryCheck = millis();
    
    // Read battery voltage
    float batteryVoltage = readBatteryVoltage();
    
    if (DEBUG) {
      Serial.print(F("Battery voltage: "));
      Serial.print(batteryVoltage);
      Serial.println(F("V"));
    }
    
    // Check if battery is low
    if (batteryVoltage < BATTERY_THRESHOLD && !batteryLow) {
      batteryLow = true;
      if (DEBUG) {
        Serial.println(F("WARNING: Battery low!"));
      }
      // Play low battery warning
      playLowBatteryWarning();
    } else if (batteryVoltage >= BATTERY_THRESHOLD) {
      batteryLow = false;
    }
  }
  
  // If battery is critically low, play warning and go to sleep
  if (batteryLow) {
    playLowBatteryWarning();
    
    // If battery is critically low, go to sleep to save power
    if (readBatteryVoltage() < BATTERY_THRESHOLD - 0.3) {
      if (DEBUG) {
        Serial.println(F("CRITICAL: Battery critically low. Entering power saving mode."));
      }
      // Power down to save battery
      powerDown();
    }
  }
}

float readBatteryVoltage() {
  // Read analog value from battery monitoring pin
  int rawValue = analogRead(BATTERY_PIN);
  
  // Convert to voltage (assuming voltage divider if needed)
  float batteryVoltage = rawValue * (5.0 / 1023.0) * 2; // Adjust multiplier based on your voltage divider
  
  return batteryVoltage;
}

void playLowBatteryWarning() {
  // Two alternating tones for low battery warning
  for (int i = 0; i < 3; i++) {
    buzzer.play(2000, 100);
    delay(150);
    buzzer.play(1000, 100);
    delay(150);
  }
}

void playStartupSound() {
  // Ascending tones to indicate system startup
  for (int freq = 1000; freq <= 2000; freq += 200) {
    buzzer.play(freq, 80);
    delay(100);
  }
}

void powerDown() {
  // Play shutdown sound
  for (int freq = 2000; freq >= 1000; freq -= 200) {
    buzzer.play(freq, 80);
    delay(100);
  }
  
  // Disable all unnecessary peripherals
  power_adc_disable();
  power_spi_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_twi_disable();
  
  // Enter sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  
  if (DEBUG) {
    Serial.println(F("Entering sleep mode to save power"));
    Serial.flush();  // Ensure all debug messages are sent
  }
  
  // Sleep until external interrupt (button press)
  sleep_mode();
  
  // When woken up
  sleep_disable();
  
  // Re-enable necessary peripherals
  power_all_enable();
  
  if (DEBUG) {
    Serial.begin(9600);
    Serial.println(F("Waking up from power saving mode"));
  }
  
  // Play startup sound
  playStartupSound();
}
