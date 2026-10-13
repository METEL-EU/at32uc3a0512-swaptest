/**
 * \file idblock.c   \brief identifikacni blok (FINAL.md kap. 3)
 *
 * Vse se jen cte: obraz flash (CRC32), factory page (Chip SN), FLASHC, PM,
 * SPI flash (JEDEC, UID). Jedina zmena stavu je docasne zapnuti RTC
 * pro mereni fCPU (po mereni se CTRL vrati na puvodni hodnotu).
 */

#include <avr32/io.h>
#include "compiler.h"
#include "debug.h"
#include "user_board.h"
#include "spi_flash.h"
#include "fw_id.h"
#include "idblock.h"

#define TPRINTF(...) dprintf(__VA_ARGS__)

/* CRC32 (polynom 0xEDB88320, reflektovany) po 4 bitech, overeno proti zlib.crc32 */
static const unsigned long crc32_nibble[16] = {
    0x00000000UL, 0x1DB71064UL, 0x3B6E20C8UL, 0x26D930ACUL,
    0x76DC4190UL, 0x6B6B51F4UL, 0x4DB26158UL, 0x5005713CUL,
    0xEDB88320UL, 0xF00F9344UL, 0xD6D6A3E8UL, 0xCB61B38CUL,
    0x9B64C2B0UL, 0x86D3D2D4UL, 0xA00AE278UL, 0xBDBDF21CUL
};

unsigned long crc32_calc(const volatile unsigned char *p, unsigned long len)
{
    unsigned long crc = 0xFFFFFFFFUL;

    while (len--) {
        crc ^= *p++;
        crc = (crc >> 4) ^ crc32_nibble[crc & 0xFUL];
        crc = (crc >> 4) ^ crc32_nibble[crc & 0xFUL];
    }
    return crc ^ 0xFFFFFFFFUL;
}

/* OSCCTRL0.STARTUP -> pocet cyklu RC */
static const unsigned short osc_startup_rc[8] = { 0, 64, 128, 2048, 4096, 8192, 16384, 0 };

/* PM.RCAUSE, bity 0..9 */
static const char *const rcause_bit_names[10] = {
    "POR", "BOD", "EXT", "WDT", "JTAG", "bit5", "SLEEP", "CPUERR", "OCDRST", "JTAGHARD"
};

void rcause_names(unsigned long rcause, char *buf, int size)
{
    int i, n = 0;
    const char *s;

    buf[0] = '\0';
    for (i = 0; i < 32; i++) {
        if (!(rcause & (1UL << i))) {
            continue;
        }
        s = (i < 10) ? rcause_bit_names[i] : "bit";
        if (n != 0 && n < size - 1) {
            buf[n++] = ',';
        }
        while (*s && n < size - 1) {
            buf[n++] = *s++;
        }
        buf[n] = '\0';
    }
    if (n == 0 && size > 4) {
        buf[0] = 'n'; buf[1] = 'o'; buf[2] = 'n'; buf[3] = 'e'; buf[4] = '\0';
    }
}

void id_hex_str(char *out, const unsigned char *in, int len)
{
    static const char hex[] = "0123456789abcdef";
    int i;

    for (i = 0; i < len; i++) {
        out[2 * i] = hex[in[i] >> 4];
        out[2 * i + 1] = hex[in[i] & 0xF];
    }
    out[2 * len] = '\0';
}

void id_collect(id_info_t *id)
{
    const volatile unsigned char *sn = (const volatile unsigned char *)CHIP_SN_ADDRESS;
    int i;

    id->image_crc = crc32_calc((const volatile unsigned char *)IMAGE_START, IMAGE_LEN);
    id->did = (unsigned long)__builtin_mfdr(AVR32_DID);
    for (i = 0; i < CHIP_SN_SIZE; i++) {
        id->chip_sn[i] = sn[i];
    }
    id->board_len = spi_flash_read_ids(id->jedec, id->board_id);

    id->gpf_lo = AVR32_FLASHC.fgpfrlo;
    id->gpf_hi = AVR32_FLASHC.fgpfrhi;
    id->fsr = AVR32_FLASHC.fsr;
    id->fcr = AVR32_FLASHC.fcr;

    id->mcctrl = AVR32_PM.mcctrl;
    id->cksel = AVR32_PM.cksel;
    id->pll0 = AVR32_PM.pll[0];
    id->oscctrl0 = AVR32_PM.oscctrl0;
    id->bod = AVR32_PM.bod;

    // fCPU z RTC na RC oscilatoru (nominalne FRCOSC, RTC = f_rc/2), timeout 2x ocekavane doby
    id->meas_cycles = rtc_measure_cycles(ID_MEAS_TICKS, 2UL * (FCPU / (FRCOSC / 2UL)) * ID_MEAS_TICKS);
    id->fcpu_meas = (unsigned long)(((unsigned long long)id->meas_cycles * (FRCOSC / 2UL)) / ID_MEAS_TICKS);
}

