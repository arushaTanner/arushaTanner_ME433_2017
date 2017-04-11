#include<xc.h>           // processor SFR definitions

#ifndef _SPI_H    /* Guard against multiple inclusion */
#define _SPI_H

#define CS LATBbits.LATB15      // chip select pin

char spi_io(char write);
void spi_init();

#endif /* _SPI_H */