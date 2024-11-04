// Rishav (2022/09/19)

#ifndef _TELECOMMAND_H_
#define _TELECOMMAND_H_

enum TelecommandFunction
{
  HI_ROM2,     // ACK msg from ROM2
  DEP_ANT,     // Deploy antenna
  STOP_COMM,   // Stop transmission
  START_COMM,  // Restart transmission
  STOP_IMG,    // Stop image Tx
  START_IMG,   // Restart image Tx
  STOP_GFSK,   // Stop GFSK data Tx
  START_GFSK,  // Restart GFSK data Tx
  STOP_CW,     // Stop morse Tx
  START_CW,    // Restart morse Tx
  IMG_GFSK_DT, // Time between IMG and GFSK Tx
  GFSK_CW_DT,  // Time between GFSK and CW Tx
  CW_HEAR_DT,  // Time between CW Tx and listen
  PASS         // Do nothing at all
};

namespace telecommand
{
  void init(void);
  TelecommandFunction listen_to_ground(void);
  void execute_function(enum TelecommandFunction t);
  void print_function(enum TelecommandFunction t);
}

#endif // telecommand.h
