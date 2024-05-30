#include "SD_card.h"

SPIClass spi = SPIClass(HSPI);

SDReader::SDReader(int numFiles) : numFiles(numFiles){
  fileNames = new String[numFiles];
}

void SDReader::init()
{
  pinMode(AUDIO_PW, OUTPUT);
  digitalWrite(AUDIO_PW, LOW);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  spi.begin(SD_SCK, SD_MISO, SD_MOSI);
  if(!SD.begin(SD_CS, spi)){
  Serial.println("Card Mount Failed");
  return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
  Serial.println("No SD card attached");
  return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
  Serial.println("MMC");
  } else if(cardType == CARD_SD){
  Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
  Serial.println("SDHC");
  } else {
  Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT); 
  audio.setVolumeSteps(101); // max 255
}

void  SDReader::set_volume(uint8_t volume)
{
  audio.setVolume(volume);  
}

void  SDReader::loop_audio()
{
  // Handle audio loop
  audio.loop();
}

void SDReader::play_audio(uint8_t fileNo)
{
  digitalWrite(AUDIO_PW, HIGH);
  String fullPath = "/sonneries/";
  fullPath += fileNames[fileNo];
  audio.connecttoFS(SD, fullPath.c_str());
}

void SDReader::stop_audio()
{
  audio.stopSong();
  digitalWrite(AUDIO_PW, LOW);
}

void SDReader::play_bat_alert()
{
  digitalWrite(AUDIO_PW, HIGH);
  audio.connecttoFS(SD, "/alertes/bat_alert");
}
void SDReader::set_alerts(uint8_t index, bool val)
{
    alerts_state[index] = val;
}
bool SDReader::read_alerts(uint8_t index)
{
  return alerts_state[index];
}
void SDReader::play_alert(uint8_t fileNo, uint8_t pack)
{
  digitalWrite(AUDIO_PW, HIGH);
  String fullPath = "/alertes/alert";
  fullPath += fileNo*pack;
  fullPath += ".mp3"; // Assuming the files are named alert0.mp3 and alert1.mp3
  audio.connecttoFS(SD, fullPath.c_str());
}

void SDReader::listFilesInDirectory(fs::FS &fs, const char * dirname) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  int fileCount = 0;
  File file = root.openNextFile();
  while (file && fileCount < numFiles) {
    if (!file.isDirectory()) {
      fileNames[fileCount] = file.name();
      fileCount++;
    }
    file = root.openNextFile();
  }
}

String SDReader::getFileName(int index) const {
  if (index >= 0 && index < numFiles) {
    return fileNames[index];
  }
  return String();
}

void SDReader::writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void SDReader::appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void SDReader::readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\n", path);
  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void SDReader::write_memory(const char* parameter, int value) {
  const char* path = "/data.txt";
  File file = SD.open(path, FILE_READ);
  String content;

  if (file) {
    while (file.available()) {
      content += (char)file.read();
    }
    file.close();
  }

  updateFileContent(content, parameter, value);

  file = SD.open(path, FILE_WRITE);
  if (file) {
    file.print(content);
    file.close();
    Serial.printf("Updated %s to %d in %s\n", parameter, value, path);
  } else {
    Serial.printf("Failed to open %s for writing\n", path);
  }
}

int SDReader::read_memory(const char* parameter) {
  const char* path = "/data.txt";
  File file = SD.open(path, FILE_READ);
  String content;
  int value = -1; // default value if not found

  if (file) {
    while (file.available()) {
      content += (char)file.read();
    }
    file.close();

    String strValue = getParameterValue(content, parameter);
    if (strValue.length() > 0) {
      value = strValue.toInt();
    }
  } else {
    Serial.printf("Failed to open %s for reading\n", path);
  }

  return value;
}

void SDReader::updateFileContent(String& content, const char* parameter, int value) {
  String paramStr = String(parameter) + " :";
  int paramIndex = content.indexOf(paramStr);

  if (paramIndex != -1) {
    int valueStartIndex = paramIndex + paramStr.length();
    int valueEndIndex = content.indexOf('\n', valueStartIndex);
    if (valueEndIndex == -1) {
      valueEndIndex = content.length();
    }
    content.remove(valueStartIndex, valueEndIndex - valueStartIndex);
    content = content.substring(0, valueStartIndex) + String(value) + content.substring(valueStartIndex);
  } else {
    content += paramStr + String(value) + "\n";
  }
}

String SDReader::getParameterValue(const String& content, const char* parameter) {
  String paramStr = String(parameter) + " :";
  int paramIndex = content.indexOf(paramStr);

  if (paramIndex != -1) {
    int valueStartIndex = paramIndex + paramStr.length();
    int valueEndIndex = content.indexOf('\n', valueStartIndex);
    if (valueEndIndex == -1) {
      valueEndIndex = content.length();
    }
    String value = content.substring(valueStartIndex, valueEndIndex);
    value.trim();
    return value;
  }
  return String();
}


void audio_eof_mp3(const char *info){
  digitalWrite(AUDIO_PW, LOW);
}