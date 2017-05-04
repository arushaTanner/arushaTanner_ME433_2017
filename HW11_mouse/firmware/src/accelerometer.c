#include "accelerometer.h"

unsigned char accel_read(unsigned char reg) {
    i2c_master_start();
    i2c_master_send(address << 1 | 0);
    i2c_master_send(reg);
    i2c_master_restart();
    i2c_master_send(address << 1 | 1);
    unsigned char data = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();

    return data;
}

//read function where data is 1/2 length. Each read is half of the data.
void accel_read_data(short *data, int length) {
    i2c_master_start();
    i2c_master_send(address << 1 | 0);
    i2c_master_send(OUTPUT);
    i2c_master_restart();
    i2c_master_send(address << 1 | 1);

    unsigned char d1 = 0;
    unsigned char d2 = 0;
    
    int i;
    for (i = 0; i < length; i++) {
        d1 = i2c_master_recv();
        i2c_master_ack(0);
        d2 = i2c_master_recv();
        if(i==(length-1))i2c_master_ack(1);
        else(i2c_master_ack(0));
        data[i] = (d2 << 8 )| d1;
    }
    i2c_master_stop();
}



void accel_init()
{
    i2c_master_start();
    i2c_master_send(address << 1 | 0);
    i2c_master_send(CTRL1_XL);
    i2c_master_send(0b10000010);
    i2c_master_stop();

    i2c_master_start();
    i2c_master_send(address << 1 | 0);
    i2c_master_send(CTRL2_G);
    i2c_master_send(0b10001000);
    i2c_master_stop();

    i2c_master_start();
    i2c_master_send(address << 1 | 0);
    i2c_master_send(CTRL3_C);
    i2c_master_send(0b00000100);
    i2c_master_stop();
}