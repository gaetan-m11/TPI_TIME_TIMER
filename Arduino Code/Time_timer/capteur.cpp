#include "tatata.h"

void capt_init()
{
  pinMode(CAPTEUR_PIN, INPUT_PULLUP);
  pinMode(BAT_PIN, INPUT);
  analogReadResolution(12);
}

bool get_orientation()
{
  static bool state;
  state = digitalRead(CAPTEUR_PIN);
  return state;
}

int get_battery_state() {
    int adcValue = analogRead(BAT_PIN);
    float voltage = adcValue * (MAX_VOLTAGE - MIN_VOLTAGE) / ADC_MAX + MIN_VOLTAGE;
    int batteryPercentage = map(int(round(voltage*10)), 10*MIN_VOLTAGE, 10*MAX_VOLTAGE, 0, 100);
    return batteryPercentage;
}
