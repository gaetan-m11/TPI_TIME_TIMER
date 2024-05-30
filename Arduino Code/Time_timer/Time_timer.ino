#include "ecran.h"
#include "control.h" 
#include "SD_card.h"
#include "myRTC.h"
#include "minuteur.h"

// Define constants for various screen states and menu items
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

MyTimer minuteur;  // Create a timer object
MyRTC rtc;         // Create an RTC object
SDReader SDcard(NUM_FILES);  // Create an SD card reader object with a fixed number of files

// Define and initialize various global variables
static bool old_orientation = 0, orientation = 0;  // Track screen orientation
static uint8_t dhour, dminute, dsecond, day, month, year;  // Date and time variables
uint8_t screen_page = HOME;  // Current screen page
uint8_t click = 0;  // Click state
int bat_val = 0;  // Battery value
int volume = 50;  // Initial volume
int old_chrono_val = 0;  // Previous chronometer value
uint8_t pack_alerte = 1;  // Alert package

uint8_t ringtone_selected = 0;  // Selected ringtone (saved in EEPROM)
int item_selected = 0;  // Currently selected menu item

// Function prototypes
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
void presets_page();
void horloge_page();
void save_preset_page();
void load_preset_page();

void setup() {
  init();  // Initialize the system
}

void loop() {
  orientation = get_orientation();  // Get current screen orientation
  click = handle_rotary_button();  // Handle rotary button input
  switch (screen_page) {  // Switch case to handle different screen pages
    case HOME:
      home();
      break;
    case MENU:
      menu();
      break;
    case RUNNING:
      running();
      break;
    case RINGING:
      ringing();
      break;
    case TIMER_PAUSED:
      paused();
      break;
    case RINGTONE:
      ringtone();
      break;
    case VOLUME:
      volume_page();
      break;
    case AVERTISSEMENT:
      avertissements();
      break;
    case PRESETS:
      presets_page();
      break;
    case HORLOGE:
      horloge_page();
      break;
    case SAVE_PRESET:
      save_preset_page();
      break;
    case LOAD_PRESET:
      load_preset_page();
      break;
  }

  old_orientation = orientation;  // Update the old orientation value
  battery_update(0);  // Update battery status
  SDcard.loop_audio();  // Loop audio playback
}

void init() {
  Serial.begin(9600);  // Start serial communication at 9600 baud
  SDcard.init();  // Initialize SD card
  SDcard.listFilesInDirectory(SD, "/sonneries");  // List files in the directory
  for (int i = 0; i < NUM_FILES; i++) {
    update_names(SDcard.getFileName(i).substring(0, SDcard.getFileName(i).length() - 4), i);  // Update file names
  }
  rtc.init();  // Initialize RTC
  if (!SDcard.read_memory("IsUpToDate")) {  // Check if RTC is up to date
    rtc.firstInit();  // Perform first-time initialization of RTC
    rtc.setCompTime();  // Set RTC to the compilation time
    SDcard.write_memory("IsUpToDate", 1);  // Mark RTC as up to date in memory
  }
  pinMode(36, INPUT);  // Set pin 36 as input
  encoder_init();  // Initialize encoder
  capt_init();  // Initialize capture
  screen_init();  // Initialize screen
  volume = SDcard.read_memory("Volume");  // Read volume from memory
  ringtone_selected = SDcard.read_memory("Sonnerie");  // Read selected ringtone from memory
  SDcard.set_volume(volume);  // Set volume on SD card
  for (int x = 0; x < 5; x++) {
    char name[8];
    sprintf(name, "Preset%d", x);  // Format preset name
    int val = SDcard.read_memory(name);  // Read preset value from memory
    update_presets_value(x, val);  // Update preset value
  }
  old_orientation = get_orientation();  // Get initial screen orientation
  orientation = old_orientation;
  flip_screen(orientation);  // Adjust screen orientation
  chrono_display(0, 10, ".");  // Display initial chronometer value
  battery_update(0);  // Update battery status
}

void home() {
  int value = rotary_loop();  // Get rotary encoder value
  static int old_value = 0;
  if (value != old_value) {  // Check if value has changed
    if (value >= 3600) {  // Check if value exceeds one hour
      change_encoder_accel(100);  // Change encoder acceleration
      value -= 3600;
      uint8_t minute = value % 60;
      uint8_t hour = 1 + ((value / 60) % 60);
      chrono_display(hour, minute, ":");  // Display hours and minutes
    } else {
      change_encoder_accel(500);  // Change encoder acceleration
      uint8_t second = value % 60;
      uint8_t minute = (value / 60) % 60;
      chrono_display(minute, second, ".");  // Display minutes and seconds
    }
    old_value = value;  // Update old value
  }
  if (click == DOUBLE_CLICK) {  // Check for double click
    old_chrono_val = value;  // Save current value
    screen_page = MENU;  // Switch to menu screen
    change_encoder_setting(0, NUM_ITEMS - 1, 1, 0, 0);  // Update encoder settings
    settings_display(0);  // Display settings
  }
  if (orientation != old_orientation) {  // Check if orientation has changed
    screen_page = RUNNING;  // Switch to running screen
    minuteur.timer_state(RESUME);  // Resume timer
    int chrono_value = rotary_loop();  // Get current rotary value
    if (chrono_value > 3600) {
      chrono_value = ((chrono_value - 3600) * 60) + 3600;  // Adjust value for minutes
    }
    minuteur.set_timer(chrono_value);  // Set timer value
    clear_display();  // Clear screen
    flip_screen(orientation);  // Adjust screen orientation
    old_orientation = orientation;  // Update old orientation
    battery_update(1);  // Update battery status
  }
}

