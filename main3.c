#if defined(__ICCAVR__)
#define SEI()     __enable_interrupt()
#define SLEEP()   __sleep()
#define NOP()     __no_operation()
#else
#define SEI()     sei()
#define SLEEP()   sleep_cpu()
#define NOP()     __asm__ __volatile__ ("nop" ::)
#endif

#define BAUD_RATE 9600
#define BAUD_PRESCALER ((F_CPU / (BAUD_RATE * 16UL)) - 1)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#if defined(__ICCAVR__)
#include "ioavr.h"
#include "inavr.h"
#else
#include <avr/sleep.h>
#endif
#include "TWI.h"
#include <string.h>
#include "ST7735.c"
#include "LCD_GFX.c"

#define TWI_CMD_MASTER_WRITE 0x10
#define TWI_CMD_MASTER_READ  0x20
#define POWER_MANAGEMENT_ENABLED

char String[50];

// UART setup
void UART_setup() {
    // Set baud rate
    UBRR0H = (unsigned char)(BAUD_PRESCALER >> 8);
    UBRR0L = (unsigned char)BAUD_PRESCALER;
    // Enable receiver and transmitter
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    // Set frame format
    UCSR0C |= (1 << USBS0); // 2 stop bits
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 data bits
}

void UART_send(unsigned char data) {
    // Wait for empty transmit buffer
    while(!(UCSR0A & (1 << UDRE0)));
    // Put data into buffer and send data
    UDR0 = data;
}

void UART_putstring(char* StringPtr) {
    // Loop through the entire string, stopping when NULL is reached
    while(*StringPtr != 0x00) {
        UART_send(*StringPtr);
        StringPtr++;
    }
}

void Initialize() {
    cli(); // Disable global interrupts
    UART_setup();
    UART_putstring("Hello World!");

    DDRD &= ~(1 << DDD3); // Set PORTD3 to be an input pin
    DDRD &= ~(1 << DDD4); // Set PORTD4 to be an input pin

    // ADC Setup
    PRR &= ~(1<<PRADC); // Clear power reduction for ADC (enable ADC)
    ADMUX |= (1<<REFS0); // Set Vref = AVcc (something about an external capacitor, not sure?)
    ADMUX &= ~(1<<REFS1);
    ADCSRA |= (1<<ADPS0); // Prescale the ADC clock by 128 to have a proper clock frequency
    ADCSRA |= (1<<ADPS1);
    ADCSRA |= (1<<ADPS2);
    ADMUX &= ~(1<<MUX0); // Select ADC0 as the ADC channel
    ADMUX &= ~(1<<MUX1);
    ADMUX &= ~(1<<MUX2);
    ADMUX &= ~(1<<MUX3);

    ADCSRA |= (1<<ADATE); // Autotriggering of ADC
    ADCSRB &= ~(1<<ADTS0); // Set to free running mode
    ADCSRB &= ~(1<<ADTS1);
    ADCSRB &= ~(1<<ADTS2);
    DIDR0 |= (1<<ADC0D); // Disable digital input buffer
    ADCSRA |= (1<<ADEN); // Enable ADC
    //ADCSRA |= (1<<ADIE); // Enable ADC interrupt
    ADCSRA |= (1<<ADSC); // Start conversion

    // Initialize the LCD display
    lcd_init();
}

