#ifndef SERIAL_H
#define SERIAL_H

#include "driver/uart.h"

#define UART_NUM UART_NUM_1  // Используем UART1 (Serial1)
      
#define UART_SPEED 9600
#define BUF_SIZE 1024

void initUart(uint32_t speed, uint8_t tx, uint8_t rx);

// Function to send a command to the MP3-TF-16P module
void playMp3(uint8_t param1, uint8_t param2);
//void incGlobalVolume();               // increase volume of mp3player via UART command
//void decGlobalVolume();               // decrease volume of mp3player via UART command
void setVolume(uint8_t volume);         // set volume of column
#endif
