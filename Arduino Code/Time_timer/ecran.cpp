#include "ecran.h"

// Initialize the display object using the U8G2 library
// Parameters: rotation, clock pin, data pin
U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R0, 19, 4);

// Arrays holding bitmap pointers

const unsigned char* files_bitmap[28] = {
	files_bitmap_a_frm0_40,
	files_bitmap_a_frm1_40,
	files_bitmap_a_frm2_50,
	files_bitmap_a_frm3_40,
	files_bitmap_a_frm4_40,
	files_bitmap_a_frm5_40,
	files_bitmap_a_frm6_40,
	files_bitmap_a_frm7_40,
	files_bitmap_a_frm8_50,
	files_bitmap_a_frm9_40,
	files_bitmap_a_frm10_40,
	files_bitmap_a_frm11_40,
	files_bitmap_a_frm12_40,
	files_bitmap_a_frm13_40,
	files_bitmap_a_frm14_50,
	files_bitmap_a_frm15_40,
	files_bitmap_a_frm16_40,
	files_bitmap_a_frm17_40,
	files_bitmap_a_frm18_40,
	files_bitmap_a_frm19_40,
	files_bitmap_a_frm20_50,
	files_bitmap_a_frm21_40,
	files_bitmap_a_frm22_40,
	files_bitmap_a_frm23_40,
	files_bitmap_a_frm24_40,
	files_bitmap_a_frm25_40,
	files_bitmap_a_frm26_50,
	files_bitmap_a_frm27_50
};
const unsigned char* ringing_logo[28] = {
	epd_bitmap__a_frm0_40,
	epd_bitmap__a_frm1_40,
	epd_bitmap__a_frm2_50,
	epd_bitmap__a_frm3_40,
	epd_bitmap__a_frm4_40,
	epd_bitmap__a_frm5_40,
	epd_bitmap__a_frm6_40,
	epd_bitmap__a_frm7_40,
	epd_bitmap__a_frm8_50,
	epd_bitmap__a_frm9_40,
	epd_bitmap__a_frm10_40,
	epd_bitmap__a_frm11_40,
	epd_bitmap__a_frm12_40,
	epd_bitmap__a_frm13_40,
	epd_bitmap__a_frm14_50,
	epd_bitmap__a_frm15_40,
	epd_bitmap__a_frm16_40,
	epd_bitmap__a_frm17_40,
	epd_bitmap__a_frm18_40,
	epd_bitmap__a_frm19_40,
	epd_bitmap__a_frm20_50,
	epd_bitmap__a_frm21_40,
	epd_bitmap__a_frm22_40,
	epd_bitmap__a_frm23_40,
	epd_bitmap__a_frm24_40,
	epd_bitmap__a_frm25_40,
	epd_bitmap__a_frm26_50,
	epd_bitmap__a_frm27_50
};

char files_name_items [NUM_ITEMS] [MAX_ITEM_LENGTH]; 

int presets_value[5];

char menu_items [NUM_ITEMS] [MAX_ITEM_LENGTH] = {  // array with menu item names
  { "Clock" }, 
  { "Ringtone" }, 
  { "Volume" }, 
  { "Alerts" }, 
  { "Presets" } 
 };

 char alerts_item [NUM_ITEMS] [MAX_ITEM_LENGTH] = {  // array with alerts item names
  { "1 minutes" }, 
  { "2 minutes" }, 
  { "5 minutes" }, 
  { "10 minutes" }, 
  { "Low Bat" } 
 };

 char preset_options [2] [MAX_ITEM_LENGTH] = {  // array with presets item names
  {"Save"},
  {"Load"}
 };

// Array holding bitmap pointers for menu icons
const unsigned char* menu_icons[5] = {
  horloge_bitmap,
  files_bitmap_a_frm7_40,
  volume_bitmap,
  alerts_bitmap,
  bat_logo_bitmap
};

// Array holding bitmap pointers for alert icons
const unsigned char* alerts_icons[10] = {
  alert_logo,
  alert_logo,
  alert_logo,
  alert_logo,
  alert_logo,
  no_alert_logo,
  no_alert_logo,
  no_alert_logo,
  no_alert_logo,
  no_alert_logo
};

