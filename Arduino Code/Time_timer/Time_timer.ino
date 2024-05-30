// *********************************************************************
// Nom du Projet: Time Timer
// Auteur: Gaëtan Michel
// Date de création: 31.05.2024
// 
// Projet réalisé dans le cadre d'un TPI
// *********************************************************************

#include "ecran.h"
#include "control.h"
#include "SD_card.h"
#include "myRTC.h"
#include "minuteur.h"

//define constants for different screen pages and settings
#define HORLOGE 0
#define RINGTONE 1
#define VOLUME 2
#define AVERTISSEMENT 3
#define PRESETS 4
#define HOME 10
#define MENU 11
#define RUNNING 12
#define TIMER_PAUSED 13
#define RINGING 14
#define LOAD_PRESET 20
#define SAVE_PRESET 21

//declare objects of the MyTimer, MyRTC, and SDReader classes.
MyTimer minuteur;
MyRTC rtc;
SDReader SDcard(NUM_FILES);

//declare global variables
static bool old_orientation = 0, orientation = 0;
uint8_t screen_page = HOME;
uint8_t click = 0;
int volume = 50;
int old_chrono_val = 0;
uint8_t pack_alerte = 1;
uint8_t ringtone_selected = 0;
int item_selected = 0;

//declare prototypes
void battery_update(bool flag);
void setup();
void loop();
void init();
void home();
void menu();
void running();
void ringing();
void paused();
void ringtone();
void avertissements();
void volume_page();

//INITIALISATION

void setup() {
  init();
}

//LOOP

void loop() {
  orientation = get_orientation();
  click = handle_rotary_button();
  switch(screen_page)               //Handle screen pages selection
  {
    case HOME:
    {
      home();
      break;
    }
    case MENU:
    {
      menu();
      break;
    }
    case RUNNING:
    {
      running();
      break;
    }
    case RINGING:
    {
      ringing();
      break;
    }
    case TIMER_PAUSED:
    {
      paused();
      break;
    }
    case RINGTONE:
    {
      ringtone();
      break;
    }
    case VOLUME:
    {
      volume_page();
      break;
    }
    case AVERTISSEMENT:
    {
      avertissements();
      break;
    }
    case PRESETS:
    {
      presets_page();
      break;
    }
    case HORLOGE:
    {
      horloge_page();
      break;
    }
    case SAVE_PRESET:
    {
      save_preset_page();
      break;
    }
    case LOAD_PRESET:
    {
      load_preset_page();
      break;
    }
  }

  old_orientation = orientation;
  battery_update(0);
  SDcard.loop_audio();
}


void init()
{
  Serial.begin(9600);
  SDcard.init();
  SDcard.listFilesInDirectory(SD, "/sonneries");//Read ringtones filenames in SDcard
  for (int i = 0; i < NUM_FILES; i++) {
    update_names(SDcard.getFileName(i).substring(0, SDcard.getFileName(i).length() -4), i);
  }
  rtc.init();

  if(!SDcard.read_memory("IsUpToDate"))// Set time and date to compilation time if it's the first initialisation
  {
    rtc.firstInit();
    rtc.setCompTime();
    SDcard.write_memory("IsUpToDate", 1);
  }
  pinMode(ROTARY_ENCODER_BUTTON_PIN, INPUT);
  encoder_init();
  capt_init();
  screen_init();
  volume = SDcard.read_memory("Volume");//Read memory and set variables value
  ringtone_selected = SDcard.read_memory("Sonnerie");
  SDcard.set_volume(volume);
  for(int x = 0; x<5; x++)//list presets value 
  {
    char name[8];
    sprintf(name, "Preset%d", x);
    int val = SDcard.read_memory(name);
    update_presets_value(x, val);
  }
  old_orientation = get_orientation();
  orientation = old_orientation;
  flip_screen(orientation);
  chrono_display(0, 10, ".");
  battery_update(0);
  }

//****************************************************************************************************//
//                                          HOME SCREEN                                               //
//****************************************************************************************************//
void home()
{
  int value = rotary_loop();
  static int old_value = 0;
  if(value!=old_value)
  {
    if(value>=3600)
    {
      change_encoder_accel(100);
      value-=3600;
      uint8_t minute = value % 60;
      uint8_t hour = 1+((value / 60) % 60);
      chrono_display(hour, minute, ":");
    }
    else
    {
      change_encoder_accel(500);
      uint8_t second = value % 60;
      uint8_t minute = (value / 60) % 60;
      chrono_display(minute, second, ".");
    }
    old_value = value;
  }
  if(click==DOUBLE_CLICK)
  {
    old_chrono_val = value;
    screen_page = MENU;
    change_encoder_setting(0, NUM_ITEMS-1, 1, 0, 0);
    settings_display(0);
  }
  if(orientation!=old_orientation)
  {
    screen_page = RUNNING;
    minuteur.timer_state(RESUME);
    int chrono_value = rotary_loop();
    if(chrono_value > 3600)
    {
      chrono_value = ((chrono_value-3600)*60) + 3600;
    }
    minuteur.set_timer(chrono_value);
    clear_display();
    flip_screen(orientation);
    old_orientation = orientation;
    battery_update(1);
  }
}

