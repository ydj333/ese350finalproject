Welcome to our ESE350 Final Project: Unbox Your Troubles

Devpost: https://devpost.com/software/unbox-your-troubles

Demo Video Link: https://youtu.be/5WyCskeEP0A

How to compile/run our code:

A total of three boards (2 NodeMCUs and 1 Arduino) are needed to run this code.

The file titled NodeMCU_AccelerometerReadings.ino needs to be flashed to the NodeMCU connected to the accelerometer inside the punching bag using the Arduino IDE.
The file titled NodeMCU_Communication.ino needs to be flashed to the NodeMCU connected to the Arduino over I2C using the Arduino IDE.
The majority of the project was completed in a series of files that need to be compiled, linked, and flashed onto the ATmega328P. ASCII_LUT.h, ST7735.h, ST7735.c, LCD_GFX.h, and LCD_GFX.c form the graphics library used to interface with the LCD screen. uart.h and uart.c form a small library used to communicate with the serial monitor on a computer over UART for debugging purposes. TWI.h and TWI.c consist of a few functions inspired by the AVR TWI (another name for I2C) library that we used to communicate with the NodeMCU over I2C. And finally, main.c consists of the core code that pulls all of these pieces together.
