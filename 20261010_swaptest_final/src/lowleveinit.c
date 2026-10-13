/**
 * \file lowlevelinit.c		\brief Low level AVR32 initialization
 *
 * swaptest_final: hodiny se nastavuji az z main() funkci boot_clock_init(),
 * aby se mohl pouzit UART a pamet (_init_startup bezi pred inicializaci
 * .data/.bss v crt0.S). C runtime se tedy inicializuje jeste na RC oscilatoru.
 *
 * Prubeh startu:
 *   1. RC 115 kHz: znacka "BOOT rc" na UART 14400 Bd, kontrola delicky RTC
 *   2. OSC0 krystal 8 MHz (G3), startup 2048 RC (~18 ms), cekani na OSC0RDY
 *   3. prepnuti hlavnich hodin na OSC0, radek "BOOT osc0" na 115200 Bd
 *   4. PLL0 = 8 * OSC0 = 64 MHz (VCO 128 MHz, /2), cekani na LOCK0
 *   5. CKSEL: PBA = main/2, PBB = HSB = CPU = main, cekani na CKRDY
 *   6. FLASHC.FCR.FWS = 1 (jediny zapis do FLASHC)
 *   7. prepnuti hlavnich hodin na PLL0
 * Vsechny cekaci smycky maji timeout: po jeho uplynuti se vypise hlaseni
 * a ceka se dal (hlaseni se opakuje s dvojnasobnym intervalem).
 */

#include "compiler.h"
#include "pm.h"
#include "wdt.h"
#include "intc.h"
#include "flashc.h"
#include "user_board.h"
#include "rawuart.h"
#include "boot.h"

#define BOOT_BAUD_RC     14400UL     // RC 115,2 kHz: OVER = 8, CD = 1 (vyssi rychlost z RC nejde)
#define BOOT_BAUD        115200UL

// timeouty v cyklech CPU (COUNT)
#define TMO_OSC0RDY      16384UL     // na RC: ~142 ms, 8x startup 2048 RC
#define TMO_LOCK0        80000UL     // na OSC0 8 MHz: 10 ms
#define TMO_CKRDY        8000UL      // na OSC0 8 MHz: 1 ms
#define TMO_RTC_RC       20000UL     // na RC: kontrola delicky RTC (ocekava se ~2048)

static unsigned long count_now(void)
{
   return Get_system_register(AVR32_COUNT);
}

/**
 * cekani na priznak v PM.POSCSR s timeoutem a vypisem
 * \return pocet cyklu CPU do nastaveni priznaku
 */
static unsigned long boot_wait(unsigned long mask, unsigned long tmo, const char *name, boot_info_t *bi)
{
   unsigned long t0 = count_now(), last = t0, interval = tmo, reported = 0, waited;

   while (!(AVR32_PM.poscsr & mask))
   {
      if (count_now() - last >= interval)
      {
         bi->timeouts++;
         reported = 1;
         rawuart_puts("BOOT TIMEOUT ");
         rawuart_puts(name);
         rawuart_puts(" waited=");
         rawuart_putdec(count_now() - t0);
         rawuart_puts(" cycles, still waiting\n");
         last = count_now();
         if (interval < 0x40000000UL)
            interval <<= 1;
      }
   }
   waited = count_now() - t0;

   if (reported)
   {
      rawuart_puts("BOOT ");
      rawuart_puts(name);
      rawuart_puts(" ok after ");
      rawuart_putdec(waited);
      rawuart_puts(" cycles\n");
   }
   return waited;
}

static int rtc_idle(void)
{
   unsigned long n = 100000UL;

   while ((AVR32_RTC.ctrl & AVR32_RTC_CTRL_BUSY_MASK) && --n) ;
   return n != 0;
}

unsigned long rtc_measure_cycles(unsigned long ticks, unsigned long timeout_cycles)
{
   volatile avr32_rtc_t *rtc = &AVR32_RTC;
   unsigned long saved = rtc->ctrl & ~AVR32_RTC_CTRL_BUSY_MASK;
   unsigned long v0, c0, result = 0;

   if (!rtc_idle())
      return 0;

   // RTC z RC oscilatoru (CLK32 = 0), PSEL = 0: f_rtc = f_rc / 2
   rtc->ctrl = AVR32_RTC_CTRL_CLKEN_MASK;
   if (!rtc_idle())
      goto restore;
   rtc->top = 0xFFFFFFFFUL;
   if (!rtc_idle())
      goto restore;
   rtc->ctrl = AVR32_RTC_CTRL_CLKEN_MASK | AVR32_RTC_CTRL_EN_MASK;
   if (!rtc_idle())
      goto restore;

   // zacatek mereni na hrane tiku
   c0 = count_now();
   v0 = rtc->val;
   while (rtc->val == v0)
      if (count_now() - c0 > timeout_cycles)
         goto restore;

   c0 = count_now();
   v0 = rtc->val;
   while (rtc->val - v0 < ticks)
      if (count_now() - c0 > timeout_cycles)
         goto restore;
   result = count_now() - c0;

restore:
   rtc_idle();
   rtc->ctrl = saved;
   rtc_idle();
   return result;
}

