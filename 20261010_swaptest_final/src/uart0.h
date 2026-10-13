/**
 * \file uart0.h     \brief UART0 driver
 */

#ifndef __uart0_h
#define __uart0_h

#include <inttypes.h>

/** initialize uart0 (vzdy na 115200 Bd / 8 bitu, viz uart0.c) */
void uart0_init(void);
void uart1_init(void);

/** write buffer to uart0 */
void uart0_write(void *buf, uint16_t count);
void uart1_write(void *buf, uint16_t count);

#endif  // uart0.h
