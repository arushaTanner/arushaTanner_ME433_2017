#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "i2c_master.h"
#include "io_expander.h"
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

int main() {

    TRISAbits.TRISA4=0;
    TRISBbits.TRISB4=1;
    LED=0;
    
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
    
    //setup expander
    io_expander_init();
    
    __builtin_enable_interrupts();
    
    //string for debugging this stupid piece of code
    char msg[50];
    LCD_clearScreen(BLACK);    
    sprintf(msg,"Brandon");
    LCD_drawString(msg,GREEN,BLACK,10,10);
    
    
    
    
    unsigned char pin=1;
    
    while(1) {
	    // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
		  // remember the core timer runs at half the CPU speed
        
        if(PUSH_BUTTON==0)
        {
            //io_expander_set(7,0);
            LED=1;
        }
        else if (PUSH_BUTTON==1)
        {
            //io_expander_set(7,1);
            LED=0;
        }
  
        pin=io_expander_get();
        sprintf(msg,"wtf: %i",pin);
        LCD_drawString(msg,GREEN,BLACK,10,20);
        if((pin&1)==0b0)
        {  
            io_expander_set(7,1); 
        }
        else
        {
            io_expander_set(7,0);
        }
        
        
    }
}