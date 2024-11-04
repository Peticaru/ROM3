// Satellite configuration file
// 2022/07/13

#ifndef _SATELLITE_CONFIG_H_
#define _SATELLITE_CONFIG_H_

#include "RH_RF24.h"

/* Enable -> 1 */
#define SATELLITE_ENABLE_SERIAL_DEBUG 1 // Must be 0 before launch
#define SATELLITE_RESET_ANTENNA_STATUS 0 // Must be 0 before launch

/* Satellite transmission config */
#define PACKET_DELAY 100
#define SATELLITE_TX_CALLSIGN "AM9ROM"
#define SATELLITE_TX_FREQUENCY 436.235
#define SATELLITE_TX_POWER 0x7f
#define SATELLITE_TX_CRC_POLYNOMIAL RH_RF24::CRC_CCITT
#define SATELLITE_TX_CW_DIT_LEN 60 // 1200 / WPM
#define SATELLITE_TX_CW_DAH_LEN (SATELLITE_TX_CW_DIT_LEN * 3)
#define SATELLITE_TX_GFSK_BAUD_DATA RH_RF24::GFSK_Rb0_5Fd1 // 500bps
#define SATELLITE_TX_GFSK_BAUD_CW RH_RF24::GFSK_Rb0_5Fd1   // 500bps
#define SATELLITE_TX_GFSK_BAUD_IMAGE RH_RF24::GFSK_Rb5Fd10 // 5kbps

/* Timings of tasks (millis) */
#define SATELLITE_TIME_WAIT_AFTER_LAUNCH 5 /*5*/ // Minutes to wait after deployed to space(5 mins)
#define SATELLITE_TIME_IMAGE_TO_GFSK 5000        // Gap between CW and image Tx
#define SATELLITE_TIME_GFSK_TO_CW 5000           // Gap between GFSK and CW Tx
#define SATELLITE_TIME_CW_TO_LISTEN 5000         // Gap between CW and listen mode Tx
#define SATELLITE_TIME_RESET 600000UL            // Reset satellite occasionally (10800000UL)
#define SATELLITE_TIME_ANTENNA_BURN 5000         // Resistor firing for antenna deployment
#define SATELLITE_TIME_ANTENNA_WAIT 1000         // Wait before heating resistor
#define SATELLITE_TIME_WAIT_AFTER_BURN 5 /*30*/  // Minutes to wait after burn (30 mins)
#define SATELLITE_TIME_LISTEN_FOR 15000          // Listen constantly for this time
#define SATELLITE_ANTENNA_BURNS 5               // Number of burns before giving up

// EEPROM offsets
#define SATELLITE_EEPROM_OFFSET_1 0   // First EEPROM offset address
#define SATELLITE_EEPROM_OFFSET_2 170 // Redundant offset address
#define SATELLITE_EEPROM_OFFSET_3 340 // 2nd redundant offset address

/* EEPROM  addresses (0-169) */
/* ! Initialize necessary EEPROM values on the startup ! */
#define SATELLITE_EEPROM_BIRTH_FLAG 0          // Is set to 1 after orbit deloyment
#define SATELLITE_EEPROM_ANT_DEP 1             // 0: Antenna not deployed, 1: Deployed
#define SATELLITE_EEPROM_RESET_COUNT 2         // Number of resets
#define SATELLITE_EEPROM_RESET_FLAG 3          // 1: Intentional reset, 0: Unintentional
#define SATELLITE_EEPROM_TX_IMAGE_FLAG 4       // 1: Tx image, 0: Don't Tx imahe
#define SATELLITE_EEPROM_RESET_DELAY_COUNTER 3 // Number of 3min delays btw two image Tx
#define SATELLITE_EEPROM_TX_IMAGE_SW 6         // 1: Enable image Tx, 0: Disable
#define SATELLITE_EEPROM_TX_GFSK_SW 7          // 1: Enable GFSK data Tx, 0: Disable
#define SATELLITE_EEPROM_TX_CW_SW 8            // 1: Enable Morse Tx, 0: Disable
#define SATELLITE_EEPROM_IMAGE_TOGGLE 9        // 1: Take image, 0: Static image

