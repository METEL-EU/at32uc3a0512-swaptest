/**
 * \file rawuart.c   \brief jednoduchy vystup na USART0 + USART1 bez printf
 *
 * Pouziva se pri startu na RC oscilatoru a OSC0 (pred prvnim __debug)
 * a v handleru vyjimky. Nepouziva .data ani .bss (jen konstanty ve flash
 * a zasobnik). Cekani na TXRDY/TXEMPTY ma timeout, aby se vystup nemohl
 * zaseknout.
 */

#include <avr32/io.h>
#include <gpio.h>
#include <usart.h>

#include "rawuart.h"

/* pruchodu cekaci smyckou: na RC (115 kHz) ~1 s, na 64 MHz ~2 ms, vzdy > 1 znak */
#define RAWUART_TIMEOUT  20000UL

static const gpio_map_t rawuart_pins =
{
   {AVR32_USART0_TXD_0_0_PIN, AVR32_USART0_TXD_0_0_FUNCTION},
   {AVR32_USART0_RXD_0_0_PIN, AVR32_USART0_RXD_0_0_FUNCTION},
   {AVR32_USART1_TXD_0_0_PIN, AVR32_USART1_TXD_0_0_FUNCTION},
   {AVR32_USART1_RXD_0_0_PIN, AVR32_USART1_RXD_0_0_FUNCTION}
};

void rawuart_init(unsigned long fpba_hz, unsigned long baudrate)
{
   usart_options_t opt;

   opt.baudrate    = baudrate;
   opt.charlength  = 8;
   opt.paritytype  = USART_NO_PARITY;
   opt.stopbits    = USART_1_STOPBIT;
   opt.channelmode = USART_NORMAL_CHMODE;

   // PA03 = 1 pred prvnim vystupem (jako uartdebug.c)
   gpio_set_gpio_pin(AVR32_PIN_PA03);

   gpio_enable_module(rawuart_pins, sizeof(rawuart_pins) / sizeof(rawuart_pins[0]));
   usart_init_rs232(&AVR32_USART0, &opt, fpba_hz);
   usart_init_rs232(&AVR32_USART1, &opt, fpba_hz);
}

static void rawuart_wait(volatile avr32_usart_t *us, unsigned long mask)
{
   unsigned long n = RAWUART_TIMEOUT;

   while (!(us->csr & mask) && --n) ;
}

void rawuart_putc(int c)
{
   rawuart_wait(&AVR32_USART0, AVR32_USART_CSR_TXRDY_MASK);
   AVR32_USART0.thr = c & 0xFF;
   rawuart_wait(&AVR32_USART1, AVR32_USART_CSR_TXRDY_MASK);
   AVR32_USART1.thr = c & 0xFF;
}

void rawuart_puts(const char *s)
{
   while (*s)
   {
      rawuart_putc(*s);
      if (*s == '\n')
         rawuart_putc('\r');
      s++;
   }
}

void rawuart_puthex(unsigned long value, int digits)
{
   static const char hex[] = "0123456789abcdef";

   while (digits-- > 0)
      rawuart_putc(hex[(value >> (4 * digits)) & 0xF]);
}

void rawuart_putdec(unsigned long value)
{
   char buf[10];
   int n = 0;

   do
   {
      buf[n++] = (char)('0' + value % 10);
      value /= 10;
   } while (value && n < (int)sizeof(buf));

   while (n > 0)
      rawuart_putc(buf[--n]);
}

void rawuart_flush(void)
{
   rawuart_wait(&AVR32_USART0, AVR32_USART_CSR_TXEMPTY_MASK);
   rawuart_wait(&AVR32_USART1, AVR32_USART_CSR_TXEMPTY_MASK);
}
