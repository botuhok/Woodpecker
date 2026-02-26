#ifndef MAX_H
#define MAX_H
#include <stdbool.h>
#include "driver/gpio.h"   
#include "globals.h"


//unsigned short leftBuffer[8];
//unsigned short rightBuffer[8];
void sendToChain(unsigned int address, unsigned int dataFar, unsigned int dataNear);
void initMax7219();
void updateDisplay();
void setPixel(unsigned short x, unsigned short y, bool state, bool UPDATE);


#endif