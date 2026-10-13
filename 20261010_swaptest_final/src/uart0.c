/**
 * \file uart0.c    \brief UART0 driver
 */

#include <stdint.h>
#include <board.h>
#include <usart.h>
#include <gpio.h>
#include <pdca.h>
#include <tc.h>

#include "uart0.h"

void uart0_init(void)
{
   static const gpio_map_t usmap =
   {
      {AVR32_USART0_TXD_0_0_PIN, AVR32_USART0_TXD_0_0_FUNCTION},
      {AVR32_USART0_RXD_0_0_PIN, AVR32_USART0_RXD_0_0_FUNCTION},
//BB      {AVR32_USART0_RTS_0_0_PIN, AVR32_USART0_RTS_0_0_FUNCTION}
   };

   static usart_options_t usopt =
   {
      .baudrate    = 115200,
      .charlength  = 8,
      .paritytype  = USART_NO_PARITY,
      .stopbits    = USART_1_STOPBIT,
      .channelmode = USART_NORMAL_CHMODE
   };

   avr32_usart_t *us = (avr32_usart_t*)&AVR32_USART0;

   gpio_enable_module(usmap, sizeof(usmap) / sizeof(usmap[0]));
   usart_init_rs232(us, &usopt, FPBA);

// BB
//gpio_set_gpio_pin(AVR32_PIN_PA03);
// BB
}

/** put char to uart0 */
inline void uart0_putchar(uint8_t c)
{
   avr32_usart_t *us = (avr32_usart_t*)&AVR32_USART0;

   while (usart_write_char(us, c) != USART_SUCCESS);
}

/** write buffer to uart0 */
void uart0_write(void *buf, uint16_t count)
{
   char *pbuf = buf;

   while(count)
   {
      uart0_putchar(*pbuf++);
      count--;
   }
}


void uart1_init(void)
{
   static const gpio_map_t usmap =
   {
      {AVR32_USART1_TXD_0_0_PIN, AVR32_USART1_TXD_0_0_FUNCTION},
      {AVR32_USART1_RXD_0_0_PIN, AVR32_USART1_RXD_0_0_FUNCTION},
//BB      {AVR32_USART0_RTS_0_0_PIN, AVR32_USART0_RTS_0_0_FUNCTION}
   };

   static usart_options_t usopt =
   {
      .baudrate    = 115200,
      .charlength  = 8,
      .paritytype  = USART_NO_PARITY,
      .stopbits    = USART_1_STOPBIT,
      .channelmode = USART_NORMAL_CHMODE
   };

   avr32_usart_t *us = (avr32_usart_t*)&AVR32_USART1;

   gpio_enable_module(usmap, sizeof(usmap) / sizeof(usmap[0]));
   usart_init_rs232(us, &usopt, FPBA);

// BB
//gpio_set_gpio_pin(AVR32_PIN_PA03);
// BB
}

/** put char to uart0 */
inline void uart1_putchar(uint8_t c)
{
   avr32_usart_t *us = (avr32_usart_t*)&AVR32_USART1;

   while (usart_write_char(us, c) != USART_SUCCESS);
}

/** write buffer to uart0 */
void uart1_write(void *buf, uint16_t count)
{
   char *pbuf = buf;

   while(count)
   {
      uart1_putchar(*pbuf++);
      count--;
   }
}
