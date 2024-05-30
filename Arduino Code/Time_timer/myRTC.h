#ifndef myRTC_DEF
#define myRTC_DEF

#include <Wire.h>
#include <Arduino.h>

#define RTC_ADD 0x6F

class MyRTC {
public:
    MyRTC(uint8_t address = RTC_ADD);
    void init();
    void firstInit();
    void setTime(uint8_t hour, uint8_t minute, uint8_t second, uint8_t day, uint8_t month, int year);
    uint8_t* getTime();
    void setCompTime();
    uint8_t getSec();
    static uint8_t old_sec;

private:
    int decToBcd(int val);
    int bcdToDec(int val);
    uint8_t _address;
    void getCompilationTime(uint8_t &hour, uint8_t &minute, uint8_t &second, uint8_t &day, uint8_t &month, int &year);
};


#endif