void id_print(const id_info_t *id, const boot_info_t *bi)
{
    char sn[2 * CHIP_SN_SIZE + 1], bid[2 * BOARD_ID_SIZE + 1], rc[80];
    unsigned long lo = id->gpf_lo;
    unsigned long rn = (id->did & AVR32_DID_RN_MASK) >> AVR32_DID_RN_OFFSET;
    unsigned long boden = (lo >> 27) & 0x3UL;
    unsigned long div_int = bi->rtc_rc_cycles / BOOT_RTC_TICKS;
    unsigned long div_frac = ((bi->rtc_rc_cycles % BOOT_RTC_TICKS) * 1000UL) / BOOT_RTC_TICKS;

    id_hex_str(sn, id->chip_sn, CHIP_SN_SIZE);
    id_hex_str(bid, id->board_id, (id->board_len != 0) ? BOARD_ID_SIZE : 0);
    rcause_names(bi->rcause, rc, sizeof(rc));

    TPRINTF("ID fw=%s ver=%s gcc=%s image_start=0x%08lx image_len=%lu image_crc32=0x%08lx\n",
            FW_NAME, FW_VER, __VERSION__, IMAGE_START, IMAGE_LEN, id->image_crc);
    TPRINTF("ID did=0x%08lx rn=%lu(%c) pn=0x%04lx mid=0x%03lx\n",
            id->did, rn, (char)('A' + rn),
            (id->did & AVR32_DID_PN_MASK) >> AVR32_DID_PN_OFFSET,
            (id->did & AVR32_DID_MID_MASK) >> AVR32_DID_MID_OFFSET);
    TPRINTF("ID chip_sn=%s\n", sn);
    TPRINTF("ID board_id=%s jedec=%02x%02x%02x\n", (id->board_len != 0) ? bid : "not_supported",
            (unsigned int)id->jedec[0], (unsigned int)id->jedec[1], (unsigned int)id->jedec[2]);
    // rozpad FGPFRLO podle UC3A_DFP AT32UC3A0512.atdf
    TPRINTF("ID gpf_lo=0x%08lx gpf_hi=0x%08lx lock=0x%04lx epfl=%lu bootprot=%lu bodlevel=0x%02lx bodhyst=%lu boden=%lu(%s)\n",
            lo, id->gpf_hi, lo & 0xFFFFUL, (lo >> 16) & 0x1UL, (lo >> 17) & 0x7UL,
            (lo >> 20) & 0x3FUL, (lo >> 26) & 0x1UL, boden,
            (boden == 1) ? "on_reset" : (boden == 2) ? "on_noreset" : "off");
    TPRINTF("ID fsr=0x%08lx security=%lu fsz=%lu locked=0x%04lx\n",
            id->fsr, (id->fsr & AVR32_FLASHC_FSR_SECURITY_MASK) >> AVR32_FLASHC_FSR_SECURITY_OFFSET,
            (id->fsr & AVR32_FLASHC_FSR_FSZ_MASK) >> AVR32_FLASHC_FSR_FSZ_OFFSET, id->fsr >> 16);
    TPRINTF("ID fcr=0x%08lx fws=%lu bit8=%lu\n",
            id->fcr, (id->fcr & AVR32_FLASHC_FCR_FWS_MASK) >> AVR32_FLASHC_FCR_FWS_OFFSET, (id->fcr >> 8) & 0x1UL);
    TPRINTF("ID clk mcctrl=0x%08lx pll0=0x%08lx cksel=0x%08lx oscctrl0=0x%08lx\n",
            id->mcctrl, id->pll0, id->cksel, id->oscctrl0);
    TPRINTF("ID clk fosc0_nom=%lu fcpu=%lu fhsb=%lu fpba=%lu fpbb=%lu fcpu_meas=%lu\n",
            FOSC0, FCPU, FHSB, FPBA, FPBB, id->fcpu_meas);
    TPRINTF("ID meas cycles=%lu rtc_ticks=%lu f_rtc=f_rc/2 f_rc_nom=%lu rtc_div_check=%lu.%03lu\n",
            id->meas_cycles, ID_MEAS_TICKS, FRCOSC, div_int, div_frac);
    TPRINTF("ID pwr rcause=0x%08lx(%s) bod=0x%08lx\n", bi->rcause, rc, id->bod);
    TPRINTF("ID boot osc0_startup=%urc osc0rdy_wait=%lu lock0_wait=%lu ckrdy_wait=%lu timeouts=%lu\n",
            (unsigned int)osc_startup_rc[(id->oscctrl0 & AVR32_PM_OSCCTRL0_STARTUP_MASK) >> AVR32_PM_OSCCTRL0_STARTUP_OFFSET],
            bi->osc0_wait, bi->lock0_wait, bi->ckrdy_wait, bi->timeouts);
}
