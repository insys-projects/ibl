#ifndef _SPI_API_H
#define _SPI_API_H
/*****************************************************************************************************
 * FILE PURPOSE: Define the boot SPI driver
 *****************************************************************************************************
 * DESCRIPTION: The SPI API is defined
 *
 *****************************************************************************************************/
 
typedef struct spiConfig_s  {

  UINT16 port;
  UINT16 mode;
  UINT16 addrWidth;
  UINT16 npin;
  UINT16 csel;
  UINT16 clkdiv;
  UINT16 c2tdelay;
  
} spiConfig_t;

SINT16 hwSpiConfig (spiConfig_t *spiCfg);
SINT16 hwSpiRead (UINT32 addr, UINT32 sizeBytes, UINT8 *data);
void hwSpiEnableXfer (UINT32 port);
void hwSpiDisableXfer (UINT32 port);

/* Return values */
#define SPI_INVALID_ADDR_WIDTH  -1
#define SPI_INVALID_NPIN        -2
#define SPI_TIMEOUT             -3
#define SPI_NOT_ENOUGH_BYTES    -4





#endif /* _SPI_API_H */




