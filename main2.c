//
// Created by Eddy Yang on 4/21/22.
//

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <uart.h>
#include <stdio.h>
#if defined(__ICCAVR__)
#include "ioavr.h"
#include "inavr.h"
#else
#include <avr/sleep.h>
#endif
#include "TWI.h"
#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

// Sample TWI transmission commands
#define TWI_CMD_MASTER_WRITE 0x10
#define TWI_CMD_MASTER_READ  0x20

// The AVR can be waken up by a TWI address match from all sleep modes,
// But it only wakes up from other TWI interrupts when in idle mode.
// If POWER_MANAGEMENT_ENABLED is defined the device will enter power-down
// mode when waiting for a new command and enter idle mode when waiting
// for TWI receives and transmits to finish.
#define POWER_MANAGEMENT_ENABLED

// Compiler-independent macros (was previously IAR intrinsics)
#if defined(__ICCAVR__)
#define SEI()     __enable_interrupt()
#define SLEEP()   __sleep()
#define NOP()     __no_operation()
#else
#define SEI()     sei()
#define SLEEP()   sleep_cpu()
#define NOP()     __asm__ __volatile__ ("nop" ::)
#endif

char String[50];

// When there has been an error, this function is run and takes care of it
//unsigned char TWI_Act_On_Failure_In_Last_Transmission ( unsigned char TWIerrorMsg );


//#if defined(__ICCAVR__)
//void
//#else
//int
//#endif
main(void)
{
    UART_init(BAUD_PRESCALER);

    unsigned char messageBuf[TWI_BUFFER_SIZE];
    unsigned char TWI_slaveAddress;

//    // LED feedback port - connect port B to the STK500 LEDS
//    DDRB  = 0xFF; // Set to ouput
//    PORTB = 0x55; // Startup pattern

    // Own TWI slave address
    TWI_slaveAddress = 0x08;

    // Initialise TWI module for slave operation. Include address and/or enable General Call.
    TWI_Slave_Initialise( (unsigned char)((TWI_slaveAddress<<TWI_ADR_BITS) | (TRUE<<TWI_GEN_BIT) ));

    SEI();

    TWI_Start_Transceiver();

    // This example is made to work together with the AVR315 TWI Master application note. In adition to connecting the TWI
    // pins, also connect PORTB to the LEDS. The code reads a message as a TWI slave and acts according to if it is a
    // general call, or an address call. If it is an address call, then the first byte is considered a command byte and
    // it then responds differently according to the commands.

    // This loop runs forever. If the TWI is busy the execution will just continue doing other operations.
    while (1)
    {
#ifdef POWER_MANAGEMENT_ENABLED
        // Sleep while waiting for TWI transceiver to complete or waiting for new commands.
        // If we have data in the buffer, we can't enter sleep because we have to take care
        // of it first.
        // If the transceiver is busy, we enter idle mode because it will wake up by all TWI
        // interrupts.
        // If the transceiver not is busy, we can enter power-down mode because next receive
        // should be a TWI address match and it wakes the device up from all sleep modes.
        if( ! TWI_statusReg.RxDataInBuf ) {
            if(TWI_Transceiver_Busy()) {
                MCUCR = (1<<SE)|(0<<SM2)|(0<<SM1)|(0<<SM0); // Enable sleep with idle mode
            } else {
                MCUCR = (1<<SE)|(0<<SM2)|(1<<SM1)|(0<<SM0); // Enable sleep with power-down mode
            }
            SLEEP();
        } else {
            NOP(); // There is data in the buffer, code below takes care of it.
        }
#else // No power management
        // Here you can add your own code that should be run while waiting for the TWI to finish
      NOP(); // Put own code here.
#endif


        // Check if the TWI Transceiver has completed an operation.
        if (!TWI_Transceiver_Busy())
        {
//            sprintf(String, "enter Transceiver\n");
//            UART_putstring(String);
            // Check if the last operation was successful
            if (TWI_statusReg.lastTransOK)
            {
//                sprintf(String, "enter last op successful\n");
//                UART_putstring(String);
                // Check if the last operation was a reception
                if (TWI_statusReg.RxDataInBuf)
                {
                    sprintf(String, "last op was reception\n");
                    UART_putstring(String);
                    TWI_Get_Data_From_Transceiver(messageBuf, 10);
                    sprintf(String, messageBuf);
                    UART_putstring(String);
                    // Check if the last operation was a reception as General Call
//                    if ( TWI_statusReg.genAddressCall )
//                    {
//                        // Put data received out to PORTB as an example.
//                        PORTB = messageBuf[0];
////                        sprintf(String, "communicating\n");
////                        UART_putstring(String);
//                    }
//                    else // Ends up here if the last operation was a reception as Slave Address Match
//                    {
//                        // Example of how to interpret a command and respond.
////                        sprintf(String, "slave address match\n");
////                        UART_putstring(String);
//                        // TWI_CMD_MASTER_WRITE stores the data to PORTB
//                        if (messageBuf[0] == TWI_CMD_MASTER_WRITE)
//                        {
////                            sprintf(String, messageBuf);
////                            UART_putstring(String);
//                            //if we enter here it means data has been stored in messageBuf
//                            //then we just have to combine with LCD code to print out data
//                            PORTB = messageBuf[1];
////                            sprintf(String, "store data\n");
////                            UART_putstring(String);
//                        }
//                        // TWI_CMD_MASTER_READ prepares the data from PINB in the transceiver buffer for the TWI master to fetch.
//                        if (messageBuf[0] == TWI_CMD_MASTER_READ)
//                        {
////                            sprintf(String, "prepare for master\n");
////                            UART_putstring(String);
//                            messageBuf[0] = PINB;
//                            TWI_Start_Transceiver_With_Data( messageBuf, 1 );
//                        }
//                    }
                }
//                else // Ends up here if the last operation was a transmission
//                {
////                    sprintf(String, "transmitted\n");
////                    UART_putstring(String);
//                    NOP(); // Put own code here.
//                }
                // Check if the TWI Transceiver has already been started.
                // If not then restart it to prepare it for new receptions.
                if (!TWI_Transceiver_Busy())
                {
//                    sprintf(String, "transceiver started\n");
//                    UART_putstring(String);
                    TWI_Start_Transceiver();
                }
            }
//            else // Ends up here if the last operation completed unsuccessfully
//            {
//                TWI_Act_On_Failure_In_Last_Transmission(TWI_Get_State_Info());
////                sprintf(String, "failed\n");
////                UART_putstring(String);
//            }
        }
    }
}


//unsigned char TWI_Act_On_Failure_In_Last_Transmission ( unsigned char TWIerrorMsg )
//{
//    // A failure has occurred, use TWIerrorMsg to determine the nature of the failure
//    // and take appropriate actions.
//    // Se header file for a list of possible failures messages.
//
//    // This very simple example puts the error code on PORTB and restarts the transceiver with
//    // all the same data in the transmission buffers.
//    PORTB = TWIerrorMsg;
//    TWI_Start_Transceiver();
//
//    return TWIerrorMsg;
//}