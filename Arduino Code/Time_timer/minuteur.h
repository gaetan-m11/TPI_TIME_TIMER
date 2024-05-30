#ifndef MINUTEUR_DEF
#define MINUTEUR_DEF

#include <Arduino.h>
#include "myRTC.h"

#define RESUME 1
#define PAUSE 0

class MyTimer
{
  public:
    void set_timer(int seconds);
    int get_timer();
    void timer_state(bool val);
  private:
    int seconds_left;
    bool state;
};

#endif