void boot_clock_init(boot_info_t *bi)
{
   volatile avr32_pm_t *pm = &AVR32_PM;

   bi->rcause = pm->rcause;
   bi->timeouts = 0;

   // 1. znacka na RC oscilatoru, jeste pred zapnutim krystalu a prepnutim na nej
   rawuart_init(FRCOSC, BOOT_BAUD_RC);
   rawuart_puts("\nBOOT rc rcause=0x");
   rawuart_puthex(bi->rcause, 8);
   rawuart_puts("\n");

   // kontrola delicky RTC (pouziva se pro fcpu_meas v ID bloku)
   bi->rtc_rc_cycles = rtc_measure_cycles(BOOT_RTC_TICKS, TMO_RTC_RC);

   // 2. OSC0: krystal 8 MHz, startup 2048 RC (doporuceni ASF), OSC0RDY s timeoutem
   pm_enable_osc0_crystal(pm, FOSC0);
   pm_enable_clk0_no_wait(pm, OSC0_STARTUP);
   bi->osc0_wait = boot_wait(AVR32_PM_POSCSR_OSC0RDY_MASK, TMO_OSC0RDY, "OSC0RDY", bi);

   // 3. prepnuti hlavnich hodin na OSC0 (jako pm_switch_to_osc0 v ASF)
   rawuart_flush();
   pm_switch_to_clock(pm, AVR32_PM_MCSEL_OSC0);
   rawuart_init(FOSC0, BOOT_BAUD);                // PBA = OSC0 (CKSEL je zatim 0)
   rawuart_puts("\nBOOT osc0 osc0rdy_wait=");
   rawuart_putdec(bi->osc0_wait);
   rawuart_puts(" rc_cycles\n");

   // 4. PLL0 na OSC0: VCO 128 MHz, vystup /2 = 64 MHz
   pm_pll_setup(pm,  // volatile avr32_pm_t *pm
                0,   // unsigned int pll
                7,   // unsigned int mul
                0,   // unsigned int div, Sel Osc0/PLL0 or Osc1/Pll1
                0,   // unsigned int osc
                16); // unsigned int lockcount

   // set PLL optimons to run @ 128 Mhz
   pm_pll_set_option(pm, // volatile avr32_pm_t *pm
                     0,  // unsigned int pll
                     1,  // unsigned int  pll_freq
                     1,  // unsigned int  pll_div2
                     0); // unsigned int  pll_wbwdisable

   // Enable PLL0
   pm_pll_enable(pm,0);

   // Wait for PLL0 locked
   bi->lock0_wait = boot_wait(AVR32_PM_POSCSR_LOCK0_MASK, TMO_LOCK0, "LOCK0", bi);

   // 5. delicky jako pm_cksel(pm, 1,0, 0,0, 0,0): PBA = main/2, ostatni main
   //    (pm_cksel by zapsal jen PBADIV = 0x00800000, pak ceka na CKRDY bez timeoutu)
   rawuart_flush();
   pm->cksel = AVR32_PM_CKSEL_PBADIV_MASK;
   rawuart_init(FOSC0 / 2, BOOT_BAUD);            // PBA = OSC0/2 (jen pro pripadne hlaseni)
   bi->ckrdy_wait = boot_wait(AVR32_PM_POSCSR_CKRDY_MASK, TMO_CKRDY, "CKRDY", bi);

   // 6. one wait state at 64 Mhz
   flashc_set_wait_state(1);

   // 7. switch to clock (UART pak nastavi __debug pro FPBA = 32 MHz)
   rawuart_flush();
   pm_switch_to_clock(pm, AVR32_PM_MCCTRL_MCSEL_PLL0);
}

int _init_startup(void)
{
   // disable watchdog
   wdt_disable();

   // disable irq
   Disable_global_interrupt();
   INTC_init_interrupts();

   // hodiny nastavuje az main() -> boot_clock_init()

   // Enable exceptions.
   Enable_global_exception();

   return 1;
}
