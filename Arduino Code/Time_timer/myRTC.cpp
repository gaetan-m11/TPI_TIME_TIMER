#include "MyRTC.h"

MyRTC::MyRTC(uint8_t address) : _address(address) {}

void MyRTC::init() 
{
    Wire.begin();
}

void MyRTC::firstInit()
{
    Wire.beginTransmission(_address);
    Wire.write(0x00); // Set register pointer to 0
    Wire.write(0x80 | 0x80); // Set ST bit to 1 to start the clock
    Wire.write(0x02); // Set register pointer to control register
    Wire.write(0x00); // Set BAT bit to 1 to enable battery backup
    Wire.write(0x03); // Set register pointer to control register
    Wire.write(0x08); // Set BAT bit to 1 to enable battery backup
    Wire.endTransmission();
}

void MyRTC::setTime(uint8_t hour, uint8_t minute, uint8_t second, uint8_t day, uint8_t month, int year) 
{
    Wire.beginTransmission(_address);
    Wire.write(0); // Set register pointer to 0
    Wire.write(decToBcd(second) | 0x80); // Set CH bit to 1 to enable oscillator
    Wire.write(decToBcd(minute));
    Wire.write(decToBcd(hour) | 0x00); // Set 12/24 hour bit (12-hour mode by default)
    Wire.write(decToBcd(0 |0x08)); // Day of the week (not used here)
    Wire.write(decToBcd(day));
    Wire.write(decToBcd(month));
    Wire.write(decToBcd(year));
    Wire.endTransmission();
}

uint8_t* MyRTC::getTime() 
{
  uint8_t* timeArray = new uint8_t[6]; // Array to hold hour, minute, second, day, month, year
  Wire.beginTransmission(_address);
  Wire.write(0); // Set register pointer to 0
  Wire.endTransmission();

  Wire.requestFrom(_address, 7);
  timeArray[0] = bcdToDec(Wire.read() & 0x7F); // Mask CH bit for second
  timeArray[1] = bcdToDec(Wire.read()); // Minute
  timeArray[2] = bcdToDec(Wire.read() & 0x3F); // Mask 12/24 hour bit for hour
  Wire.read(); // Day of the week (not used here)
  timeArray[3] = bcdToDec(Wire.read()); // Day
  timeArray[4] = bcdToDec(Wire.read() & 0x1F); // Month
  timeArray[5] = bcdToDec(Wire.read()); // Year
  return timeArray;
}

uint8_t MyRTC::getSec() 
{
    Wire.beginTransmission(_address);
    Wire.write(0); // Set register pointer to 0
    Wire.endTransmission();

    Wire.requestFrom(_address, 1);
    return bcdToDec(Wire.read() & 0x7F); // Mask CH bit
}

uint8_t MyRTC::old_sec = 0;

void MyRTC::setCompTime() 
{
    uint8_t hour, minute, second, day, month;
    int year;
    getCompilationTime(hour, minute, second, day, month, year);
    Serial.println("TIME SET TO " + String(hour) + ":" + String(minute) + ":" + String(second) + " " + String(day) + "/" + String(month) + "/" + String(year));
    setTime(hour, minute, second, day, month, year);
}

void MyRTC::getCompilationTime(uint8_t &hour, uint8_t &minute, uint8_t &second, uint8_t &day, uint8_t &month, int &year) 
{
    // Extract time from __TIME__
    sscanf(__TIME__, "%hhu:%hhu:%hhu", &hour, &minute, &second);

    // Extract date from __DATE__
    char monthStr[4];
    sscanf(__DATE__, "%s %hhu %hu", monthStr, &day, &year);
    year %= 100;

    // Convert month string to number
    const char* monthNames = "JanFebMarAprMayJunJulAugSepOctNovDec";
    month = (strstr(monthNames, monthStr) - monthNames) / 3 + 1;
}


int MyRTC::decToBcd(int val) 
{
    return (val / 10 * 16) + (val % 10);
}

int MyRTC::bcdToDec(int val) 
{
    return (val / 16 * 10) + (val % 16);
}