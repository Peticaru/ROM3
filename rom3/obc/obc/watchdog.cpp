// 2022-09-03

#include "watchdog.h"
#include "satellite_config.h"

static uint8_t watchdog_toggle_flag = 0;

void watchdog::init(void)
{
  pinMode(SATELLITE_PIN_WDT_SET1, OUTPUT);
  pinMode(SATELLITE_PIN_WDT_TOGGLE, OUTPUT);
}

void watchdog::toggle(void)
{
  watchdog_toggle_flag = !watchdog_toggle_flag;
  digitalWrite(SATELLITE_PIN_WDT_TOGGLE, watchdog_toggle_flag);
}

void watchdog::set_time_period(const WatchdogTimePeriod t)
{
  switch (t)
  {
  case _1_SECOND:
  {
    digitalWrite(SATELLITE_PIN_WDT_SET1, LOW);
    break;
  }

  case _3_MINUTES:
  {
    digitalWrite(SATELLITE_PIN_WDT_SET1, HIGH);
    break;
  }
  }
}
