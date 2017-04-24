#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "i2c_master.h"
#include "LED.h"
#include <stdio.h>

// DEVCFG0
#pragma config DEBUG = OFF // no debugging 
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL =  ICS_PGx1// use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // free up secondary osc pins
#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // slowest wdt
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN = OFF // wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 48MHz
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiply by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

#define LED LATAbits.LATA4 
#define PUSH_BUTTON PORTBbits.RB4

//addresses for accelerometer
#define address 0b1101010
#define WHO_AM_I 0x0F
#define OUTPUT 0x20
#define CTRL1_XL 0x10 //0b10000010
#define CTRL2_G 0x11 //0b10001000
#define CTRL3_C 0x12 //0b00000100 -> default value

unsigned char read(unsigned char reg);
void accel_init();

int main() {

    TRISAbits.TRISA4 = 0;
    TRISBbits.TRISB4 = 1;
    LED = 0;

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    //turn off analog pins
    ANSELBbits.ANSB2 = 0;
    ANSELBbits.ANSB3 = 0;

    //setup LCD screen
    SPI1_init();
    LCD_init();

    //setup i2c communication
    i2c_master_setup();

    //initialize accelerometer
    accel_init();

    __builtin_enable_interrupts();

    //string for debugging this stupid piece of code
    char msg[50];
    LCD_clearScreen(BLACK);
    sprintf(msg, "Tanner");
    LCD_drawString(msg, GREEN, BLACK, 10, 10);

    //unsigned char whoami = read(WHO_AM_I);
    //sprintf(msg, "who: %i", whoami);
    //LCD_drawString(msg, GREEN, BLACK, 10, 20);
    
    short data[7];

    while (1) {
        // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
        // remember the core timer runs at half the CPU speed

        if (PUSH_BUTTON == 0)LED = 1;
        else if (PUSH_BUTTON == 1)LED = 0;
        
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT()<48000000/2/10);
        
        read_data(data,7);
        
        //draw bars in x direction
        int x=-1*data[4];
        x=(x*50)/16000;
        LCD_drawBarX(x,RED,BLACK);
        LCD_drawBarX(-x,BLACK,BLACK);
        
        //draw bars in y direction
        int y=-1*data[5];
        y=(y*50)/16000;
        LCD_drawBarY(y,RED,BLACK);
        LCD_drawBarY(-y,BLACK,BLACK);
        
        //draw center
        LCD_drawCenter(BLUE);
        
        sprintf(msg, "x:%i  ",x);
        LCD_drawString(msg,GREEN,BLACK,70,10);
        sprintf(msg,"y:%i  ",y);
        LCD_drawString(msg,GREEN,BLACK,70,20);
        


    }
}

unsigned char read(unsigned char reg) {
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
void read_data(short *data, int length) {
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