// Function to update the names in the files_name_items array
void update_names(String name, int index) {
  // Check if the index is valid
  if (index >= 0 && index < NUM_ITEMS) {
    // Copy the name into the array at the specified index
    name.toCharArray(files_name_items[index], MAX_ITEM_LENGTH);
  }
}


// Function to initialize the screen
void screen_init() {
  // Initialize the display
  u8g2.begin();
  
  // Clear the internal memory of the display
  u8g2.clearDisplay();
}


// Function to clear the display
void clear_display() {
  // Clear the internal memory of the display
  u8g2.clearDisplay();
  
  // Update the display to reflect the cleared memory
  u8g2.updateDisplay();
}


// Function to flip the display orientation
void flip_screen(bool state) {
  // Set the flip mode of the display based on the state parameter
  u8g2.setFlipMode(state);
}


void chrono_display(uint8_t firstNumber, uint8_t secondNumber, String separation)
{
  String firstString;
  String secondString;
  if(firstNumber < 10)firstString = "0" + String(firstNumber);
  else firstString = String(firstNumber);
  if(secondNumber < 10)secondString = "0" + String(secondNumber);
  else secondString = String(secondNumber);
  u8g2.setDrawColor(0);
  u8g2.drawBox(chrono_x, chrono_y, chrono_width, chrono_height);
  u8g2.setDrawColor(1);
  u8g2.setFont(u8g2_font_fub35_tr); // choose a suitable font
  u8g2.setCursor(65, 50);
  u8g2.print(String(firstString)+separation+String(secondString)); // write something to the internal memory
  u8g2.updateDisplay();
}
void pause_display(uint8_t firstNumber, uint8_t secondNumber, String separation)
{
  String firstString;
  String secondString;
  if(firstNumber < 10)firstString = "0" + String(firstNumber);
  else firstString = String(firstNumber);
  if(secondNumber < 10)secondString = "0" + String(secondNumber);
  else secondString = String(secondNumber);
  u8g2.setDrawColor(0);
  u8g2.drawBox(chrono_x, chrono_y, chrono_width, chrono_height);
  u8g2.setDrawColor(1);
  u8g2.drawXBM(17, 17, 32, 32, pause_logo);
  u8g2.setFont(u8g2_font_fub35_tr); // choose a suitable font
  u8g2.setCursor(65, 50);
  u8g2.print(String(firstString)+separation+String(secondString)); // write something to the internal memory
  u8g2.updateDisplay();
}

void battery_display(uint8_t bat_val)
{
  uint8_t x = 228;
  uint8_t y = 0;

  u8g2.setDrawColor(0); 
  u8g2.drawBox(x, y, bat_logo_width, bat_logo_height);
  u8g2.setDrawColor(1); 

  u8g2.drawXBMP(x, y, bat_logo_width, bat_logo_height, bat_bitmap);

  int gauge_width = map(bat_val, 0, 100, 0, 19); 

  u8g2.drawRBox(x + 4, y + 2, gauge_width, 10, 2); 

  u8g2.updateDisplay(); 

}

void settings_display(uint8_t item)
{
  int previous_item;
  int next_item;
    // set correct values for the previous and next items
  previous_item = item - 1;
  if (previous_item < 0) {previous_item = NUM_ITEMS - 1;} // previous item would be below first = make it the last
  next_item = item + 1;  
  if (next_item >= NUM_ITEMS) {next_item = 0;} // next item would be after last = make it the first

  // Clear display and set it up for the next operations
  u8g2.clearDisplay();
  u8g2.setDrawColor(1);
  // draw previous item as icon + label
  u8g2.setFont(u8g2_font_t0_14_mf);
  u8g2.drawStr(4+(80-u8g2.getStrWidth(menu_items[previous_item]))/2, 58, menu_items[previous_item]); 
  u8g2.drawXBM( 20, 0, 48, 48, menu_icons[previous_item]);          

  // draw selected item as icon + label in bold font
  u8g2.setFont(u8g2_font_t0_14_mf);    
  u8g2.drawStr(88+(80-u8g2.getStrWidth(menu_items[item]))/2 , 58, menu_items[item]);   
  u8g2.drawXBM( 104, 0, 48, 48, menu_icons[item]);     

  // draw next item as icon + label
  u8g2.setFont(u8g2_font_t0_14_mf);     
  u8g2.drawStr(172+(80-u8g2.getStrWidth(menu_items[next_item]))/2, 58, menu_items[next_item]);   
  u8g2.drawXBM( 188, 0, 48, 48, menu_icons[next_item]);  

  
  // selected item background
  u8g2.drawRFrame(87, 0, 80, 63, 10);


  u8g2.updateDisplay();
}

