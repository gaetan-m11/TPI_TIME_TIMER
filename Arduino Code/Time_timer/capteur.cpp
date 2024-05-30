#include "control.h"

// Function to initialize sensor and battery pins
void capt_init() {
  pinMode(CAPTEUR_PIN, INPUT_PULLUP); // Set the sensor pin as input with internal pull-up resistor
  pinMode(BAT_PIN, INPUT); // Set the battery pin as input
  analogReadResolution(12); // Set the analog read resolution to 12 bits
}

// Function to get the orientation state from the sensor
bool get_orientation() {
  static bool state; // Declare a static boolean variable to hold the state
  state = digitalRead(CAPTEUR_PIN); // Read the state of the sensor pin
  return state; // Return the state
}

// Function to get the battery state as a percentage
int get_battery_state() {
  int adcValue = analogRead(BAT_PIN); // Read the analog value from the battery pin
  // Calculate the voltage from the ADC value
  float voltage = adcValue * (MAX_VOLTAGE - MIN_VOLTAGE) / ADC_MAX + MIN_VOLTAGE;
  // Map the voltage to a battery percentage
  int batteryPercentage = map(int(round(voltage * 10)), 10 * MIN_VOLTAGE, 10 * MAX_VOLTAGE, 0, 100);
  return batteryPercentage; // Return the battery percentage
}
