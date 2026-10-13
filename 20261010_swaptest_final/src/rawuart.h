/**
 * \file rawuart.h   \brief jednoduchy vystup na USART0 + USART1 bez printf
 *
 * Pro start (pred prepnutim na PLL0, kdy jeste nebezi __debug) a pro handler
 * vyjimky. Nepouziva .data ani .bss.
 */

#ifndef __rawuart_h
#define __rawuart_h

/** nastav USART0 a USART1 (piny, 8N1, baudrate) pro danou frekvenci PBA, PA03 = 1 */
void rawuart_init(unsigned long fpba_hz, unsigned long baudrate);

/** vysli znak na oba USARTy (stridave, vysilaji soucasne) */
void rawuart_putc(int c);

/** vysli retezec, '\n' se posila jako "\n\r" (stejne jako __debug) */
void rawuart_puts(const char *s);

/** hexadecimalne, presne 'digits' cislic, bez 0x */
void rawuart_puthex(unsigned long value, int digits);

/** dekadicky */
void rawuart_putdec(unsigned long value);

/** pockej, az oba USARTy dovysilaji (pred zmenou hodin nebo nastaveni USARTu) */
void rawuart_flush(void);

#endif  // rawuart.h