void ringtone_gif()
{
  static long old_frame = 0;
  if(millis()-old_frame >= 40)
  {
    static int frame = 0;
    u8g2.setDrawColor(0);
    u8g2.drawBox(103, 1, RINGTONE_GIF_WIDTH, RINGTONE_GIF_HEIGHT);
    u8g2.setDrawColor(1);
    u8g2.drawXBM(103, 1, RINGTONE_GIF_WIDTH, RINGTONE_GIF_HEIGHT, files_bitmap[frame]);
    u8g2.updateDisplay();
    frame = (frame + 1) % RINGTONE_GIF_COUNT;
    old_frame = millis();
  }
}
void ringtone_display(uint8_t item)
{
  int previous_item;
  int next_item;
    // set correct values for the previous and next items
  previous_item = item - 1;
  if (previous_item < 0) {previous_item = NUM_ITEMS - 1;} // previous item would be below first = make it the last
  next_item = item + 1;  
  if (next_item >= NUM_ITEMS) {next_item = 0;} // next item would be after last = make it the first

  // Clear display and set it up for the next operations
  u8g2.clearDisplay();
  u8g2.setDrawColor(1);
  // draw previous item as icon + label
  u8g2.setFont(u8g2_font_t0_14_mf);
  u8g2.drawStr(4+(80-u8g2.getStrWidth(files_name_items[previous_item]))/2, 58, files_name_items[previous_item]); 
  u8g2.drawXBM( 22, 0, 48, 48, files_bitmap_a_frm7_40);          

  // draw selected item as icon + label in bold font
  u8g2.setFont(u8g2_font_t0_14_mf);    
  u8g2.drawStr(88+(80-u8g2.getStrWidth(files_name_items[item]))/2 , 58, files_name_items[item]);   

  // draw next item as icon + label
  u8g2.setFont(u8g2_font_t0_14_mf);     
  u8g2.drawStr(172+(80-u8g2.getStrWidth(files_name_items[next_item]))/2, 58, files_name_items[next_item]);   
  u8g2.drawXBM( 190, 0, 48, 48, files_bitmap_a_frm7_40);  

  
  // selected item background
  u8g2.drawRFrame(87, 0, 80, 63, 10);


  u8g2.updateDisplay();

  // draw scrollbar background
 // u8g2.drawBitmapP(128-8, 0, 8/8, 64, bitmap_scrollbar_background);

  // draw scrollbar handle
 // u8g2.drawBox(125, 64/NUM_ITEMS * item_selected, 3, 64/NUM_ITEMS); 
}

void ringin_display()
{
  static long old_frame = 0;
  if(millis()-old_frame >= 40)
  {
    static int frame = 0;
    u8g2.setDrawColor(0);
    u8g2.drawBox(96, 0, FRAME_WIDTH, FRAME_HEIGHT);
    u8g2.setDrawColor(1);
    u8g2.drawXBM(96, 0, FRAME_WIDTH, FRAME_HEIGHT, ringing_logo[frame]);
    u8g2.updateDisplay();
    frame = (frame + 1) % FRAME_COUNT;
    old_frame = millis();
  }
}

void date_display(String hour, String minute, String second)
{
  u8g2.setDrawColor(0);
  u8g2.drawBox(hour_x, hour_y, hour_width, hour_height);
  u8g2.setDrawColor(1);
  u8g2.setFont(u8g2_font_t0_12_mf); // choose a suitable font
  u8g2.setCursor(0, 12);
  u8g2.print(hour + ":" + minute + ":" + second); // write something to the internal memory
  u8g2.updateDisplay();

}

