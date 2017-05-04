#ifndef _ACCELEROMETER_H    /* Guard against multiple inclusion */
#define _ACCELEROMETER_H

//addresses for accelerometer
#define address 0b1101010
#define WHO_AM_I 0x0F
#define OUTPUT 0x20
#define CTRL1_XL 0x10 //0b10000010
#define CTRL2_G 0x11 //0b10001000
#define CTRL3_C 0x12 //0b00000100 -> default value

void accel_init();
void accel_read_data(short *data, int length);
unsigned char accel_read(unsigned char reg);

#endif /* _ACCELEROMETER_H */

/* *****************************************************************************
 End of File
 */
