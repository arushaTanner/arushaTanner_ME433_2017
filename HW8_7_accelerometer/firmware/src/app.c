/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "i2c_master.h"
#include "LED.h"
#include <stdio.h>

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

#define LED LATAbits.LATA4 
#define PUSH_BUTTON PORTBbits.RB4

//addresses for accelerometer
#define address 0b1101010
#define WHO_AM_I 0x0F
#define OUTPUT 0x20
#define CTRL1_XL 0x10 //0b10000010
#define CTRL2_G 0x11 //0b10001000
#define CTRL3_C 0x12 //0b00000100 -> default value


// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
 */

APP_DATA appData;
short data[7];
char msg[50];

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
 */

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
 */
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

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize(void) {
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;


    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}

/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks(void) {

    /* Check the application's current state. */
    switch (appData.state) {
            /* Application's initial state. */
        case APP_STATE_INIT:
        {


            TRISAbits.TRISA4 = 0;
            TRISBbits.TRISB4 = 1;
            LED = 0;


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

            //string for debugging this stupid piece of code
            
            LCD_clearScreen(BLACK);
            sprintf(msg, "Tanner");
            LCD_drawString(msg, GREEN, BLACK, 10, 10);

            //unsigned char whoami = read(WHO_AM_I);
            //sprintf(msg, "who: %i", whoami);
            //LCD_drawString(msg, GREEN, BLACK, 10, 20);

            

            bool appInitialized = true;

            if (appInitialized) {

                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            if (PUSH_BUTTON == 0)LED = 1;
            else if (PUSH_BUTTON == 1)LED = 0;

            _CP0_SET_COUNT(0);
            while (_CP0_GET_COUNT() < 48000000 / 2 / 10);

            read_data(data, 7);

            //draw bars in x direction
            int x = -1 * data[4];
            x = (x * 50) / 16000;
            LCD_drawBarX(x, RED, BLACK);
            LCD_drawBarX(-x, BLACK, BLACK);

            //draw bars in y direction
            int y = -1 * data[5];
            y = (y * 50) / 16000;
            LCD_drawBarY(y, RED, BLACK);
            LCD_drawBarY(-y, BLACK, BLACK);

            //draw center
            LCD_drawCenter(BLUE);

            sprintf(msg, "x:%i  ", x);
            LCD_drawString(msg, GREEN, BLACK, 70, 10);
            sprintf(msg, "y:%i  ", y);
            LCD_drawString(msg, GREEN, BLACK, 70, 20);


            break;
        }

            /* TODO: implement your application state machine.*/


            /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}



/*******************************************************************************
 End of File
 */