void volume_display(uint8_t volume)
{
  u8g2.setFont(u8g2_font_ncenB08_tr); // Set font for text
  u8g2.drawStr(0, 10, "Volume Control"); // Draw the title

  // Draw the volume level as a filled rectangle
  u8g2.drawFrame(0, 20, 256, 20); // Draw the outer frame of the gauge
  u8g2.setDrawColor(0); // Set draw color to black
  u8g2.drawBox(2, 22, 252, 16); // Clear the previous gauge
  u8g2.setDrawColor(1); // Set draw color to white
  u8g2.drawBox(2, 22, volume * 2.52, 16); // Draw the new gauge

  // Update the volume percentage
  u8g2.setDrawColor(0); // Set draw color to black
  u8g2.drawBox(108, 40, 40, 12); // Clear the previous percentage area
  u8g2.setDrawColor(1); // Set draw color to white
  char volStr[10];
  sprintf(volStr, "%d%%", volume);
  uint8_t w;
  w = u8g2.getStrWidth(volStr); // Get the text width
  uint8_t centerX = (256 - w) / 2; // Calculate the starting x position to center the text
  u8g2.drawStr(centerX, 50, volStr); // Draw the new volume percentage
  u8g2.updateDisplay();

}
void ringtone_set()
{
  u8g2.clearDisplay();
  u8g2.setDrawColor(1);
  u8g2.setFont(u8g2_font_ncenB14_tr); // Set larger font for confirmation text
  char confirmStr1[20];
  sprintf(confirmStr1, "Ringtone");
  char confirmStr2[10];
  sprintf(confirmStr2, "Changed !");
  uint8_t w1 = u8g2.getStrWidth(confirmStr1); // Get the text width
  uint8_t w2 = u8g2.getStrWidth(confirmStr2); // Get the text width
  uint8_t centerX1 = (256 - w1) / 2; // Calculate the starting x position to center the text
  uint8_t centerX2 = (256 - w2) / 2; // Calculate the starting x position to center the text
  u8g2.drawStr(centerX1, 24, confirmStr1); // Draw the first line centered
  u8g2.drawStr(centerX2, 48, confirmStr2); // Draw the second line centered
  u8g2.updateDisplay();
  delay(2000);
}

void volume_set(uint8_t volume)
{
  u8g2.clearDisplay();
  u8g2.setDrawColor(1);
  u8g2.setFont(u8g2_font_ncenB14_tr); // Set larger font for confirmation text
  char confirmStr1[20];
  sprintf(confirmStr1, "Volume set at");
  char confirmStr2[10];
  sprintf(confirmStr2, "%d%%", volume);
  uint8_t w1 = u8g2.getStrWidth(confirmStr1); // Get the text width
  uint8_t w2 = u8g2.getStrWidth(confirmStr2); // Get the text width
  uint8_t centerX1 = (256 - w1) / 2; // Calculate the starting x position to center the text
  uint8_t centerX2 = (256 - w2) / 2; // Calculate the starting x position to center the text
  u8g2.drawStr(centerX1, 24, confirmStr1); // Draw the first line centered
  u8g2.drawStr(centerX2, 48, confirmStr2); // Draw the second line centered
  u8g2.updateDisplay();
  delay(2000);
}

void avertissements_display(uint8_t item, bool statue_1, bool statue_2, bool statue_3, bool statue_4, bool statue_5)
{
  int previous_item;
  int next_item;
  bool alerts_state[5] = {statue_1, statue_2, statue_3, statue_4, statue_5};
    // set correct values for the previous and next items
  previous_item = item - 1;
  if (previous_item < 0) {previous_item = NUM_ITEMS - 1;} // previous item would be below first = make it the last
  next_item = item + 1;  
  if (next_item >= NUM_ITEMS) {next_item = 0;} // next item would be after last = make it the first

  // Clear display and set it up for the next operations
  u8g2.clearDisplay();
  u8g2.setDrawColor(1);
  // draw previous item as icon + label
  u8g2.setFont(u8g2_font_t0_14_mf);
  u8g2.drawStr(4+(80-u8g2.getStrWidth(alerts_item[previous_item]))/2, 58, alerts_item[previous_item]); 
  if(alerts_state[previous_item] == 1)
    u8g2.drawXBM( 22, 0, 48, 48, alerts_icons[previous_item]);
  else
    u8g2.drawXBM( 22, 0, 48, 48, alerts_icons[previous_item+5]);

  // draw selected item as icon + label in bold font
  u8g2.setFont(u8g2_font_t0_14_mf);    
  u8g2.drawStr(88+(80-u8g2.getStrWidth(alerts_item[item]))/2 , 58, alerts_item[item]);   
  if(alerts_state[item] == 1)
    u8g2.drawXBM( 106, 0, 48, 48, alerts_icons[item]);
  else
    u8g2.drawXBM( 106, 0, 48, 48, alerts_icons[item+5]);

  // draw next item as icon + label
  u8g2.setFont(u8g2_font_t0_14_mf);     
  u8g2.drawStr(172+(80-u8g2.getStrWidth(alerts_item[next_item]))/2, 58, alerts_item[next_item]);   
  if(alerts_state[next_item] == 1)
    u8g2.drawXBM( 190, 0, 48, 48, alerts_icons[next_item]);
  else
    u8g2.drawXBM( 190, 0, 48, 48, alerts_icons[next_item+5]);
  
  // selected item background
  u8g2.drawRFrame(87, 0, 80, 63, 10);


  u8g2.updateDisplay();
}

