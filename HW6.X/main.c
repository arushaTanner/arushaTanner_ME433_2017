#include "LED.h"
#include <xc.h>           // processor SFR definitions
#include <sys/attribs.h>  // __ISR macro
#include <stdio.h>

// DEVCFG0
#pragma config DEBUG = 0b10 // no debugging
#pragma config JTAGEN = 0 // no jtag
#pragma config ICESEL =  0b11// use PGED1 and PGEC1
#pragma config PWP = 0b1111111 // no write protect
#pragma config BWP = 1 // no boot write protect
#pragma config CP = 1 // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = 0b10 // high speed crystal mode
#pragma config OSCIOFNC = 1 // free up secondary osc pins
#pragma config FPBDIV = 0b00 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = 0b11 // do not enable clock switch
#pragma config WDTPS = 0b10100 // slowest wdt
#pragma config WINDIS = 1 // no wdt window
#pragma config FWDTEN = 0 // wdt off by default
#pragma config FWDTWINSZ = 0b11 // wdt window at 25%

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
        
    //initialize LCD screen
    SPI1_init();
    LCD_init();
    
    __builtin_enable_interrupts();
 
    
    
    LCD_clearScreen(BLACK);
    
    char msg[50];
    sprintf(msg,"Brandon Tanner");
    LCD_drawString(msg,GREEN,BLACK,10,10);
    
    int counter=0;
    
    while(1) {
	    // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
		  // remember the core timer runs at half the CPU speed
        
        if(PUSH_BUTTON==0)LED=1;
        else if (PUSH_BUTTON==1)LED=0;
        
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT()<500000){}
        
        //code for the bar
        sprintf(msg,"Hello World:%i   ",counter);
        LCD_drawString(msg,RED,BLACK,18,32);
        
        //if(counter==-50)LCD_drawBar(-50,50,0,YELLOW);
        //else if(counter<0) LCD_drawBar(0,counter,-50,YELLOW);
        if(counter==50) LCD_drawBar(0,50,50,BLACK,BLACK);
        else if(counter>0) LCD_drawBar(0,counter,50,YELLOW,BLACK);
        else LCD_drawBar(-50,counter,0,BLACK,YELLOW);
        
        counter++;
        if(counter==51)counter=-50;
              
    }
}