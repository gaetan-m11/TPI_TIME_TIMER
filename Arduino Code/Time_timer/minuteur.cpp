#include "minuteur.h"

void MyTimer::set_timer(int seconds)
{
  seconds_left = seconds;
}

int MyTimer::get_timer()
{
  MyRTC rtc;
  if(state)
  {
    if(rtc.getSec()!=rtc.old_sec)
      {
        rtc.old_sec = rtc.getSec();
        if(seconds_left>0)seconds_left -= 1;
        else seconds_left=0;
      }
  }
  return seconds_left;
}

void MyTimer::timer_state(bool val)
{
  state = val;
}