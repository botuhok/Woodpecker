#ifndef GLOBALS_H
#define GLOBALS_H
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "presets.h"
#define VERSION "0.4 (05/02/2026)"

#define SLEEPMODE true                  // do you need sleep mode or not? SLEEPAFTER - minutes when not playing and
                                        // not pressing before sleep

// PINS

// uart pins for DFPlayer Mini
#define TX_PIN 21
#define  RX_PIN 20

// buttons pins
#define ENTER_PIN 10
#define UP_PIN 0
#define DOWN_PIN 1
#define LEFT_PIN 3
#define RIGHT_PIN 2

// MAX7219 pins
#define DIN_PIN 6
#define CLK_PIN 5
#define CS_PIN 7
#define BRIGHTNESS_MAX7219 1           // brightness for MAX7219 led matrix (0-15, default 1)

// OLED1306 pins
#define SCL_PIN 9
#define SDA_PIN 8
#define CONTRAST_OLED 0x50             // contrast for oled display (0-255, default 0x50)

// MAX7219 led matrix size
extern uint8_t COORDSX_MIN;
extern uint8_t COORDSX_MAX;
extern uint8_t COORDSY_MIN;
extern uint8_t COORDSY_MAX;


extern uint8_t COORDS[2];              // {X, Y} actual cursor coords (on MAX7219)
extern uint8_t PRECOORDS[2];           // {X, Y} previous cursor coords (for cleaning)
extern bool CURSOR_MOVE;               // is cursor now moving or not? 
extern bool ANYPRESSED;                // true if something was pressed until it was processed, then sets to false
extern uint8_t SLEEPAFTER;             // sleep after min


extern bool MENU;                      // changing to true/false when enter + down
extern uint8_t MENUINDEX;              // actual index of menu lines
extern char MENUITEMS[8][10];          // menu items names for oled display
extern uint16_t MENUVALUES[8];         // parameters for bpm, preset, shuffle
extern uint8_t KITSMAX;                // how many kits (sound presets) on sd card (maximum - 10)
extern uint16_t BPMMAX;                // maximum BPM (which sets in menu)
extern uint16_t BPM_PAUSE;             // ms delay between drums hits (default for 120 bpm = 125)
extern uint8_t BPMMIN;
extern uint8_t SHUFFLEMAX;             // maximum shuffle ms delay after each even hit
extern uint8_t CURRENTVOLUME;          // current global volume
extern uint8_t PREVIOUSVOLUME;         // previous global volume (for sending UART only if changing)
extern uint8_t MAXVOLUME;              // maximum global volume
extern bool VOLUMECHANGE;              // sets true if volume change via menu

extern bool PLAY;                      // changing to true/false when pressed ENTER + UP
extern bool MAX_STATE;                 // cursor led state (for blinking)
extern bool NEED_REFRESH_OLED;         // if oled needs refresh - sets to true, if refresh done - sets to false
extern bool NEED_REFRESH_MAX;          // if MAX7219 led matrix needs refresh - sets to true, if refresh is done - sets to false


#define PATTERNS 4                     // how many drums patterns
extern uint8_t PATTERN;                // contain current playing patter
extern bool DRUMS[PATTERNS][8][16];    // array of drums patterns
extern uint8_t DRUMSVOL[PATTERNS][16]; // contain volumes sets for each drum column


extern uint8_t NOW_PLAYING;            // which column now playing;
extern bool STARTPLAYING;              // set to true when start playing and after this sets to false!


/* 
   for loading preloops (presets) to DRUMS array 
*/
void setPreset(bool preset[PATTERNS][8][16]);

/*
   initialization drum array volumes
*/
void initVol();

#endif