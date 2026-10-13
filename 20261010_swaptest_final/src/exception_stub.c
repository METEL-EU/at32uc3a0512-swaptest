/**
 * \file exception_stub.c   \brief handler vyjimek: vypis priciny a adresy
 *
 * Vola se z isr_exception (exception.S) s novym zasobnikem od _estack.
 * Vypis jde pres rawuart (bez printf, bez .data/.bss). Baudrate se odvodi
 * z aktualnich hodin: RC -> 14400 Bd, OSC0 nebo PLL0 -> 115200 Bd.
 * Hlaseni se opakuje zhruba kazde 2 s (handler se nevraci).
 */

#include <avr32/io.h>
#include "compiler.h"
#include "user_board.h"
#include "rawuart.h"

#define RAM_END  0x00010000UL

/* poradi podle cisel vektoru v exception.S (mov r8, N) */
static const char *const exc_names[] = {
    "?", "UNRECOVERABLE", "TLB_MULTIPLE_HIT", "BUS_ERROR_DATA", "BUS_ERROR_INSTR",
    "NMI", "INSTR_ADDRESS", "ITLB_PROTECTION", "BREAKPOINT", "ILLEGAL_OPCODE",
    "UNIMPLEMENTED_INSTR", "PRIVILEGE_VIOLATION", "FLOATING_POINT", "COPROCESSOR_ABSENT",
    "DATA_ADDRESS_READ", "DATA_ADDRESS_WRITE", "DTLB_PROTECTION_READ", "DTLB_PROTECTION_WRITE",
    "DTLB_MODIFIED", "ITLB_MISS", "DTLB_MISS_READ", "DTLB_MISS_WRITE", "SUPERVISOR_CALL"
};

static void put_kv(const char *key, unsigned long value)
{
    rawuart_puts(key);
    rawuart_puthex(value, 8);
}

/**
 * \param vector  cislo vektoru (1..22, viz exc_names)
 * \param frame   SP pri vstupu do handleru: frame[0] = SR, frame[1] = PC (ulozeno CPU)
 * \param lr      LR v okamziku vyjimky
 */
void exception(int vector, const unsigned long *frame, unsigned long lr)
{
    unsigned long ecr = Get_system_register(AVR32_ECR);
    unsigned long bear = Get_system_register(AVR32_BEAR);
    unsigned long mcsel = AVR32_PM.mcctrl & AVR32_PM_MCCTRL_MCSEL_MASK;
    unsigned long cksel = AVR32_PM.cksel;
    unsigned long fmain, fpba, t0, n = 0;
    unsigned long sp = (unsigned long)frame;
    int frame_ok = (sp % 4 == 0) && (sp <= RAM_END - 8);

    if (mcsel == AVR32_PM_MCCTRL_MCSEL_SLOW)
        fmain = FRCOSC;
    else if (mcsel == AVR32_PM_MCCTRL_MCSEL_OSC0)
        fmain = FOSC0;
    else
        fmain = FCPU;
    fpba = fmain;
    if (cksel & AVR32_PM_CKSEL_PBADIV_MASK)
        fpba = fmain >> (((cksel & AVR32_PM_CKSEL_PBASEL_MASK) >> AVR32_PM_CKSEL_PBASEL_OFFSET) + 1);

    rawuart_flush();
    rawuart_init(fpba, (mcsel == AVR32_PM_MCCTRL_MCSEL_SLOW) ? 14400UL : 115200UL);

    for (;;)
    {
        n++;
        rawuart_puts("\n!!! EXCEPTION !!! #");
        rawuart_putdec(n);
        rawuart_puts(" vec=");
        rawuart_putdec((unsigned long)vector);
        rawuart_puts("(");
        rawuart_puts((vector > 0 && vector < (int)(sizeof(exc_names) / sizeof(exc_names[0]))) ?
                     exc_names[vector] : "?");
        rawuart_puts(")");
        put_kv(" ecr=0x", ecr);
        if (frame_ok)
        {
            put_kv(" pc=0x", frame[1]);
            put_kv(" sr=0x", frame[0]);
        }
        else
        {
            rawuart_puts(" pc=? sr=?");
        }
        put_kv(" lr=0x", lr);
        put_kv(" sp=0x", sp);
        put_kv(" bear=0x", bear);
        rawuart_puts(" mcsel=");
        rawuart_putdec(mcsel);
        rawuart_puts("\n");

        t0 = Get_system_register(AVR32_COUNT);
        while (Get_system_register(AVR32_COUNT) - t0 < 2 * fmain) ;
    }
}
