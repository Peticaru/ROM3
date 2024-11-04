// Implementation of Finite State Machine for satellite mission
// Rishav (2022/07/21)

#ifndef _SATELLITE_FSM_H_
#define _SATELLITE_FSM_H_

#include <Arduino.h>
#include "satellite.h"

enum SatelliteState
{
  SLEEP,    // OBC sleep mode
  RESTART,  // Restart OBC
  TX_CW,    // Tx Morse housekeeping data
  TX_GFSK,  // Tx GFSK housekeeping data
  TX_IMAGE, // Transmit image
  LISTEN,   // Listen to uplink commands
  IDLE      // Do nothing at all
};
extern SatelliteState satellite_fsm_last_state;

namespace fsm
{
  // FSM decision and execution functions
  SatelliteState get_state(const uint32_t t, const enum SatellitePowerMode m);
  void execute_state(const enum SatelliteState s);

  void execute_state_sleep(void);
  void execute_state_tx_cw(void);
  void execute_state_tx_image(void);
  void execute_state_tx_gfsk(void);
  void execute_state_listen(void);
  void print_state(SatelliteState s);
}

#endif // satellite_fsm.h
