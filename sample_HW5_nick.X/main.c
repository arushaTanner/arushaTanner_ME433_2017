#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "i2c_master_noint.h"

// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
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

#define SLAVE_ADDR 0b0100000

void init_expander(void);
unsigned char get_expander(void);
void set_expander(unsigned char);

int main() {
    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    
    // do your TRIS and LAT commands here
    TRISAbits.TRISA4 = 0; // green led
    TRISBbits.TRISB4 = 1; // user pushbutton
    LATAbits.LATA4 = 1;
    
    i2c_master_setup();
    init_expander();
    
    __builtin_enable_interrupts();
    
    unsigned char pins = 0;
    
    while(1) {
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < 48000000/2/1000){}
        
        pins = get_expander();
        if(pins&1 == 0b1){
            set_expander(0);
            //LATAbits.LATA4 = 1;
        }
        else {
            set_expander(1);
            //LATAbits.LATA4 = 0;
        }
    }
}

void init_expander(void){
    i2c_master_start();     
    i2c_master_send(SLAVE_ADDR << 1 | 0);  
    i2c_master_send(0); // io        
    i2c_master_send(0b00001111);                       
    i2c_master_stop();   
    
    i2c_master_start();     
    i2c_master_send(SLAVE_ADDR << 1 | 0);  
    i2c_master_send(0x6); // pullups        
    i2c_master_send(0b00001111);                       
    i2c_master_stop(); 
}

unsigned char get_expander(){
    i2c_master_start();     
    i2c_master_send(SLAVE_ADDR << 1 | 0);  
    i2c_master_send(0x9);        
    i2c_master_restart();              
    i2c_master_send((SLAVE_ADDR << 1) | 1); 
    unsigned char a = i2c_master_recv();          
    i2c_master_ack(1);                    
    i2c_master_stop(); 
    
    return a;
}

void set_expander(unsigned char value){
    i2c_master_start();     
    i2c_master_send(SLAVE_ADDR << 1 | 0);  
    i2c_master_send(0x9);   
    if(value == 0) {
        i2c_master_send(0);
    }
    else {
        i2c_master_send(0b10000);
    }                       
    i2c_master_stop(); 
}