//****************************************************************************************************//
//                                          MENU SCREEN                                               //
//****************************************************************************************************//
void menu()
{
  int value = rotary_loop();
  Serial.println(value);
  static int old_value = 0;

  if(value!=old_value)
  {
    item_selected = value;
    settings_display(item_selected);
    old_value=value;
  }
  if(click==SHORT_CLICK)
  {
    switch(item_selected)
    {
      case HORLOGE:
      {
        clear_display();
        screen_page = HORLOGE;
        uint8_t* timeArray = rtc.getTime();
        horloge_display(timeArray[2], timeArray[1], timeArray[0], timeArray[3], timeArray[4], timeArray[5]);
        battery_update(1);
        break;
      }
      case VOLUME:
      {
        screen_page = VOLUME;
        change_encoder_setting(0, 100, 0, 0, volume);
        clear_display();
        volume_display(volume);
        break;
      }
      case RINGTONE:
      {
        screen_page = RINGTONE;
        set_encoder_value(ringtone_selected);
        item_selected = ringtone_selected;
        ringtone_display(item_selected);
        SDcard.play_audio(item_selected);
        break;
      }
      case AVERTISSEMENT:
      {
        screen_page = AVERTISSEMENT;
        set_encoder_value(0);
        item_selected = 0;
        avertissements_display(item_selected,SDcard.read_alerts(0),SDcard.read_alerts(1),SDcard.read_alerts(2),SDcard.read_alerts(3),SDcard.read_alerts(4));
        break;
      }
      case PRESETS:
      {
        change_encoder_setting(0, 1, 0, 0, 0);
        presets_display(0);
        screen_page = PRESETS;
        break;
      }

    }
    
  }
  if(click==LONG_CLICK)
  {
    screen_page = HOME;
    clear_display();
    battery_update(1);
    change_encoder_setting(10, 3780, 0, 500, old_chrono_val);
    if(old_chrono_val>=3600)
    {
      change_encoder_accel(100);
      old_chrono_val-=3600;
      uint8_t minute = old_chrono_val % 60;
      uint8_t hour = 1+((old_chrono_val / 60) % 60);
      chrono_display(hour, minute, ":");
    }
    else
    {
      change_encoder_accel(500);
      uint8_t second = old_chrono_val % 60;
      uint8_t minute = (old_chrono_val / 60) % 60;
      chrono_display(minute, second, ".");
    }
  }
}

//****************************************************************************************************//
//                                          RUNNING SCREEN                                            //
//****************************************************************************************************//
void running()
{
  int val =minuteur.get_timer();
  static bool blink = 0;
  static int old_val = 0;
  if(val!= old_val)
  {
    if(val == 0)
    {
      screen_page = RINGING;
      minuteur.timer_state(PAUSE);
      SDcard.play_audio(ringtone_selected);
      clear_display();
    }
    else
    {
      if(val == 60)
      {
        if(SDcard.read_alerts(0))
          SDcard.play_alert(1, pack_alerte);
      }
      if(val == 120)
      {
        if(SDcard.read_alerts(1))
          SDcard.play_alert(2, pack_alerte);

      }
      if(val == 300)
      {
        if(SDcard.read_alerts(2))
          SDcard.play_alert(3, pack_alerte);

      }
      if(val == 600)
      {
        if(SDcard.read_alerts(3))
          SDcard.play_alert(4, pack_alerte);

      }
      int seconds = val%60;
      int minute = ((val-seconds)/60)%60;
      int hour = (val/3600);
      if (hour>0)
      {
        blink = !blink;
        if(blink)chrono_display(hour, minute, ":");
        else chrono_display(hour, minute, " ");
      }
      else
      {
        chrono_display(minute, seconds, ".");
      }
      old_val = val;
    }
  }
  if(orientation!=old_orientation)
  {
    minuteur.timer_state(PAUSE);
    clear_display();
    flip_screen(orientation);
    screen_page = TIMER_PAUSED;
    battery_update(1);
    old_orientation = orientation;
  }
}

//****************************************************************************************************//
//                                          RINGING SCREEN                                            //
//****************************************************************************************************//
void ringing()
{
  ringin_display();
  if(click==SHORT_CLICK)
  {
    clear_display();
    SDcard.stop_audio();
    screen_page = HOME;
    set_encoder_value(10);
    chrono_display(10, 0, ".");
    battery_update(1);
  }
}

