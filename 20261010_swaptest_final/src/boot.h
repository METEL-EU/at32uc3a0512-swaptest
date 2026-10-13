/**
 * \file boot.h   \brief start hodin RC -> OSC0 -> PLL0 se znackami na UART a timeouty
 */

#ifndef __boot_h
#define __boot_h

/** udaje ze startu, vypisuji se v ID bloku (radek "ID boot") */
typedef struct {
    unsigned long rcause;        /**< PM.RCAUSE pri startu */
    unsigned long osc0_wait;     /**< cekani na OSC0RDY, cyklu CPU na RC */
    unsigned long lock0_wait;    /**< cekani na LOCK0, cyklu CPU na OSC0 */
    unsigned long ckrdy_wait;    /**< cekani na CKRDY, cyklu CPU na OSC0 */
    unsigned long timeouts;      /**< pocet hlaseni timeoutu ve vsech cekacich smyckach */
    unsigned long rtc_rc_cycles; /**< cyklu CPU na RC za BOOT_RTC_TICKS tiku RTC (kontrola delicky RTC) */
} boot_info_t;

/** tiku RTC pro kontrolu delicky RTC na RC (CPU i RTC bezi z RC, ocekava se 2 cykly/tik) */
#define BOOT_RTC_TICKS  1024UL

/** nastaveni hodin (64 MHz) a zaznam prubehu startu; vola se jako prvni v main() */
void boot_clock_init(boot_info_t *bi);

/** cyklu CPU (COUNT) za 'ticks' tiku RTC z RC oscilatoru (PSEL = 0), 0 = mereni selhalo */
unsigned long rtc_measure_cycles(unsigned long ticks, unsigned long timeout_cycles);

#endif  // boot.h
