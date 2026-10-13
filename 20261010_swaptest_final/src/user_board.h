/*
 * user_board.h
 *
 *  Created on: 23.3.2010
 *      Author: CESAK
 */

#ifndef __USER_BOARD_H_
#define __USER_BOARD_H_

//
// Main uP system clock
//
/** initialize osc, system clock 64MHz */
// swaptest_final: krystal 8 MHz (zesileni G3); doba ustaleni OSC0 2048 cyklu RC (~18 ms),
// hodnota z referencni desky Atmel EVK1100 v ASF
#define FOSC0                                   8000000UL
#define OSC0_STARTUP                            AVR32_PM_OSCCTRL0_STARTUP_2048_RCOSC

//#define FOSC1                                   10000000
//#define OSC1_STARTUP                            AVR32_PM_OSCCTRL0_STARTUP_0_RCOSC

#define FRCOSC                                  115200UL

#define FCPU                                    64000000UL
#define FPBA                                    32000000UL
#define FPBB                                    64000000UL
#define FHSB                                    64000000UL

//
// SDRAM Definitions
//
/** Part header file of used SDRAM(s) */
#define SDRAM_PART_HDR  "MT48LC16M16A2TG7E/mt48lc16m16a2tg7e.h"

/** Data bus width to use the SDRAM(s) with (16 or 32 bits; always 16 bits on UC3 */
#define SDRAM_DBW       16


#endif /* __USER_BOARD_H_ */