//****************************************************************************************************//
//                                          PAUSE SCREEN                                              //
//****************************************************************************************************//
void paused()
{
  int val =minuteur.get_timer();
  {
    int seconds = val%60;
    int minute = ((val-seconds)/60)%60;
    int hour = (val/3600);
    if (hour>0)
    {
      pause_display(hour, minute, ":");
    }
    else
    {
      pause_display(minute, seconds, ".");
    }
  }
  if(orientation!=old_orientation)
  {
    minuteur.timer_state(RESUME);
    clear_display();
    flip_screen(orientation);
    screen_page = RUNNING;
    old_orientation = orientation;
    battery_update(1);
  }
  if(click==SHORT_CLICK)
  {
    clear_display();
    screen_page = HOME;
    if(val >= 3600)
    {
      val = ((val-3600) / 60)+3600;
      change_encoder_accel(100);
    }
    change_encoder_accel(500);
    set_encoder_value(val);
    battery_update(1);
  }
}

//****************************************************************************************************//
//                                          RINGTONE SCREEN                                           //
//****************************************************************************************************//
void ringtone()
{
  ringtone_gif();
  int value = rotary_loop();
  static int old_value = 0;
  if(value!=old_value)
  {
    item_selected = value;
    ringtone_display(item_selected);
    SDcard.stop_audio();
    SDcard.play_audio(item_selected);
    old_value=value;
  }
  if(click==SHORT_CLICK)
  {
    SDcard.stop_audio();
    set_encoder_value(0);
    ringtone_selected = item_selected;
    SDcard.write_memory("Sonnerie", ringtone_selected);
    ringtone_set();
    set_encoder_value(RINGTONE);
    settings_display(RINGTONE);
    screen_page = MENU;
  }
  if(click==LONG_CLICK)
  {
    SDcard.stop_audio();
    screen_page = MENU;
    set_encoder_value(RINGTONE);
    settings_display(RINGTONE);
  }
}

//****************************************************************************************************//
//                                          ALERTS SCREEN                                             //
//****************************************************************************************************//
void avertissements()
{
  int value = rotary_loop();
  static int old_value = -1;
  static bool alert1 = SDcard.read_memory("Alerte_1");
  static bool alert2 = SDcard.read_memory("Alerte_2");
  static bool alert3 = SDcard.read_memory("Alerte_3");
  static bool alert4 = SDcard.read_memory("Alerte_4");
  static bool bat_alert = SDcard.read_memory("LowBatAlert");
  if(value!=old_value)
  {
    item_selected = value;
    avertissements_display(item_selected,alert1,alert2,alert3, alert4, bat_alert);
    old_value=value;
  }
  if(click==SHORT_CLICK)
  {
    switch(item_selected)
    {
      case 0:
      {
        alert1 = !alert1;
        SDcard.set_alerts(0, alert1);
        SDcard.write_memory("Alerte_1", alert1);
        break;
      }
      case 1:
      {
        alert2 = !alert2;
        SDcard.set_alerts(1, alert2);
        SDcard.write_memory("Alerte_2", alert2);
        break;
      }
      case 2:
      {
        alert3 = !alert3;
        SDcard.set_alerts(2, alert3);
        SDcard.write_memory("Alerte_3", alert3);
        break;
      }
      case 3:
      {
        alert4 = !alert4;
        SDcard.set_alerts(3, alert4);
        SDcard.write_memory("Alerte_4", alert4);
        break;
      }
      case 4:
      {
        bat_alert = !bat_alert;
        SDcard.set_alerts(4, bat_alert);
        SDcard.write_memory("LowBatAlert", bat_alert);
        break;
      }
    }
    avertissements_display(item_selected,alert1,alert2,alert3, alert4, bat_alert);
  }
  if(click==LONG_CLICK)
  {
    screen_page = MENU;
    set_encoder_value(AVERTISSEMENT);
    settings_display(AVERTISSEMENT);
  }


}

//****************************************************************************************************//
//                                          VOLUME SCREEN                                             //
//****************************************************************************************************//
void volume_page()
{
  int value = rotary_loop();
  static int old_value = 0;
  if(value!=old_value)
  {
    volume_display(value);
    old_value=value;
  }
  if(click==LONG_CLICK)
  {
    screen_page = MENU;
    change_encoder_setting(0, NUM_ITEMS-1, 1, 0, VOLUME);
    settings_display(VOLUME);
  }
  if(click==SHORT_CLICK)
  {
    volume = value;
    volume_set(volume);
    SDcard.set_volume(volume);
    SDcard.write_memory("Volume", volume);
    change_encoder_setting(0, NUM_ITEMS-1, 1, 0, VOLUME);
    settings_display(VOLUME);
    screen_page = MENU;
  }
}


