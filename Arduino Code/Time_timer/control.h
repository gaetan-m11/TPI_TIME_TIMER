#ifndef CONTROL_DEF
#define CONTROL_DEF

#include "AiEsp32RotaryEncoder.h"

#define CAPTEUR_PIN 35

#define BAT_PIN 34

#define ROTARY_ENCODER_A_PIN 17
#define ROTARY_ENCODER_B_PIN 16
#define ROTARY_ENCODER_BUTTON_PIN 36
#define ROTARY_ENCODER_VCC_PIN -1
#define ROTARY_ENCODER_STEPS 4

#define NO_CLICK 0
#define SHORT_CLICK 1
#define LONG_CLICK 2
#define DOUBLE_CLICK 3

// Les valeurs maximales et minimales de tension pour une batterie LiPo 1S
const float MAX_VOLTAGE = 4.3; // Tension maximale de la batterie pleine
const float MIN_VOLTAGE = 3.0;// Tension minimale de la batterie vide
const float ADC_MAX = 4095.0; // Valeur maximale de l'ADC (résolution 12 bits)

// Les valeurs des résistances pour le diviseur de tension
const float R1 = 10000.0; // 10k ohms
const float R2 = 33000.0; // 33k ohms

//paramaters for button
#define shortPressAfterMilliseconds 50  //how long short press shoud be. Do not set too low to avoid bouncing (false press events).
#define longPressAfterMilliseconds 500  //how long čong press shoud be.
#define doubleClickMaxInterval 500

void readEncoderISR();
int rotary_loop();
void encoder_init();
uint8_t handle_rotary_button();
void change_encoder_setting(long minval, long maxval, bool cycle, int accel);
void change_encoder_setting(long minval, long maxval, bool cycle, int accel, long val);
void change_encoder_accel(int accel);
void set_encoder_value(int value);

void capt_init();
bool get_orientation();

int get_battery_state();

#endif //CONTROL_DEF