void horloge_display(uint8_t heure, uint8_t minute, uint8_t seconde, uint8_t jour, uint8_t mois, uint8_t annee)
{
  u8g2.setDrawColor(0);
  u8g2.setFont(u8g2_font_fub35_tr); 
  u8g2.drawBox(0, 0, 226, 50); 
  u8g2.setDrawColor(1);


  char timeStr[9];
  sprintf(timeStr, "%02d:%02d:%02d", heure, minute, seconde);
  uint8_t w1;
  w1 = u8g2.getStrWidth(timeStr); // Get the text width
  uint8_t centerX1 = (256 - w1) / 2; // Calculate the starting x position to center the text
  u8g2.drawStr(centerX1, 40, timeStr);
  
  u8g2.setDrawColor(0);
  u8g2.setFont(u8g2_font_fur11_tr);
  u8g2.drawBox(0, 50, 256, 20);
  u8g2.setDrawColor(1);

  char dateStr[11]; 
  sprintf(dateStr, "%02d/%02d/%04d", jour, mois, 2000 + annee);
  uint8_t w2;
  w2 = u8g2.getStrWidth(dateStr); // Get the text width
  uint8_t centerX2 = (256 - w2) / 2; // Calculate the starting x position to center the text
  u8g2.drawStr(centerX2, 60, dateStr); // Positionner la date
  
  u8g2.updateDisplay();
}

