#include "buttons.h"
#include "esp_log.h"
#include "globals.h"

#define TAG "DRUM_MACHINE"


uint8_t BUTTONS[] = {ENTER_PIN, UP_PIN, DOWN_PIN, LEFT_PIN, RIGHT_PIN}; // just pins list for cycle checking
bool STATES[5];                                                         // Saving button states
uint16_t DEBOUNCE_DELAY = 100;                                          // Time (ms) between button press checks
uint16_t COMBINATION_DELAY = 600;                                       // ms delay after combination (enter + up, enter + down) 
bool COMBINATION = false; // sets true if combination of keys was pressed 
                          // (Enter + up, Enter + down, Enter + left, Enter + right)

/* 
  if MENU = false
  checking and processing buttons
  buttons move cursor and Enter set/unset drum hit
*/
void notInMenu(){
  if(!STATES[0]){                           // pressed Enter and set/unset drum
    DRUMS[PATTERN][COORDS[1]][COORDS[0]] = !DRUMS[PATTERN][COORDS[1]][COORDS[0]];

    if(DRUMS[PATTERN][COORDS[1]][COORDS[0]])
      playMp3(MENUVALUES[1]+1, COORDS[1]+1);     // play mp3 when drum set
    NEED_REFRESH_MAX = true;
  }
  
  // Move cursor
  else {
    CURSOR_MOVE = true;
    if(PRECOORDS[0] != COORDS[0])
      NEED_REFRESH_OLED = true;                  // change info about volume of drum column only if cursor moving
                                                 // horizontal
    PRECOORDS[0] = COORDS[0];
    PRECOORDS[1] = COORDS[1];
    if(!STATES[3]){                              // pressed left, move cursor left
      if(COORDS[0] > COORDSX_MIN) COORDS[0]--;

      else {                                     // change to previous pattern or stop moving left
        if(PATTERN > 0){
          --PATTERN;
          COORDS[0] = COORDSX_MAX;
          MENUVALUES[4] = PATTERN;
          NEED_REFRESH_MAX = true;
          NEED_REFRESH_OLED = true;
        }
        else {                                  // change PATTERN to last one when cursor moved to left edge
          PATTERN = 3;
          COORDS[0] = COORDSX_MAX;
          MENUVALUES[4] = PATTERN;
          NEED_REFRESH_MAX = true;
          NEED_REFRESH_OLED = true;
        }
      }
    }
    else if(!STATES[4]){                       // pressed right; move cursor right
      if(COORDS[0] < COORDSX_MAX)
        COORDS[0]++;

      else {                                   // change to next pattern
        if(PATTERN < PATTERNS - 1){
          ++PATTERN;
          COORDS[0] = COORDSX_MIN;
          MENUVALUES[4] = PATTERN;
          NEED_REFRESH_MAX = true;
          NEED_REFRESH_OLED = true;
        }
        else {                                 // change PATTERN to begin when cursor moved right edge of PATTERN 3
          PATTERN = 0;
          COORDS[0] = COORDSX_MIN;
          MENUVALUES[4] = PATTERN;
          NEED_REFRESH_MAX = true;
          NEED_REFRESH_OLED = true;

        }
      }
    }
    else if(!STATES[2]){                        // pressed down, move cursor down
      if(COORDS[1] < COORDSY_MAX) COORDS[1]++;
      else COORDS[1] = COORDSY_MIN;
    }
    else if(!STATES[1]){                        // pressed up, move cursor up
      if(COORDS[1] > COORDSY_MIN) COORDS[1]--;
      else COORDS[1] = COORDSY_MAX;
    }
  }
}

