
#include "reset.h"
#include "antenna.h"
#include "satellite.h"
#include "satellite_fsm.h"
#include "RF4463.h"

extern uint32_t timekeeper;
RF4463 rf4463;

void setup()
{
#if SATELLITE_ENABLE_SERIAL_DEBUG == 1
  Serial.begin(115200);
  Serial.println("\n*ROM-2*\n");
#endif

  satellite::handle_reset();
  satellite::init();
  antenna::deploy();
  timekeeper = millis();
}

void loop()
{
  // Detect power mode, decide satellite state and execute it
  SatellitePowerMode power_mode = satellite::get_power_mode();
  SatelliteState state = fsm::get_state(millis(), power_mode);
  fsm::execute_state(state);
}