void draw_keyboard () {
    // First row of keyboard squares
    LCD_drawBlock(0, 81, 15, 95, rgb565(255, 255, 255));
    LCD_drawBlock(17, 81, 31, 95, rgb565(255, 255, 255));
    LCD_drawBlock(33, 81, 47, 95, rgb565(255, 255, 255));
    LCD_drawBlock(49, 81, 63, 95, rgb565(255, 255, 255));
    LCD_drawBlock(65, 81, 79, 95, rgb565(255, 255, 255));
    LCD_drawBlock(81, 81, 95, 95, rgb565(255, 255, 255));
    LCD_drawBlock(97, 81, 111, 95, rgb565(255, 255, 255));
    LCD_drawBlock(113, 81, 127, 95, rgb565(255, 255, 255));
    LCD_drawBlock(129, 81, 143, 95, rgb565(255, 255, 255));
    LCD_drawBlock(145, 81, 159, 95, rgb565(255, 255, 255));

    // Second row of keyboard squares
    LCD_drawBlock(0, 97, 15, 111, rgb565(255, 255, 255));
    LCD_drawBlock(17, 97, 31, 111, rgb565(255, 255, 255));
    LCD_drawBlock(33, 97, 47, 111, rgb565(255, 255, 255));
    LCD_drawBlock(49, 97, 63, 111, rgb565(255, 255, 255));
    LCD_drawBlock(65, 97, 79, 111, rgb565(255, 255, 255));
    LCD_drawBlock(81, 97, 95, 111, rgb565(255, 255, 255));
    LCD_drawBlock(97, 97, 111, 111, rgb565(255, 255, 255));
    LCD_drawBlock(113, 97, 127, 111, rgb565(255, 255, 255));
    LCD_drawBlock(129, 97, 143, 111, rgb565(255, 255, 255));

    // Third row of keyboard squares
    LCD_drawBlock(0, 113, 15, 127, rgb565(255, 255, 255));
    LCD_drawBlock(17, 113, 31, 127, rgb565(255, 255, 255));
    LCD_drawBlock(33, 113, 47, 127, rgb565(255, 255, 255));
    LCD_drawBlock(49, 113, 63, 127, rgb565(255, 255, 255));
    LCD_drawBlock(65, 113, 79, 127, rgb565(255, 255, 255));
    LCD_drawBlock(81, 113, 95, 127, rgb565(255, 255, 255));
    LCD_drawBlock(97, 113, 111, 127, rgb565(255, 255, 255));
    // Enter key: LCD_drawBlock(113, 113, 159, 127, rgb565(255, 255, 255));

    // First row of keyboard letters
    LCD_drawChar(6, 85, 'Q', rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(22, 85, 'W', rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(38, 85, 'E', rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(54, 85, 'R', rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(70, 85, 'T', rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(86, 85, 'Y', rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(102, 85, 'U', rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(118, 85, 'I', rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(134, 85, 'O', rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(150, 85, 'P', rgb565(0, 0, 0), rgb565(255, 255, 255));

    // Second row of keyboard letters
    LCD_drawChar(6, 101, 'A', rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(22, 101, 'S', rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(38, 101, 'D', rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(54, 101, 'F', rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(70, 101, 'G', rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(86, 101, 'H', rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(102, 101, 'J', rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(118, 101, 'K', rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(134, 101, 'L', rgb565(0, 0, 0), rgb565(255, 255, 255));

    // Third row of keyboard letters
    LCD_drawChar(6, 117, 'Z', rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(22, 117, 'X', rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(38, 117, 'C', rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(54, 117, 'V', rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(70, 117, 'B', rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(86, 117, 'N', rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(102, 117, 'M', rgb565(0, 0, 0), rgb565(255, 255, 255));
    /* Enter key: LCD_drawChar(118, 117, '=', rgb565(0, 0, 0), rgb565(255, 255, 255));
     * LCD_drawChar(134, 117, '=', rgb565(0, 0, 0), rgb565(255, 255, 255));
     * LCD_drawChar(150, 117, '=', rgb565(0, 0, 0), rgb565(255, 255, 255));*/

}

void draw_pointer (int new_char_pos, uint16_t color) {
    int x = 16 * (new_char_pos % 10) + 2;
    int y = 16 * (new_char_pos / 10) + 82;
    LCD_drawBlock(x, y, x + 12, y + 1, color);
    LCD_drawBlock(x, y + 11, x + 12, y + 12, color);
    LCD_drawBlock(x, y + 2,  x + 1, y + 10, color);
    LCD_drawBlock(x + 11, y + 2, x + 12, y + 10, color);
}

void draw_high_scores (int scores [4], char names [4][3]) {
    /*LCD_drawString(40, 24, "1. XXX => 000", rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawString(40, 34, "2. XXX => 000", rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawString(40, 44, "3. XXX => 000", rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawString(40, 54, "4. XXX => 000", rgb565(0, 0, 0), rgb565(255, 255, 255));*/

    // Erase old score
    LCD_drawBlock(55, 24, 70, 64, rgb565(255, 255, 255));
    LCD_drawBlock(90, 24, 105, 64, rgb565(255, 255, 255));

    // Draw score 1
    LCD_drawChar(55, 24, names[0][0], rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(60, 24, names[0][1], rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(65, 24, names[0][2], rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(90, 24, '0' + scores[0] / 100, rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(95, 24, '0' + (scores[0] % 100) / 10, rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(100, 24, '0' + (scores[0] % 10), rgb565(0, 0, 0), rgb565(255, 255, 255));
    // Draw score 2
    LCD_drawChar(55, 34, names[1][0], rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(60, 34, names[1][1], rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(65, 34, names[1][2], rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(90, 34, '0' + scores[1] / 100, rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(95, 34, '0' + (scores[1] % 100) / 10, rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(100, 34, '0' + (scores[1] % 10), rgb565(0, 0, 0), rgb565(255, 255, 255));
    // Draw score 3
    LCD_drawChar(55, 44, names[2][0], rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(60, 44, names[2][1], rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(65, 44, names[2][2], rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(90, 44, '0' + scores[2] / 100, rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(95, 44, '0' + (scores[2] % 100) / 10, rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(100, 44, '0' + (scores[2] % 10), rgb565(0, 0, 0), rgb565(255, 255, 255));
    // Draw score 4
    LCD_drawChar(55, 54, names[3][0], rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(60, 54, names[3][1], rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(65, 54, names[3][2], rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(90, 54, '0' + scores[3] / 100, rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(95, 54, '0' + (scores[3] % 100) / 10, rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawChar(100, 54, '0' + (scores[3] % 10), rgb565(0, 0, 0), rgb565(255, 255, 255));

}

char string [100];

int main(void) {
    Initialize();

    int char_pos = 0;
    int count = 0;
    char name [3];
    int accepting_name = 1;

    int highscores [4];
    highscores[0] = 0;
    highscores[1] = 0;
    highscores[2] = 0;
    highscores[3] = 0;

    char highscore_names [4][3];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            highscore_names[i][j] = 'X';
        }
    }

    // Prepare I2C
    unsigned char messageBuf[TWI_BUFFER_SIZE];
    unsigned char TWI_slaveAddress;
    TWI_slaveAddress = 0x08;
    TWI_Slave_Initialise( (unsigned char)((TWI_slaveAddress<<TWI_ADR_BITS) | (TRUE<<TWI_GEN_BIT) ));
    SEI();
    TWI_Start_Transceiver();

    // Clear screen
    LCD_setScreen(rgb565(0, 0, 0));

    // Draw score box
    LCD_drawBlock(0, 0, 159, 10, rgb565(255, 255, 255));
    LCD_drawString(7, 2, "Y O U R   S C O R E :", rgb565(0, 0, 0), rgb565(255, 255, 255));

    // Draw high score board
    LCD_drawBlock(0, 12, 159, 66, rgb565(255, 255, 255));
    LCD_drawString(45, 14, "High Scores:", rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawString(40, 24, "1. ", rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawString(40, 34, "2. ", rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawString(40, 44, "3. ", rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawString(40, 54, "4. ", rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawString(75, 24, "=>", rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawString(75, 34, "=>", rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawString(75, 44, "=>", rgb565(0, 0, 0), rgb565(255, 255, 255));
    LCD_drawString(75, 54, "=>", rgb565(0, 0, 0), rgb565(255, 255, 255));
    draw_high_scores(highscores, highscore_names);

    // Draw text box for entering name
    LCD_drawBlock(0, 68, 159, 79, rgb565(255, 255, 255));
    LCD_drawString(7, 70, "Enter name to play: ", rgb565(0, 0, 0), rgb565(255, 255, 255));


    // Draw keyboard
    draw_keyboard();

    // Draw letter select icon
    draw_pointer(char_pos, rgb565(255, 0, 0));

    // Encoding of letter position: (x, y), 0<=x<=9, 0<=y<=2
    // Single int position has first digit y position, second digit x position

    while(1) {
        if (accepting_name) {
            if (!(PIND & (1 << PIND3))) {
                if (count < 3) {
                    char c;
                    switch (char_pos) {
                        case 0:
                            c = 'Q';
                            break;
                        case 1:
                            c = 'W';
                            break;
                        case 2:
                            c = 'E';
                            break;
                        case 3:
                            c = 'R';
                            break;
                        case 4:
                            c = 'T';
                            break;
                        case 5:
                            c = 'Y';
                            break;
                        case 6:
                            c = 'U';
                            break;
                        case 7:
                            c = 'I';
                            break;
                        case 8:
                            c = 'O';
                            break;
                        case 9:
                            c = 'P';
                            break;
                        case 10:
                            c = 'A';
                            break;
                        case 11:
                            c = 'S';
                            break;
                        case 12:
                            c = 'D';
                            break;
                        case 13:
                            c = 'F';
                            break;
                        case 14:
                            c = 'G';
                            break;
                        case 15:
                            c = 'H';
                            break;
                        case 16:
                            c = 'J';
                            break;
                        case 17:
                            c = 'K';
                            break;
                        case 18:
                            c = 'L';
                            break;
                        case 20:
                            c = 'Z';
                            break;
                        case 21:
                            c = 'X';
                            break;
                        case 22:
                            c = 'C';
                            break;
                        case 23:
                            c = 'V';
                            break;
                        case 24:
                            c = 'B';
                            break;
                        case 25:
                            c = 'N';
                            break;
                        case 26:
                            c = 'M';
                            break;
                    }
                    LCD_drawChar(110 + count * 8, 70, c, rgb565(0, 0, 0), rgb565(255, 255, 255));
                    name[count] = c;
                    count++;
                }
                if (count == 3) {
                    // Now that count is 3, the name is complete and we should remove the red square and wait for a punch
                    draw_pointer(char_pos, rgb565(255, 255, 255));
                    accepting_name = 0;
                }
                UART_putstring("Button Pressed!\n");
            } else {
                int xpos = ADC;
                ADMUX |= (1 << MUX0);
                sprintf(string, "X-position: %d, ", xpos);
                UART_putstring(string);
                int ypos = ADC;
                ADMUX &= ~(1 << MUX0);
                sprintf(string, "Y-position: %d, ", ypos);
                UART_putstring(string);

                // Find absolute difference of position values from the center
                int xdir = 0;
                int ydir = 0;
                xpos -= 512;
                if (xpos < 0) {
                    // Take absolute value and indicate that the joystick was pushed in the negative direction
                    xpos *= -1;
                    xdir = 1;
                }
                if (xpos < 100) {
                    // Add a buffer of 100 units in any direction to prevent unintended movements
                    xpos = 0;
                }

                ypos -= 512;
                if (ypos < 0) {
                    ypos *= -1;
                    ydir = 1;
                }
                if (ypos < 100) {
                    ypos = 0;
                }
                char dir = (xpos > ypos) ? ((xdir == 0) ? 'D' : 'U') : ((ypos > xpos) ? ((ydir == 0) ? 'R' : 'L')
                                                                                      : 'X');
                switch (dir) {
                    case 'D':
                        if (char_pos != 9 && char_pos != 17 && char_pos != 18 && char_pos < 20) {
                            draw_pointer(char_pos, rgb565(255, 255, 255));
                            char_pos += 10;
                            draw_pointer(char_pos, rgb565(255, 0, 0));
                        }
                        break;
                    case 'U' :
                        if (char_pos != 29 && char_pos >= 10) {
                            draw_pointer(char_pos, rgb565(255, 255, 255));
                            char_pos -= 10;
                            draw_pointer(char_pos, rgb565(255, 0, 0));
                        }
                        break;
                    case 'R' :
                        if (char_pos != 18 && char_pos != 26 && char_pos % 10 < 9) {
                            draw_pointer(char_pos, rgb565(255, 255, 255));
                            char_pos += 1;
                            draw_pointer(char_pos, rgb565(255, 0, 0));
                        }
                        break;
                    case 'L' :
                        if (char_pos % 10 > 0) {
                            draw_pointer(char_pos, rgb565(255, 255, 255));
                            char_pos -= 1;
                            draw_pointer(char_pos, rgb565(255, 0, 0));
                        }
                        break;
                    case 'X' :
                        break;
                }
            }
        } else {
            // No longer accepting name, look for punch from I2C
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
                        sprintf(String, "%s", messageBuf);
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

            if ((PIND & (1 << PIND4))) {
                // Reset
                char_pos = 0;
                draw_pointer(char_pos, rgb565(255, 0, 0));
                count = 0;
                // Cover up the old name and score
                LCD_drawBlock(120, 0, 159, 10, rgb565(255, 255, 255));
                LCD_drawBlock(110, 68, 159, 79, rgb565(255, 255, 255));
                // Accept name again
                accepting_name = 1;
            } else {
                // Wait for punch reading

                // LOGIC TO DETERMINE PUNCH SCORE FROM SENSOR READING
                int punch = 123;

                // Update "Your Score"
                LCD_drawBlock(120, 0, 159, 10, rgb565(255, 255, 255));
                LCD_drawChar(127, 2, '0' + punch / 100, rgb565(0, 0, 0), rgb565(255, 255, 255));
                LCD_drawChar(137, 2, '0' + (punch % 100) / 10, rgb565(0, 0, 0), rgb565(255, 255, 255));
                LCD_drawChar(147, 2, '0' + (punch % 10), rgb565(0, 0, 0), rgb565(255, 255, 255));

                // Check to see if a high score has been set
                if (punch > highscores[3]) {
                    highscores[3] = punch;
                    highscore_names[3][0] = name[0];
                    highscore_names[3][1] = name[1];
                    highscore_names[3][2] = name[2];
                    if (punch > highscores[2]) {
                        highscores[3] = highscores[2];
                        highscores[2] = punch;
                        highscore_names[3][0] = highscore_names[2][0];
                        highscore_names[3][1] = highscore_names[2][1];
                        highscore_names[3][2] = highscore_names[2][2];
                        highscore_names[2][0] = name[0];
                        highscore_names[2][1] = name[1];
                        highscore_names[2][2] = name[2];
                        if (punch > highscores[1]) {
                            highscores[2] = highscores[1];
                            highscores[1] = punch;
                            highscore_names[2][0] = highscore_names[1][0];
                            highscore_names[2][1] = highscore_names[1][1];
                            highscore_names[2][2] = highscore_names[1][2];
                            highscore_names[1][0] = name[0];
                            highscore_names[1][1] = name[1];
                            highscore_names[1][2] = name[2];
                            if (punch > highscores[0]) {
                                highscores[1] = highscores[0];
                                highscores[0] = punch;
                                highscore_names[1][0] = highscore_names[0][0];
                                highscore_names[1][1] = highscore_names[0][1];
                                highscore_names[1][2] = highscore_names[0][2];
                                highscore_names[0][0] = name[0];
                                highscore_names[0][1] = name[1];
                                highscore_names[0][2] = name[2];
                            }
                        }
                    }
                    // Update leaderboard
                    UART_putstring("\n");
                    draw_high_scores(highscores, highscore_names);
                }
            }
        }
    }


// TODO: Next Steps:
// - External storage by storing high scores on screen
// - Add buzzer + any additional features + servo motor / bell?