/*
   if MENU = true
   check and process buttons 
   up, down - move menu lines 
   left, right - change menu values
*/
void inMenu(){
  // pressed LEFT
  if(!STATES[3]){    
    switch(MENUINDEX){
      case 0: if(MENUVALUES[0] > BPMMIN) MENUVALUES[0]--;   // decrease BPM
              BPM_PAUSE = 60000 / MENUVALUES[0] / 4;
              break;
      case 1: if(MENUVALUES[1] > 0) MENUVALUES[1]--;         // decrease sound kit preset
              break;
      case 2: if(MENUVALUES[2] > 0) MENUVALUES[2]--;         // decrease shuffle
              break;
      case 3: if(MENUVALUES[3] > 0){                         // decrease global volume
                MENUVALUES[3]--;
                CURRENTVOLUME = MENUVALUES[3];
              }
              break;
      case 4: if(MENUVALUES[4] > 0){                         // change current pattern
                MENUVALUES[4]--;
                NEED_REFRESH_MAX = true;                     // change pattern, so need redraw drums
                PATTERN = MENUVALUES[4];     
              }    
              break;
      case 5: if(MENUVALUES[5] > 0){                         // sets DRUMS PRESETS
                MENUVALUES[5]--;
                setPreset(PRESETS[MENUVALUES[5]]);           // set DRUMS array = PRESETS number
                NEED_REFRESH_MAX = true;
                NEED_REFRESH_OLED = true;
                
              }
              break;
      case 6:
              break;
      case 7:
              break;
    }
  }

  // pressed RIGHT;
  else if(!STATES[4]){ 
    switch(MENUINDEX){ 
      case 0: if(MENUVALUES[0] < BPMMAX) MENUVALUES[0]++;       // increase BPM
              BPM_PAUSE = 60000 / MENUVALUES[0] / 4;
              break;
      case 1: if(MENUVALUES[1] < KITSMAX) MENUVALUES[1]++;    // increase preset
              break;
      case 2: if(MENUVALUES[2] < SHUFFLEMAX) MENUVALUES[2]++;    // increase shuffle
              break;
      case 3: if(MENUVALUES[3] < MAXVOLUME){                     // increase global volume
                MENUVALUES[3]++;
                CURRENTVOLUME = MENUVALUES[3];
              }
              break;
      case 4: if(MENUVALUES[4] < PATTERNS - 1){                  // increase pattern num
                MENUVALUES[4]++;
                NEED_REFRESH_MAX = true;                         // change pattern, so need redraw drums
                PATTERN = MENUVALUES[4];
              }      
              break;
      case 5: if(MENUVALUES[5] < PRESETSMAX-1){                  // increase preset number
                MENUVALUES[5]++;
                NEED_REFRESH_MAX = true;
                NEED_REFRESH_OLED = true;
                setPreset(PRESETS[MENUVALUES[5]]);             // sets DRUMS array = current preset
              }
              break;
      case 6:
              break;
      case 7:
              break;
    }
  }
  // pressed DOWN (change menu index)
  else if(!STATES[2]){   
    if(MENUINDEX < 7) MENUINDEX++;
  }
  // pressed UP (change menu index)
  else if(!STATES[1]){
    if(MENUINDEX > 0) MENUINDEX--;
  }
  NEED_REFRESH_OLED = true;                     //  refreshing oled screen
}

/*
   endless cycle for checking buttons and run
   notInMenu()        moving cursor on MAX7219 leds and sets drum hit
   inMenu()           moving between menu lines and change menu settings

   all ping in PULLUP mode so if
   pin state == 0 - button pressed
*/
void checkButtons(void *arg){
  ESP_LOGI(TAG, "Task %s stack high water mark: %u", pcTaskGetName(NULL), uxTaskGetStackHighWaterMark(NULL));
  // Инициализация пинов под кнопки 
  for(uint8_t i = 0; i < 5; ++i){
    esp_rom_gpio_pad_select_gpio(BUTTONS[i]);          // change pin to GPIO mode
    gpio_set_direction(BUTTONS[i], GPIO_MODE_INPUT);   // change pin to INPUT
    gpio_pullup_en(BUTTONS[i]);                        // sets PULLUP (0 = button pressed)
  }
  while(1){
    ANYPRESSED = false;                                // sets to true if any button was pressed
    COMBINATION = false;                               // sets to true if was pressed two buttons in one time

    for(uint8_t i = 0; i < 5; ++i){                    // sets all buttons states to "not pressed"
      STATES[i] = 1;
    }
    // fill STATES array of buttons  
    for(uint8_t i = 0; i < 5; ++i){
      if (gpio_get_level(BUTTONS[i]) == 0){                       // if some button was pressed
        MENUVALUES[4] = PATTERN;                                  // send current pattern to menu   
        STATES[i] = 0;                                            // sets this button state to "pressed"
        ANYPRESSED = true;
        vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_DELAY));                // delay between checking pins 
      }
     // else STATES[i] = 1;                                       // not pressed
    }

    if(ANYPRESSED){                                               // some button pressed, checking what button
      // Checking for combination
      if(!STATES[0]){                                             // pressed ENTER
        if(!STATES[2]){                                           // ENTER + DOWN (enter in menu)
          COMBINATION = true;
          MENU = !MENU;
        }
        else if(!STATES[1]){                                      // ENTER + UP (Start/Stop playing)
          COMBINATION = true;
          PLAY = !PLAY;
          if(PLAY){
            STARTPLAYING = true;                                 // for refreshing OLED
           // MENU = false;                                      // exit from menu when start playing
          }
        }
        else if(!STATES[3]){                                    // ENTER + LEFT (decrease volume of drum column)
          COMBINATION = true;
          if(DRUMSVOL[PATTERN][COORDS[0]] > 2) DRUMSVOL[PATTERN][COORDS[0]] -= 2;
        }
        else if(!STATES[4]) {                                   // ENTER + RIGHT (increase volume of drum column)
          COMBINATION = true;
          if(DRUMSVOL[PATTERN][COORDS[0]] < 30) DRUMSVOL[PATTERN][COORDS[0]] += 2;
        }
      }

      if(COMBINATION){                                         // for all combination
        NEED_REFRESH_OLED = true;
        vTaskDelay(pdMS_TO_TICKS(COMBINATION_DELAY));          // combination delay
        continue;                                              // do not check other keys
      }

      // checking single pressing
      if(!COMBINATION){
        if(!MENU) notInMenu();                                 // moving cursor and set/unset drum
        else inMenu();                                         // moving between menu lines
      }
    }
    vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_DELAY));                 // delay between checking pins        
  }
}