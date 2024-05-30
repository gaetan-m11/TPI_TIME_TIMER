#ifndef SD_card_DEF
#define SD_card_DEF
/*
* Connect the SD card to the following pins:
*
* SD Card | ESP32
* D2 -
* D3 SS
* CMD MOSI
* VSS GND
* VDD 3.3V
* CLK SCK
* VSS GND
* D0 MISO
* D1 -
*/
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <Arduino.h>
#include "Audio.h"

#define I2S_DOUT      32
#define I2S_BCLK      33
#define I2S_LRC       25
#define AUDIO_PW      26

#define SD_CS         27
#define SD_SCK        14
#define SD_MISO       12
#define SD_MOSI       13

#define NUM_FILES     5

class SDReader {
  public:
    SDReader(int numFiles);

    void init();
    void listFilesInDirectory(fs::FS &fs, const char * dirname);
    String getFileName(int index) const;
    void play_audio(uint8_t fileNo);
    void stop_audio();
    void loop_audio();
    void set_volume(uint8_t volume);
    void writeFile(fs::FS &fs, const char * path, const char * message);
    void appendFile(fs::FS &fs, const char * path, const char * message);
    void readFile(fs::FS &fs, const char * path);
    void set_alerts(uint8_t index, bool val);
    bool read_alerts(uint8_t index);
    void play_alert(uint8_t fileNo, uint8_t pack);
    void play_bat_alert();
    void write_memory(const char* parameter, int value);
    int read_memory(const char* parameter);

  private:
    Audio audio;
    String* fileNames;
    int numFiles;
    bool alerts_state[4];
    void updateFileContent(String& content, const char* parameter, int value);
    String getParameterValue(const String& content, const char* parameter);
};


#endif //SD_card_DEF