/**
 * \file spi_flash.h      \brief   SPI flash interface
 */

#ifndef __spi_flash_h
#define __spi_flash_h

#include <stdint.h>

/** CE pin mode: 0 = CE driven manually as GPIO on PA09,
 *               1 = CE driven by SPI0 peripheral NPCS0 on PA10 */
#ifndef SPI_FLASH_CE_NPCS0
  #define SPI_FLASH_CE_NPCS0  0
#endif

#if SPI_FLASH_CE_NPCS0
  #define SPI_FLASH_CE        AVR32_PIN_PA10      // pheripheral NPCS0
#else
  #define SPI_FLASH_CE        AVR32_PIN_PA09      // manually SPI0_CS0
#endif


#define SPIFLASH_IDREAD           0x9F        // read JEDEC manufacturer and device ID
#define SPIFLASH_MACREAD          0x4B        // Winbond: read unique ID number
#define SPIFLASH_SECIDREAD        0x88        // SST26: read security ID (unique ID at 0000h-0007h)

#define SPIFLASH_MID_SST          0xBF        // JEDEC manufacturer ID SST/Microchip
#define SPIFLASH_MID_WINBOND      0xEF        // JEDEC manufacturer ID Winbond

#define SPIFLASH_SST26            0x26        // JEDEC device type SST26

#define SPI_FLASH_SERIAL_NUMBER_SIZE  8       // 64-bit unique ID

typedef struct{
  uint8_t bManufactuerID;
  uint8_t bDeviceType;
  uint8_t bDeviceID;
} jedec_id_s;


#if SPI_FLASH_CE_NPCS0
  /** This procedure set CE = High. */
  #define mSPI_FLASH_CE_High()  {spi_unselectChip(mspi, 0);}
  /** This procedure drives the CE of the device to low. */
  #define mSPI_FLASH_CE_Low()   {spi_selectChip(mspi, 0);}

#else
  /** This procedure set CE = High. */
  #define mSPI_FLASH_CE_High()  {gpio_set_gpio_pin(SPI_FLASH_CE);}
  /** This procedure drives the CE of the device to low. */
  #define mSPI_FLASH_CE_Low()   {gpio_clr_gpio_pin(SPI_FLASH_CE);}

#endif // SPI_FLASH_CE_NPCS0


/** intialize SPI Flash */
void spi_flash_init(void);

/** read serial number from FLASH and print it (call spi_flash_init() first) */
void spi_flash_print_serial_number(void);

/** read JEDEC ID (3 B) and unique ID (8 B), return unique ID length, 0 = not supported */
uint8_t spi_flash_read_ids(uint8_t *jedec, uint8_t *sn);

#endif // __spi_flash_h
