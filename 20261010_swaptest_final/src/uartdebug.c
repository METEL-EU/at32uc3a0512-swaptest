#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>

#include <avr32/io.h>

#include "gpio.h"
#include "uart0.h"

static int uart_debug_initialized = 0;

static void uart_debug_ensure_init(void)
{
    if (!uart_debug_initialized)
    {
        gpio_enable_gpio_pin(AVR32_PIN_PA03);
        uart0_init();
        uart1_init();
        uart_debug_initialized = 1;
    }
}

#define UARTDEBUG_BUFFER_SIZE  256
static char txtdebug[UARTDEBUG_BUFFER_SIZE];

void __debug(const char *format, ...)
{
    va_list args;
    int len;

    va_start(args, format);
    len = vsnprintf(txtdebug, sizeof(txtdebug), format, args);
    va_end(args);

    if (len <= 0) {
        return;
    }
    if ((size_t)len >= sizeof(txtdebug)) {
        len = sizeof(txtdebug) - 1;
    }

    uart_debug_ensure_init();
    gpio_set_gpio_pin(AVR32_PIN_PA03);
    uart0_write(txtdebug, (uint16_t)len);
    uart1_write(txtdebug, (uint16_t)len);

    if (txtdebug[len - 1] == '\n')
    {
        uart0_write("\r", 1);
        uart1_write("\r", 1);
    }
}