void presets_display(uint8_t item)
{
  int next_item;
  next_item = item + 1;  
  if (next_item >= 2) {next_item = 0;} // next item would be after last = make it the first

  // Clear display and set it up for the next operations
  u8g2.clearDisplay();
  u8g2.setDrawColor(1);

  // draw selected item as icon + label in bold font
  u8g2.setFont(u8g2_font_t0_14_mf);    
  u8g2.drawStr(32+(80-u8g2.getStrWidth(preset_options[0]))/2 , 58, preset_options[0]);   
  u8g2.drawXBM( 48, 0, 48, 48, download_icon);  

  // draw next item as icon + label
  u8g2.setFont(u8g2_font_t0_14_mf);     
  u8g2.drawStr(144+(80-u8g2.getStrWidth(preset_options[1]))/2, 58, preset_options[1]);   
  u8g2.drawXBM( 160, 0, 48, 48, upload_icon);  

  
  // selected item background
  if(item == 0)
    u8g2.drawRFrame(32, 0, 80, 63, 10);
  else
    u8g2.drawRFrame(144, 0, 80, 63, 10);



  u8g2.updateDisplay();
}
void save_preset_display(uint8_t item)
{
  int previous_item;
  int next_item;
    // set correct values for the previous and next items
  previous_item = item - 1;
  if (previous_item < 0) {previous_item = NUM_ITEMS - 1;} // previous item would be below first = make it the last
  next_item = item + 1;  
  if (next_item >= NUM_ITEMS) {next_item = 0;} // next item would be after last = make it the first

  int previous_val = (presets_value[previous_item]);
  int item_val = (presets_value[item]);
  int next_val = (presets_value[next_item]);
  char previous_name[6];
  char item_name[6];
  char next_name[6];

  if(previous_val>=3600)
  {
    previous_val-=3600;
    uint8_t minute = previous_val % 60;
    uint8_t hour = 1+((previous_val / 60) % 60);
    sprintf(previous_name, "%02d:%02d", hour, minute);
  }
  else
  {
    uint8_t second = previous_val % 60;
    uint8_t minute = (previous_val / 60) % 60;
    sprintf(previous_name, "%02d.%02d", minute, second);
  }

  if(item_val>=3600)
  {
    item_val-=3600;
    uint8_t minute = item_val % 60;
    uint8_t hour = 1+((item_val / 60) % 60);
    sprintf(item_name, "%02d:%02d", hour, minute);
  }
  else
  {
    uint8_t second = item_val % 60;
    uint8_t minute = (item_val / 60) % 60;
    sprintf(item_name, "%02d.%02d", minute, second);
  }

  if(next_val>=3600)
  {
    next_val-=3600;
    uint8_t minute = next_val % 60;
    uint8_t hour = 1+((next_val / 60) % 60);
    sprintf(next_name, "%02d:%02d", hour, minute);
  }
  else
  {
    uint8_t second = next_val % 60;
    uint8_t minute = (next_val / 60) % 60;
    sprintf(next_name, "%02d.%02d", minute, second);
  }
  // Clear display and set it up for the next operations
  u8g2.clearDisplay();
  u8g2.setDrawColor(1);
  // draw previous item as icon + label
  u8g2.setFont(u8g2_font_t0_14_mf);
  u8g2.drawStr(4+(80-u8g2.getStrWidth(previous_name))/2, 58, previous_name); 
  u8g2.drawXBM( 22, 0, 48, 48, bat_logo_bitmap);          

  // draw selected item as icon + label in bold font
  u8g2.setFont(u8g2_font_t0_14_mf);    
  u8g2.drawStr(88+(80-u8g2.getStrWidth(item_name))/2 , 58,item_name); 
  u8g2.drawXBM( 106, 0, 48, 48, bat_logo_bitmap);

  // draw next item as icon + label
  u8g2.setFont(u8g2_font_t0_14_mf);     
  u8g2.drawStr(172+(80-u8g2.getStrWidth(next_name))/2, 58, next_name);   
  u8g2.drawXBM( 190, 0, 48, 48, bat_logo_bitmap);  

  
  // selected item background
  u8g2.drawRFrame(87, 0, 80, 63, 10);


  u8g2.updateDisplay();
}

void preset_saved_display()
{
  u8g2.clearDisplay();
  u8g2.setDrawColor(1);
  u8g2.setFont(u8g2_font_ncenB14_tr); // Set larger font for confirmation text
  char confirmStr1[20];
  sprintf(confirmStr1, "Preset");
  char confirmStr2[10];
  sprintf(confirmStr2, "Saved !");
  uint8_t w1 = u8g2.getStrWidth(confirmStr1); // Get the text width
  uint8_t w2 = u8g2.getStrWidth(confirmStr2); // Get the text width
  uint8_t centerX1 = (256 - w1) / 2; // Calculate the starting x position to center the text
  uint8_t centerX2 = (256 - w2) / 2; // Calculate the starting x position to center the text
  u8g2.drawStr(centerX1, 24, confirmStr1); // Draw the first line centered
  u8g2.drawStr(centerX2, 48, confirmStr2); // Draw the second line centered
  u8g2.updateDisplay();
  delay(2000);

}

void preset_loaded_display()
{
  u8g2.clearDisplay();
  u8g2.setDrawColor(1);
  u8g2.setFont(u8g2_font_ncenB14_tr); // Set larger font for confirmation text
  char confirmStr1[20];
  sprintf(confirmStr1, "Preset");
  char confirmStr2[10];
  sprintf(confirmStr2, "Loaded !");
  uint8_t w1 = u8g2.getStrWidth(confirmStr1); // Get the text width
  uint8_t w2 = u8g2.getStrWidth(confirmStr2); // Get the text width
  uint8_t centerX1 = (256 - w1) / 2; // Calculate the starting x position to center the text
  uint8_t centerX2 = (256 - w2) / 2; // Calculate the starting x position to center the text
  u8g2.drawStr(centerX1, 24, confirmStr1); // Draw the first line centered
  u8g2.drawStr(centerX2, 48, confirmStr2); // Draw the second line centered
  u8g2.updateDisplay();
  delay(2000);
}
void update_presets_value(uint8_t item, int value)
{
  presets_value[item] = value;
}