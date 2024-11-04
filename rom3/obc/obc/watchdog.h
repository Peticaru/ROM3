// 2022-09-03

#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

enum WatchdogTimePeriod
{
  _1_SECOND,
  _3_MINUTES
};

namespace watchdog
{
  void init(void);
  void toggle(void);
  void set_time_period(const WatchdogTimePeriod t);
}

#endif // watchdog.h
