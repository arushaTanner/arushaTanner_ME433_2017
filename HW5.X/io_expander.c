#include "i2c_master.h"
#include "io_expander.h"

void io_expander_init(void)
{
    unsigned char pin_function = 0b00001111;
    
    i2c_master_start();//send signal to start transfer
    i2c_master_send(SLAVE_ADDR<<1|0);
    i2c_master_send(EXtrisbits);//send address of the tris bits
    i2c_master_send(pin_function);//set pins G0-G3 to be outputs and G4-G7 to be inputs
    i2c_master_stop();
    
    i2c_master_start();     
    i2c_master_send(SLAVE_ADDR << 1 | 0);  
    i2c_master_send(0x6); // pullups        
    i2c_master_send(0b00001111);                       
    i2c_master_stop(); 
}

unsigned char io_expander_get()
{
    unsigned char bit_values;
    
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR<<1|0);
    i2c_master_send(EXportbits);
    i2c_master_restart();
    i2c_master_send((SLAVE_ADDR<<1)|1);
    bit_values =i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    
    return bit_values;
}

void io_expander_set(unsigned char pin,unsigned char level)
{
    //unsigned char bit_values= io_expander_get();
    unsigned char bit_values=level<<pin;
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR<<1|0);
    i2c_master_send(EXportbits);
    i2c_master_send(bit_values);
    i2c_master_stop();
}


