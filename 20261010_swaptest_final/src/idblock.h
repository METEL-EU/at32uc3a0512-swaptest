/**
 * \file idblock.h   \brief identifikacni blok (FINAL.md kap. 3)
 */

#ifndef __idblock_h
#define __idblock_h

#include "boot.h"

/** obraz flash, ze ktereho se za behu pocita CRC32 (= cely .bin) */
#define IMAGE_START       0x80000000UL
#define IMAGE_LEN         0x7FC04UL      /* konec checkX19, overuje se po buildu */

#define CHIP_SN_ADDRESS   0x80800204UL   /* factory page */
#define CHIP_SN_SIZE      15
#define BOARD_ID_SIZE     8

/** tiku RTC (f_rc/2 = 57,6 kHz) pro mereni fCPU: 100 ms */
#define ID_MEAS_TICKS     5760UL

typedef struct {
    unsigned long image_crc;      /**< CRC32 (zlib) IMAGE_START..+IMAGE_LEN, spocteno za behu */
    unsigned long did;
    unsigned char chip_sn[CHIP_SN_SIZE];
    unsigned char jedec[3];
    unsigned char board_id[BOARD_ID_SIZE];
    unsigned char board_len;      /**< 0 = SPI flash nepodporuje UID */
    unsigned long gpf_lo, gpf_hi, fsr, fcr;
    unsigned long mcctrl, cksel, pll0, oscctrl0, bod;
    unsigned long meas_cycles;    /**< cyklu CPU za ID_MEAS_TICKS tiku RTC, 0 = selhalo */
    unsigned long fcpu_meas;      /**< Hz, 0 = mereni selhalo */
} id_info_t;

/** standardni CRC32 (zlib, IEEE 802.3) */
unsigned long crc32_calc(const volatile unsigned char *p, unsigned long len);

/** nacti vse pro ID blok (jen cteni; meri fCPU ~100 ms a CRC obrazu ~0,1 s) */
void id_collect(id_info_t *id);

/** vypis radku "ID ..." (bez radku "ID plan") */
void id_print(const id_info_t *id, const boot_info_t *bi);

/** bajty jako hex retezec (2*len znaku + '\0') */
void id_hex_str(char *out, const unsigned char *in, int len);

/** zapis do buf jmena nastavenych bitu RCAUSE oddelena carkou */
void rcause_names(unsigned long rcause, char *buf, int size);

#endif  // idblock.h
