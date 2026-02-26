#ifndef BUTTON_H
#define BUTTON_H
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "globals.h"
#include "serial.h"


extern uint16_t DEBOUNCE_DELAY;       // Time between button press checks
extern uint16_t COMBINATION_DELAY;
extern bool STATES[5];
extern bool COMBINATION;              // sets true if combination of keys was pressed 

/* 
  if MENU = false
  checking and processing buttons
  buttons move cursor and Enter set/unset drum hit
*/
void notInMenu();

/*
   if MENU = true
   check and process buttons 
   up, down - move menu lines 
   left, right - change menu values
*/
void inMenu();

/*
   endless cycle for checking buttons and run
   notInMenu()        moving cursor on MAX7219 leds and sets drum hit
   inMenu()           moving between menu lines and change menu settings

   all ping in PULLUP mode so if
   pin state == 0 - button pressed
*/
void checkButtons(void *arg);

#endif

