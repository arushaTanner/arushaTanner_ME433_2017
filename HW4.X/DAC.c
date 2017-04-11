#include "DAC.h"
#include "SPI.h"


void setVoltage(unsigned char channel, unsigned char voltage){
    //
    unsigned char output1 = channel<<7;
    output1=output1+0b1110000;
    output1=output1+(voltage>>4);
    unsigned char output2 = voltage<<4;
    
    CS=0;
    spi_io(output1);
    spi_io(output2);
    CS=1;
}