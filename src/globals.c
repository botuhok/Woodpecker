#include "globals.h"

uint8_t COORDS[2] = {0, 0};                  // actual cursor coords (on MAX7219)
uint8_t PRECOORDS[2] = {0, 0};               // previous cursor coords (for cleaning)
uint8_t PATTERN = 0;
bool MENU = false;                           // changing to true/false when enter + down
bool PLAY = false;                           // changing to true/false whet enter + up
bool MAX_STATE = false;                      // cursor led state (for blinking)
bool NEED_REFRESH_OLED = false;              // if oled needs refresh - sets to true, if refresh done - sets to false
bool NEED_REFRESH_MAX = true;                // if MAX7219 led matrix needs refresh - sets to true, if refresh is done - sets to false
bool CURSOR_MOVE = false;                    // is cursor now moving or not? 

// MAX7219 led matrix size
uint8_t COORDSX_MIN = 0;
uint8_t COORDSX_MAX = 15;
uint8_t COORDSY_MIN = 0;
uint8_t COORDSY_MAX = 7;
bool ANYPRESSED = false;
uint8_t SLEEPAFTER = 5;                                          // sleep after min

bool DRUMS[PATTERNS][8][16] = {{}};                              // array of drums patterns

char MENUITEMS[8][10] = {"BPM", "Kit", "Shuffle", "Volume", "Pattern", "Preset", "", ""};  // menu items names for oled display
uint8_t MENUINDEX = 0;                                           // actual index of menu items
uint16_t MENUVALUES[8] = {100, 0, 0, 30, 0, 0, 0, 0};            // parameters for bpm, preset, shuffle

uint8_t KITSMAX = 9;                                           // how many sound presets (folders on sd card)
uint16_t BPM_PAUSE = 150;                                        // ms delay between drums hits (default for 120 bpm = 60000/120/4 = 125)
uint16_t BPMMAX = 200;                                           // maximum BPM (which sets in menu)
uint8_t BPMMIN = 30;                                             // minimum bpm (which sets in menu)
uint8_t SHUFFLEMAX = 100;                                        // maximum shuffle ms delay after each even hit
uint8_t CURRENTVOLUME = 30;                                      // current global volume
uint8_t PREVIOUSVOLUME = 30;                                     // previous volume (for sending UART only if changing)
uint8_t MAXVOLUME = 30;                                          // maximum global volume
uint8_t DRUMSVOL[PATTERNS][16]= {};                              // volume of each column
uint8_t NOW_PLAYING = 0;                                         // which column now playing
bool STARTPLAYING = false;


/* 
   for loading preloops (presets) to DRUMS array 
*/

void setPreset(bool preset[PATTERNS][8][16]){
  for(uint8_t pat = 0; pat < PATTERNS; ++pat){
    for(uint8_t y = 0; y < 8; ++y){
      for(uint8_t x = 0; x < 16; ++x){
        DRUMS[pat][y][x] = preset[pat][y][x];
      }
    }
  }
}

/*
   initialization drum array volumes
*/
void initVol(){
  // sets volume for all drums
  for(uint8_t pat = 0; pat < 4; ++pat){
    for(uint8_t i = 0; i < 16; ++i){
      DRUMSVOL[pat][i] = 30;
    }
  }
}