// 3 mins delays between two image tx for different power modes
#define SATELLITE_3MIN_DELAYS_NORMAL 3   // Normal
#define SATELLITE_3MIN_DELAYS_MEDIUM 6   // Medium
#define SATELLITE_3MIN_DELAYS_CRITICAL 0 // Critical

/* Payload config */
#define SATELLITE_PAYLOAD_I2C_ADD 0x09
#define SATELLITE_PAYLOAD_SSDV_PKTS 20             // 20 Number of SSDV packets expected
#define SATELLITE_PAYLOAD_WAIT_IMAGE_PROCESSING 10 // 10 Secs

/* Voltage threshold for power modes */
#define SATELLITE_POWER_MODE_FATAL (float)2.9
#define SATELLITE_POWER_MODE_CRITICAL (float)3.3
#define SATELLITE_POWER_MODE_MEDIUM (float)3.8
#define SATELLITE_POWER_MODE_NORMAL (float)5.0

/* Pin configurations */
#define SATELLITE_PIN_ANTENNA_DEPLOY A2   // Antenna resistor burn
#define SATELLITE_PIN_ANTENNA_SENSE A1    // Feedback to antenna deployment
#define SATELLITE_PIN_RFM_SDN 9           // RFM module shut down
#define SATELLITE_PIN_RFM_SLAVE_SELECT 10 // RFM module SS
#define SATELLITE_PIN_RFM_INTERRUPT 2     // RFM interrupt
#define SATELLITE_PIN_RFM_MORSE 7         // Pin that controls Morse transmission
#define SATELLITE_PIN_WDT_TOGGLE 8        // Reset watchdog timer
#define SATELLITE_PIN_WDT_SET1 4          // Watchdog timer config

/*
  Override functions
  ==================
  Preamble:
    [] (5B and 5D)

  Commands:
    D: Deploy antenna
    A: Stop transmission
    B: Restart transmission
    M: Stop image Tx
    N: Restart image Tx
    P: Stop GFSK data Tx
    Q: Restart GFSK data Tx
    S: Stop morse Tx
    R: Restart morse Tx
    W: Time between IMG and GFSK Tx
    X: Time between GFSK and CW Tx
    Y: Time between CW Tx and listen

  Acknowledgement
  ===============
    ROM : Receive acknowledge from the satellite
 */

/* Telecommands */
#define SATELLITE_TELECOMMAND_PRE_HI_ROM2 "ROM" // Preamble for ACK msg from ROM2
#define SATELLITE_TELECOMMAND_PRE_OVERRIDE "[]" // Peamble for override
#define SATELLITE_TELECOMMAND_DEP_ANT 'D'       // Deploy antenna
#define SATELLITE_TELECOMMAND_STOP_COMM 'A'     // Stop transmission
#define SATELLITE_TELECOMMAND_START_COMM 'B'    // Restart transmission
#define SATELLITE_TELECOMMAND_STOP_IMG 'M'      // Stop image Tx
#define SATELLITE_TELECOMMAND_START_IMG 'N'     // Restart image Tx
#define SATELLITE_TELECOMMAND_STOP_GFSK 'P'     // Stop GFSK data Tx
#define SATELLITE_TELECOMMAND_START_GFSK 'Q'    // Restart GFSK data Tx
#define SATELLITE_TELECOMMAND_STOP_CW 'S'       // Stop morse Tx
#define SATELLITE_TELECOMMAND_START_CW 'R'      // Restart morse Tx
#define SATELLITE_TELECOMMAND_IMG_GFSK_DT 'W'   // Time between IMG and GFSK Tx
#define SATELLITE_TELECOMMAND_GFSK_CW_DT 'X'    // Time between GFSK and CW Tx
#define SATELLITE_TELECOMMAND_CW_HEAR_DT 'Y'    // Time between CW Tx and listen

#define SATELLITE_TELECOMMAND_HI_ROM2_MSG "Hi Earthlings!" // Hello msg
#define SATELLITE_TELECOMMAND_BUFF_LEN 5                   // Buffer length for commands

#endif // satellite_config.h