void menu() {
  int value = rotary_loop();  // Get rotary encoder value
  Serial.println(value);  // Print value for debugging
  static int old_value = 0;

  if (value != old_value) {  // Check if value has changed
    item_selected = value;  // Update selected item
    settings_display(item_selected);  // Display settings for selected item
    old_value = value;  // Update old value
  }
  if (click == SHORT_CLICK) {  // Check for short click
    switch (item_selected) {  // Handle different menu items
      case HORLOGE:
        clear_display();  // Clear screen
        screen_page = HORLOGE;  // Switch to clock screen
        uint8_t* timeArray = rtc.getTime();  // Get current time
        horloge_display(timeArray[2], timeArray[1], timeArray[0], timeArray[3], timeArray[4], timeArray[5]);  // Display time
        battery_update(1);  // Update battery status
        break;
      case VOLUME:
        screen_page = VOLUME;  // Switch to volume screen
        change_encoder_setting(0, 100, 0, 0, volume);  // Update encoder settings
        clear_display();  // Clear screen
        volume_display(volume);  // Display volume
        break;
      case RINGTONE:
        screen_page = RINGTONE;  // Switch to ringtone screen
        set_encoder_value(ringtone_selected);  // Set encoder value to selected ringtone
        item_selected = ringtone_selected;  // Update selected item
        ringtone_display(item_selected);  // Display ringtone
        SDcard.play_audio(item_selected);  // Play selected ringtone
        break;
      case AVERTISSEMENT:
        screen_page = AVERTISSEMENT;  // Switch to alerts screen
        set_encoder_value(0);  // Reset encoder value
        item_selected = 0;  // Reset selected item
        avertissements_display(item_selected, SDcard.read_alerts(0), SDcard.read_alerts(1), SDcard.read_alerts(2), SDcard.read_alerts(3), SDcard.read_alerts(4));  // Display alerts
        break;
      case PRESETS:
        change_encoder_setting(0, 1, 0, 0, 0);  // Update encoder settings
        presets_display(0);  // Display presets
        screen_page = PRESETS;  // Switch to presets screen
        break;
    }
  }
  if (click == LONG_CLICK) {  // Check for long click
    screen_page = HOME;  // Switch to home screen
    clear_display();  // Clear screen
    battery_update(1);  // Update battery status
    change_encoder_setting(10, 3780, 0, 500, old_chrono_val);  // Update encoder settings
    if (old_chrono_val >= 3600) {  // Check if value exceeds one hour
      change_encoder_accel(100);  // Change encoder acceleration
      old_chrono_val -= 3600;
      uint8_t minute = old_chrono_val % 60;
      uint8_t hour = 1 + ((old_chrono_val / 60) % 60);
      chrono_display(hour, minute, ":");  // Display hours and minutes
    } else {
      change_encoder_accel(500);  // Change encoder acceleration
      uint8_t second = old_chrono_val % 60;
      uint8_t minute = (old_chrono_val / 60) % 60;
      chrono_display(minute, second, ".");  // Display minutes and seconds
    }
  }
}

void running() {
  int val = minuteur.get_timer();  // Get timer value
  static bool blink = 0;
  static int old_val = 0;

  if (val != old_val) {  // Check if timer value has changed
    if (val == 0) {  // Check if timer has reached zero
      screen_page = RINGING;  // Switch to ringing screen
      minuteur.timer_state(PAUSE);  // Pause timer
      SDcard.play_audio(ringtone_selected);  // Play selected ringtone
      clear_display();  // Clear screen
    } else {
      // Play alerts at specific times
      if (val == 60 && SDcard.read_alerts(0)) SDcard.play_alert(1, pack_alerte);
      if (val == 120 && SDcard.read_alerts(1)) SDcard.play_alert(2, pack_alerte);
      if (val == 300 && SDcard.read_alerts(2)) SDcard.play_alert(3, pack_alerte);
      if (val == 600 && SDcard.read_alerts(3)) SDcard.play_alert(4, pack_alerte);

      int seconds = val % 60;
      int minute = ((val - seconds) / 60) % 60;
      int hour = (val / 3600);

      if (hour > 0) {  // Check if hours are greater than zero
        blink = !blink;  // Toggle blink state
        if (blink) chrono_display(hour, minute, ":");  // Display hours and minutes with colon
        else chrono_display(hour, minute, " ");  // Display hours and minutes with space
      } else {
        chrono_display(minute, seconds, ".");  // Display minutes and seconds
      }
      old_val = val;  // Update old timer value
    }
  }

  if (orientation != old_orientation) {  // Check if orientation has changed
    minuteur.timer_state(PAUSE);  // Pause timer
    clear_display();  // Clear screen
    flip_screen(orientation);  // Adjust screen orientation
    screen_page = TIMER_PAUSED;  // Switch to timer paused screen
    battery_update(1);  // Update battery status
    old_orientation = orientation;  // Update old orientation
  }
}

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

void horloge_page()
{
  if(rtc.getSec()!=rtc.old_sec)
  {
    uint8_t* timeArray = rtc.getTime(); // Récupère l'heure depuis la RTC
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
