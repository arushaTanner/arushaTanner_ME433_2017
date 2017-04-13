#include "i2c_master.h"
#include "io_expander.h"

void io_expander_init(void)
{
    unsigned char pin_function = 0b00000000;
    
    i2c_master_start();//send signal to start transfer
    i2c_master_send(expander_addr<<1);
    i2c_master_send(EXtrisbits);//send address of the tris bits
    i2c_master_send(pin_function);//set pins G0-G3 to be outputs and G4-G7 to be inputs
    i2c_master_stop();
}

char io_expander_get(void)
{
    unsigned char bit_values;
    
    i2c_master_start();
    i2c_master_send(expander_addr<<1);
    i2c_master_send(EXportbits);
    i2c_master_restart();
    i2c_master_send(expander_addr<<1|1);
    bit_values =i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    return bit_values;
}

void io_expander_set(unsigned char pin,unsigned char level)
{
    unsigned char bit_values= io_expander_get();
    bit_values=bit_values|(level<<pin);
    i2c_master_start();
    i2c_master_send(expander_addr<<1);
    i2c_master_send(EXportbits);
    i2c_master_send(bit_values);
    i2c_master_stop();
}

void io_expander_sset(unsigned char pin,unsigned char level)
{
    unsigned char bit_value=0b11111111;//level<<pin;
    i2c_master_start();
    i2c_master_send(expander_addr<<1);
    i2c_master_send(EXportbits);
    i2c_master_send(bit_value);
    i2c_master_stop();
}


