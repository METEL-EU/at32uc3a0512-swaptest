/**
 * \file spi_flash.c      \brief   SPI flash interface
 */

#include <stdint.h>
#include <gpio.h>
#include <spi.h>

#include "user_board.h"
#include "debug.h"
#include "spi_flash.h"

#ifndef mspi
  #define mspi                &AVR32_SPI0
#endif // mspi

static jedec_id_s sJedecId = {0x00, 0x00, 0x00};


/** initialize SPI0 AT32 interface */
static void spi0_init(void)
{
  static const gpio_map_t QQMAP =
  {
    {AVR32_SPI0_SCK_0_0_PIN ,  AVR32_SPI0_SCK_0_0_FUNCTION },
    {AVR32_SPI0_MISO_0_0_PIN ,  AVR32_SPI0_MISO_0_0_FUNCTION },
    {AVR32_SPI0_MOSI_0_0_PIN ,  AVR32_SPI0_MOSI_0_0_FUNCTION },
    #if SPI_FLASH_CE_NPCS0
      {AVR32_SPI0_NPCS_0_0_PIN,  AVR32_SPI0_NPCS_0_0_FUNCTION },
    #endif // SPI_FLASH_CE_NPCS0

  };

  static const spi_options_t QQOPT =
  {
    .reg      = 0,        // channel NPCS0 pheripheral
    .baudrate = 8000000,  // baudrate /* 3.5.2024 zrychleno 8x */
    .bits     = 8,

    .spck_delay   = 0,
    .trans_delay  = 0,
    #if SPI_FLASH_CE_NPCS0
      .stay_act     = 1,
    #else
      .stay_act     = 0,
    #endif // SPI_FLASH_CE_NPCS0

    .spi_mode     = 3,
    .modfdis      = 1,
  };

   gpio_enable_module(QQMAP, sizeof(QQMAP) / sizeof(QQMAP[0]));

   if (!spi_is_enabled(mspi))
   {
     while (spi_initMaster(mspi, &QQOPT) != SPI_OK) ;
     while (spi_selectionMode(mspi, 0, 0, 0) != SPI_OK) ;
   }

   while (spi_setupChipReg(mspi, &QQOPT, FPBA) != SPI_OK) ;

   spi_enable(mspi);

   spi_selectChip(mspi, 0);
}

/**
 * This procedure outputs a byte shifting out 1-bit per clock rising
 * edge on the the SI pin(LSB 1st).
 */
static void spi_flash_send_byte(unsigned char out)
{
    spi_write(mspi, out);
    while (!spi_writeEndCheck(mspi)) ;
}

/**
 *  This procedure inputs a byte shifting in 1-bit per clock falling
 */
static unsigned char spi_flash_get_byte(void)
{
    unsigned short bb = 0x00;

    spi_write(mspi, bb);
    while (!spi_writeEndCheck(mspi)) ;
    spi_read(mspi, &bb);
    return (bb & 0xFF);
}


/**
 * This procedure Reads the manufacturer's ID, memory type
 * and device. It will use 9Fh as the JEDEC ID command.
 * Please see the product datasheet for details.
 */
static void spi_flash_jedec_id_read(void)
{
   mSPI_FLASH_CE_Low();			                    /* enable device */
   spi_flash_send_byte(SPIFLASH_IDREAD);        /* send JEDEC ID command (9Fh) */
   sJedecId.bManufactuerID = spi_flash_get_byte();
   sJedecId.bDeviceType = spi_flash_get_byte();
   sJedecId.bDeviceID = spi_flash_get_byte();
   mSPI_FLASH_CE_High();			                  /* disable device */
}

/**
 * read factory programmed unique serial number of FLASH
 * SST26: Read Security ID (88h), 2 address bytes, 1 dummy byte, ID at 0000h-0007h
 * Winbond: Read Unique ID (4Bh), 4 dummy bytes, 8 bytes ID
 * @param buf output buffer (SPI_FLASH_SERIAL_NUMBER_SIZE bytes)
 * @return number of bytes read, 0 if not supported for this FLASH
**/
static uint8_t spi_flash_serial_number_read(uint8_t *buf)
{
  uint8_t i;

  if (sJedecId.bManufactuerID == SPIFLASH_MID_SST && sJedecId.bDeviceType == SPIFLASH_SST26) {
    mSPI_FLASH_CE_Low();			                    /* enable device */
    spi_flash_send_byte(SPIFLASH_SECIDREAD);      /* send Read Security ID command (88h) */
    spi_flash_send_byte(0x00);                    /* send 2 address bytes */
    spi_flash_send_byte(0x00);
    spi_flash_send_byte(0x00);                    /* dummy byte */
  } else if (sJedecId.bManufactuerID == SPIFLASH_MID_WINBOND) {
    mSPI_FLASH_CE_Low();			                    /* enable device */
    spi_flash_send_byte(SPIFLASH_MACREAD);        /* send Read Unique ID command (4Bh) */
    for (i = 0; i < 4; i++) {
      spi_flash_send_byte(0x00);                  /* 4 dummy bytes */
    }
  } else {
    return 0;
  }

  for (i = 0; i < SPI_FLASH_SERIAL_NUMBER_SIZE; i++) {
    buf[i] = spi_flash_get_byte();
  }
  mSPI_FLASH_CE_High();			                    /* disable device */

  return SPI_FLASH_SERIAL_NUMBER_SIZE;
}

/**
 *
**/
void spi_flash_init(void)
{
  //! Disable device
  mSPI_FLASH_CE_High();

  //! Initialize SPI0
  spi0_init();
}

/**
 * read JEDEC ID and unique serial number of FLASH (for ID block)
 * @param jedec output: manufacturer ID, device type, device ID (3 bytes)
 * @param sn output buffer (SPI_FLASH_SERIAL_NUMBER_SIZE bytes)
 * @return number of serial number bytes, 0 if not supported for this FLASH
**/
uint8_t spi_flash_read_ids(uint8_t *jedec, uint8_t *sn)
{
  uint8_t len;

  spi_flash_jedec_id_read();
  len = spi_flash_serial_number_read(sn);
  jedec[0] = sJedecId.bManufactuerID;
  jedec[1] = sJedecId.bDeviceType;
  jedec[2] = sJedecId.bDeviceID;
  return len;
}

/**
 * read serial number from FLASH and print it
**/
void spi_flash_print_serial_number(void)
{
  uint8_t sn[SPI_FLASH_SERIAL_NUMBER_SIZE];
  uint8_t jedec[3];
  uint8_t len, i;

  len = spi_flash_read_ids(jedec, sn);

  dprintf("Board ID = ");
  if (len == 0) {
    dprintf("not supported\n");
    return;
  }
  for (i = 0; i < len; i++) {
    dprintf("%02x", sn[i]);
  }
  dprintf("\n");
}