//****************************************************************************************************//
//                                          BATTERY UPDATE                                            //
//****************************************************************************************************//
void battery_update(bool flag)
{
  static uint8_t batteryState = 0;
  static uint8_t old_batteryState = 0;
  static long old_millis = 0;
  if((millis()-old_millis) > 1000)
  {
    batteryState = get_battery_state();
    if(batteryState <= 10) //correspond a peut pres a une heure restante (si 250ma de conso)
    {
        if(SDcard.read_alerts(4))
        SDcard.play_bat_alert();
    }
    old_millis=millis();
  }
  if(batteryState != old_batteryState)
  {
    if(screen_page == HOME || screen_page == TIMER_PAUSED || screen_page == RUNNING || screen_page == RINGING ||screen_page == HORLOGE)
    {
      battery_display(batteryState);
    }
    old_batteryState = batteryState;
  }
  if(flag == 1 && (screen_page == HOME || screen_page == TIMER_PAUSED || screen_page == RUNNING || screen_page == RINGING ||screen_page == HORLOGE))
  {
    battery_display(batteryState);
  }
}


//****************************************************************************************************//
//                                          PRESET SCREEN                                             //
//****************************************************************************************************//

void presets_page()
{
  int value = rotary_loop();
  Serial.println(value);
  static int old_value = 0;
  if(value!=old_value)
  {
    item_selected = value;
    presets_display(item_selected);
    old_value=value;
  }
  if(click==SHORT_CLICK)
  {
    switch(item_selected)
    {
      case 0:
      {
        item_selected = 0;
        change_encoder_setting(0, NUM_ITEMS-1, 1, 0, 0);
        screen_page = SAVE_PRESET;
        save_preset_display(0);
        break;
      }
      case 1:
      {
        item_selected = 0;
        change_encoder_setting(0, NUM_ITEMS-1, 1, 0, 0);
        screen_page = LOAD_PRESET;
        save_preset_display(0);
        break;
      }
    }
  }
  if(click==LONG_CLICK)
  {
    screen_page = MENU;
    change_encoder_setting(0, NUM_ITEMS-1, 1, 0, PRESETS);
    item_selected = PRESETS;
    settings_display(PRESETS);
  }
}

//****************************************************************************************************//
//                                          CLOCK SCREEN                                              //
//****************************************************************************************************//
void horloge_page()
{
  if(rtc.getSec()!=rtc.old_sec)
  {
    uint8_t* timeArray = rtc.getTime();
    horloge_display(timeArray[2], timeArray[1], timeArray[0], timeArray[3], timeArray[4], timeArray[5]);
    rtc.old_sec = rtc.getSec();
  }
  if(click==LONG_CLICK)
  {
    screen_page = MENU;
    change_encoder_setting(0, NUM_ITEMS-1, 1, 0, HORLOGE);
    settings_display(HORLOGE);
  }
}

//****************************************************************************************************//
//                                          SAVE SCREEN                                               //
//****************************************************************************************************//

void save_preset_page()
{
  int value = rotary_loop();
  static int old_value = 0;
  if(value!=old_value)
  {
    item_selected = value;
    save_preset_display(item_selected);
    old_value=value;
  }
  if(click==SHORT_CLICK)
  {
    update_presets_value(item_selected, old_chrono_val);
    char name[8];
    sprintf(name, "Preset%d", item_selected);
    SDcard.write_memory(name, old_chrono_val);
    preset_saved_display();
    item_selected = PRESETS;
    change_encoder_setting(0, NUM_ITEMS-1, 1, 0, PRESETS);
    screen_page = MENU;
  }
  if(click==LONG_CLICK)
  {
    screen_page = MENU;
    change_encoder_setting(0, NUM_ITEMS-1, 1, 0, PRESETS);
    settings_display(PRESETS);
  }
}

//****************************************************************************************************//
//                                          LOAD SCREEN                                               //
//****************************************************************************************************//
void load_preset_page()
{
  int value = rotary_loop();
  static int old_value = 0;
  if(value!=old_value)
  {
    item_selected = value;
    save_preset_display(item_selected);
    old_value=value;
  }
  if(click==SHORT_CLICK)
  {
    char name[8];
    sprintf(name, "Preset%d", item_selected);
    old_chrono_val = SDcard.read_memory(name);
    preset_loaded_display();
    clear_display();
    battery_update(1);
    change_encoder_setting(10, 3780, 0, 500, old_chrono_val);
    screen_page = HOME;
  }
  if(click==LONG_CLICK)
  {
    screen_page = MENU;
    change_encoder_setting(0, NUM_ITEMS-1, 1, 0, PRESETS);
    settings_display(PRESETS);
  }
}
