////
//// Created by Eddy Yang on 2/23/22.
////

//#ifndef LAB3_UART_H
//#define LAB3_UART_H
//
//
//void initialise_uart(int prescaler);
//
//
//
//void send_string(char* string);
//
//void send_uart(unsigned char val);
//
//#endif //LAB3_UART_H


#ifndef UART_H
#define UART_H

void UART_init(int prescale);

void UART_send(unsigned char data);

void UART_putstring(char* StringPtr);

#endif
