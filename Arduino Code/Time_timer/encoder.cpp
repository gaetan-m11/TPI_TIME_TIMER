#include "control.h"

AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);


void encoder_init()
{
    rotaryEncoder.begin();
    rotaryEncoder.setup(readEncoderISR);

    //set boundaries and if values should cycle or not
    //in this example we will set possible values between 0 and 1000;
    bool circleValues = false;
    rotaryEncoder.setBoundaries(10, 3780, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)

    //rotaryEncoder.disableAcceleration(); //acceleration is now enabled by default - disable if you dont need it
    rotaryEncoder.setAcceleration(500); //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration

}

void set_encoder_value(int value)
{
  rotaryEncoder.setEncoderValue(value);
}



void change_encoder_setting(long minval, long maxval, bool cycle, int accel)
{
  rotaryEncoder.setBoundaries(minval, maxval, cycle); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  rotaryEncoder.setAcceleration(accel);//1 or 0 mean no accel
}

void change_encoder_setting(long minval, long maxval, bool cycle, int accel, long val)
{
  rotaryEncoder.setBoundaries(minval, maxval, cycle); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  rotaryEncoder.setEncoderValue(val);
  rotaryEncoder.setAcceleration(accel);//1 or 0 mean no accel
}

void change_encoder_accel(int accel)
{
  rotaryEncoder.setAcceleration(accel);
}

int rotary_loop()
{
      return rotaryEncoder.readEncoder();
}

void IRAM_ATTR readEncoderISR()
{
    rotaryEncoder.readEncoder_ISR();
}

uint8_t handle_rotary_button() {
  uint8_t click_length = NO_CLICK;
  static unsigned long lastTimeButtonDown = 0;
  static unsigned long lastTimeButtonUp = 0;
  static bool wasButtonDown = false;
  static bool pressedOnce = false;

  bool isEncoderButtonDown = rotaryEncoder.isEncoderButtonDown();
  //isEncoderButtonDown = !isEncoderButtonDown; //uncomment this line if your button is reversed
  if (isEncoderButtonDown) {
    if (!wasButtonDown) {
      //start measuring
      lastTimeButtonDown = millis();
    }
    //else we wait since button is still down
    wasButtonDown = true;
  } else {
    //button is up
    if (wasButtonDown) {
      unsigned long pressDuration = millis() - lastTimeButtonDown;

      //click happened, lets see if it was short click, long click or just too short
      if (pressDuration >= longPressAfterMilliseconds) {
        click_length = LONG_CLICK;
      } else if (pressDuration >= shortPressAfterMilliseconds) {
        if (pressedOnce && (millis() - lastTimeButtonUp) <= doubleClickMaxInterval) {
          click_length = DOUBLE_CLICK;
          pressedOnce = false; // Reset for next double click detection
        } else {
          click_length = SHORT_CLICK;
          pressedOnce = true;
        }
      }
      lastTimeButtonUp = millis();
    }
    wasButtonDown = false;
  }

  // Reset pressedOnce if no second click happens within the doubleClickMaxInterval
  if (pressedOnce && (millis() - lastTimeButtonUp) > doubleClickMaxInterval) {
    pressedOnce = false;
  }

  return click_length;
}