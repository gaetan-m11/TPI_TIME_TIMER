#include "SD_card.h"

// Initialize the SPI class with the HSPI bus
SPIClass spi = SPIClass(HSPI);

// Constructor for the SDReader class
// Initializes the number of files and allocates memory for the filenames
SDReader::SDReader(int numFiles) : numFiles(numFiles) {
  fileNames = new String[numFiles];
}

// Method to initialize the SD card and associated components
void SDReader::init() {
  // Configure the pins for audio power and SD card chip select
  pinMode(AUDIO_PW, OUTPUT);
  digitalWrite(AUDIO_PW, LOW);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  // Initialize the SPI interface
  spi.begin(SD_SCK, SD_MISO, SD_MOSI);
  
  // Initialize the SD card
  if (!SD.begin(SD_CS, spi)) {
    Serial.println("Card Mount Failed");
    return;
  }

  uint8_t cardType = SD.cardType(); // Detect the type of SD card

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  // Print the type of SD card
  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  // Print the size of the SD card
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  // Set the pins for audio and the volume
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT); 
  audio.setVolumeSteps(101); // max volume is 255
}

// Method to set the volume
void SDReader::set_volume(uint8_t volume) {
  audio.setVolume(volume);  
}

// Method to handle the audio loop
void SDReader::loop_audio() {
  audio.loop(); // Manage the audio processing loop
}

// Method to play an audio file
void SDReader::play_audio(uint8_t fileNo) {
  digitalWrite(AUDIO_PW, HIGH); // Turn on audio power
  String fullPath = "/sonneries/";
  fullPath += fileNames[fileNo];
  audio.connecttoFS(SD, fullPath.c_str()); // Connect to the SD filesystem
}

// Method to stop audio playback
void SDReader::stop_audio() {
  audio.stopSong(); // Stop the song
  digitalWrite(AUDIO_PW, LOW); // Turn off audio power
}

// Method to play a battery alert
void SDReader::play_bat_alert() {
  digitalWrite(AUDIO_PW, HIGH); // Turn on audio power
  audio.connecttoFS(SD, "/alertes/bat_alert"); // Connect to the battery alert file on the SD card
}

// Method to set alert states
void SDReader::set_alerts(uint8_t index, bool val) {
  alerts_state[index] = val; // Set the alert state at the given index
}

// Method to read alert states
bool SDReader::read_alerts(uint8_t index) {
  return alerts_state[index]; // Return the alert state at the given index
}

// Method to play an alert
void SDReader::play_alert(uint8_t fileNo, uint8_t pack) {
  digitalWrite(AUDIO_PW, HIGH); // Turn on audio power
  String fullPath = "/alertes/alert";
  fullPath += fileNo * pack;
  fullPath += ".mp3"; // Assuming the files are named alert0.mp3, alert1.mp3, etc.
  audio.connecttoFS(SD, fullPath.c_str()); // Connect to the alert file on the SD card
}

// Method to list files in a directory
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
      fileNames[fileCount] = file.name(); // Store the file name
      fileCount++;
    }
    file = root.openNextFile();
  }
}

// Method to get a file name by index
String SDReader::getFileName(int index) const {
  if (index >= 0 && index < numFiles) {
    return fileNames[index]; // Return the file name at the given index
  }
  return String();
}

// Method to write to a file
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

// Method to append to a file
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

// Method to read from a file
void SDReader::readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\n", path);
  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }
  while (file.available()) {
    Serial.write(file.read()); // Print the file contents
  }
  file.close();
}

// Method to write a parameter and its value to a memory file
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

// Method to read a parameter's value from a memory file
int SDReader::read_memory(const char* parameter) {
  const char* path = "/data.txt";
  File file = SD.open(path, FILE_READ);
  String content;
  int value = -1; // Default value if not found

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

// Method to update the content of a file with a new parameter value
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

// Method to get the value of a parameter from the file content
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

// Callback function for end of MP3 playback
void audio_eof_mp3(const char *info) {
  digitalWrite(AUDIO_PW, LOW); // Turn off audio power
}
