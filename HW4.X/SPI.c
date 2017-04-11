#include "SPI.h"

char spi_io(char write){
    //
      SPI1BUF = write;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

void spi_init(){
    //
    //set up pins for SPI1
    //SCK1 fixed to B14 (PIN25)
    //choose SS1 to be B15 (PIN26) 0011
    SS1Rbits.SS1R =0b0011;
    //choose SDI to be B8 (PIN22)  0100
    SDI1Rbits.SDI1R = 0b0100;
    //choose SDO to be A1  (PIN3)  0011
    RPA1Rbits.RPA1R = 0b0011;
    
    //set up chip select bit as an output
    //data is sent when it goes from low to high
    TRISBbits.TRISB15 = 0;
    CS = 1;
    
    // setup spi1
    SPI1CON = 0;              // turn off the spi module and reset it
    SPI1BUF;                  // clear the rx buffer by reading from it
    SPI1BRG = 0x3;            // baud rate to 10 MHz [SPI2BRG = (48000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0;  // clear the overflow bit
    SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1;    // master operation
    SPI1CONbits.ON = 1;       // turn on spi 2

    // Master - SPI1, pins are: SDI1(B8), SDO4(A1), SCK4(B14).  
    // we manually control SS4 as a digital output (B15)
    // since the pic is just starting, we know that spi is off. We rely on defaults here
}