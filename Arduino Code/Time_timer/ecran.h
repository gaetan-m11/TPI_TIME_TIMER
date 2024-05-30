#ifndef ECRAN_DEF
#define ECRAN_DEF

#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include "bitmap.h"

#define chrono_x 32
#define chrono_y 14
#define chrono_width 164
#define chrono_height 38

#define bat_val_x 224
#define bat_val_y 0
#define bat_val_width 32
#define bat_val_height 10

#define hour_x 0
#define hour_y 0
#define hour_width 64
#define hour_height 12

#define NUM_ITEMS 5
#define MAX_ITEM_LENGTH 12 // maximum characters for the item name

void battery_display(uint8_t bat_val);
void chrono_display(uint8_t firstNumber, uint8_t SecondNumber, String separation);
void pause_display(uint8_t firstNumber, uint8_t SecondNumber, String separation);
void screen_init();
void update_names(String name, int index);
void settings_display(uint8_t item);
void ringtone_display(uint8_t item);
void  ringtone_gif();
void clear_display();
void ringin_display();
void date_display(String hour, String minute, String second);
void volume_display(uint8_t volume);
void flip_screen(bool state);
void ringtone_set();
void volume_set(uint8_t volume);
void horloge_display(uint8_t heure, uint8_t minute, uint8_t seconde, uint8_t jour, uint8_t mois, uint8_t annee);
void presets_display(uint8_t item);
void save_preset_display(uint8_t item);
void load_preset_display(uint8_t item);
void preset_saved_display();
void preset_loaded_display();
void update_presets_value(uint8_t item, int value);

void avertissements_display(uint8_t item, bool statue_1, bool statue_2, bool statue_3, bool statue_4, bool statue_5);
#endif //ECRAN_DEF