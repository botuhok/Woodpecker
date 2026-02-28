/*
TODO:
 - is it possible change UART speed to 115200?      // Maybe use DFPlayer Pro? https://wiki.dfrobot.com/DFPlayer_PRO_SKU_DFR0768
 - add animation in startup function
 - adjust volume on samples and may be choose better samples
 - False set drums when use combination keys ! 
*/


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"  
#include <stdbool.h>
#include "max.h"
#include "buttons.h"
#include <driver/i2c_master.h>
#include "esp_random.h"
#include "esp_ssd1306.h"
#include "serial.h"
#include "esp_log.h"
#include "esp_sleep.h"
#define TAG "WOODPECKER"

// startup logo
static const uint8_t wood[512] = 
{0,48,208,16,16,16,16,16,16,16,16,16,208,48,0,0,0,0,128,96,16,16,16,16,16,16,16,16,16,16,240,0,0,0,128,96,27,5,1,129,97,25,5,3,1,192,48,8,4,2,2,1,193,49,1,9,9,9,201,49,1,1,1,1,3,198,60,0,0,0,192,48,8,4,2,2,1,193,49,1,9,9,9,201,49,1,1,1,1,3,198,60,0,0,0,192,48,13,3,1,1,1,1,193,49,9,9,9,9,241,1,1,1,1,3,6,4,8,240,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,0,0,0,255,0,128,96,24,6,129,96,24,4,254,0,0,0,0,0,0,0,255,96,24,6,1,128,96,24,6,1,0,0,112,140,3,0,0,0,0,48,76,67,64,64,0,48,12,3,0,0,128,64,48,12,3,0,0,112,140,3,0,0,0,0,48,76,67,64,64,0,48,12,3,0,0,128,64,48,12,3,0,192,48,12,3,0,0,0,0,64,112,76,67,64,64,32,16,12,3,0,0,0,0,128,64,48,12,3,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,0,0,0,15,6,129,96,24,6,1,0,0,0,255,0,128,64,64,32,32,16,16,8,8,4,6,1,0,0,0,0,0,0,0,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,0,0,0,0,0,0,0,0,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,0,0,0,2,2,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,128,64,64,32,32,16,16,8,8,4,6,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};

static const uint8_t pecker[86] =
{0,1,3,255,255,0,255,33,33,33,33,51,30,30,12,0,0,60,126,126,219,145,145,145,145,147,90,78,12,0,0,60,126,126,195,129,129,129,129,129,66,102,36,0,129,255,255,255,24,60,60,102,231,195,129,129,0,0,60,126,126,219,145,145,145,145,147,90,78,12,0,129,255,255,0,255,33,33,33,97,97,243,158,158,140,0,};

// animation when playing

static const uint8_t woodpeckerFrame1[128] =
{0,4,4,12,92,252,252,252,60,62,30,30,14,14,14,12,28,24,120,240,128,0,0,0,0,0,0,0,64,0,32,0,0,0,0,0,0,6,15,48,192,128,0,0,0,0,0,1,3,0,128,192,193,33,33,34,34,34,34,20,20,148,9,40,0,0,0,128,224,120,28,7,11,9,17,48,224,193,1,0,0,0,7,31,248,224,128,0,0,2,0,0,2,0,0,0,224,120,30,7,1,128,64,0,0,128,64,32,24,15,7,0,0,128,192,96,63,31,7,0,0,0,0,0,0,0,0,0,};

static const uint8_t woodpeckerFrame2[128] =
{4,12,28,60,252,252,62,62,30,30,14,14,12,12,28,24,112,224,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,31,48,64,128,128,0,0,0,2,7,2,0,128,64,33,33,35,34,34,34,20,20,20,8,8,0,0,0,0,0,0,0,224,120,6,1,8,8,8,17,17,226,194,128,0,1,6,30,120,224,128,0,0,0,0,0,0,0,0,0,0,224,56,15,7,0,0,128,64,48,128,64,32,24,15,7,0,128,192,96,56,31,7,0,0,0,0,0,0,0,0,0,};

bool frame = true;                      // for switching between woodpeckerFrame1 and woodpeckerFrame2

TaskHandle_t Buttons = NULL;
TaskHandle_t Blink = NULL;
TaskHandle_t Oled = NULL;
TaskHandle_t Drums = NULL;
TaskHandle_t Play = NULL;
TaskHandle_t Sleep = NULL;

/*
   I2C Master Configuration for oled screen
*/
static i2c_master_bus_config_t i2c_master_bus_config = {
      .i2c_port = I2C_NUM_0,
      .scl_io_num = SCL_PIN,
      .sda_io_num = SDA_PIN,
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .glitch_ignore_cnt = 7,
      .flags.enable_internal_pullup = true};
static i2c_master_bus_handle_t i2c_master_bus;
static i2c_ssd1306_config_t i2c_ssd1306_config = {
    .i2c_device_address = 0x3C,
    .i2c_scl_speed_hz = 400000,
    .width = 128,
    .height = 32,
    .wise = SSD1306_BOTTOM_TO_TOP};
static i2c_ssd1306_handle_t i2c_ssd1306;

/* Blinking cursor on MAX7219 led */
void CursorBlink(void *arg){
  ESP_LOGI(TAG, "Task %s stack high water mark: %u", pcTaskGetName(NULL), uxTaskGetStackHighWaterMark(NULL));
  initMax7219();
  while(1){
    MAX_STATE = !MAX_STATE;
    if(CURSOR_MOVE){
      CURSOR_MOVE = false;
      if(!DRUMS[PATTERN][PRECOORDS[1]][PRECOORDS[0]])
        setPixel(PRECOORDS[0], PRECOORDS[1], false, true);
      else setPixel(PRECOORDS[0], PRECOORDS[1], true, true);
    }
    setPixel(COORDS[0], COORDS[1], MAX_STATE, true);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

/* blinking playing column on MAX7219 led*/
void blinkPlayingHit(uint8_t* preX, uint8_t* x){
  if(!DRUMS[PATTERN][0][*preX]){
    setPixel(*preX, 0, false, false);         // turn on led of previous playing drum
  }
  setPixel(*x, 0, true, true);                // turn off led of now playing drum
  *preX = *x;
}

/* SENDING VOLUME VIA UART */
void sendVolume(){     
  // if global changing volume
  if(PREVIOUSVOLUME != CURRENTVOLUME) {
    PREVIOUSVOLUME = CURRENTVOLUME;
    setVolume(DRUMSVOL[PATTERN][NOW_PLAYING] - (MAXVOLUME - CURRENTVOLUME));
  }

  // if last drum volume != first drum volume
  else if(NOW_PLAYING == 0 && DRUMSVOL[PATTERN][0] != DRUMSVOL[PATTERN][15])
    setVolume(DRUMSVOL[PATTERN][NOW_PLAYING] - (MAXVOLUME - CURRENTVOLUME));   // sets volume of current column

  // if previous drum volume != current drum volume
  else if(NOW_PLAYING > 0 && DRUMSVOL[PATTERN][NOW_PLAYING] != DRUMSVOL[PATTERN][NOW_PLAYING - 1])
    setVolume(DRUMSVOL[PATTERN][NOW_PLAYING] - (MAXVOLUME - CURRENTVOLUME));   // sets volume of current column
}

/* draw drums array on MAX7219 led matrix */
void DrawDrums(void *arg){
  ESP_LOGI(TAG, "Task %s stack high water mark: %u", pcTaskGetName(NULL), uxTaskGetStackHighWaterMark(NULL));

  while(1){
    if(NEED_REFRESH_MAX){
      for(uint8_t y = 0; y < 8; ++y){
        for(uint8_t x = 0; x < 16; ++x){
          if(DRUMS[PATTERN][y][x] == true) setPixel(x, y, true, true);
          else setPixel(x, y, false, true);
        }
      }
      // REFRESH MAX7219
      if(DRUMS[PATTERN][7][15]) setPixel(0, 0, true, true);
      else setPixel(15, 7, false, true);

      NEED_REFRESH_MAX = false;
    }
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

/*
   checking activity and if not after SLEEPAFTER min - then turn off all
 */
void sleepTask(void *arg){
  uint32_t sleepTimeout = SLEEPAFTER * 60 * 1000;
  uint32_t countdownTimer = sleepTimeout;

  while(1){
    vTaskDelay(pdMS_TO_TICKS(1000));                 // check every 1 sec

    if(ANYPRESSED || PLAY)
      countdownTimer = sleepTimeout;                // if any pressed, wait 

    else if(countdownTimer > 1000){                 // else - decrease timer
      countdownTimer -= 1000;
      if(countdownTimer < 5000){
        i2c_ssd1306_buffer_fill(&i2c_ssd1306, false);
        i2c_ssd1306_buffer_text(&i2c_ssd1306, 0, 16, "Sleep in ", false);
        i2c_ssd1306_buffer_int(&i2c_ssd1306, 70, 16, (int) countdownTimer / (int)1000, false);
        i2c_ssd1306_buffer_to_ram(&i2c_ssd1306);
      }
    }
  
    else {
      sendToChain(0x0C, 0x00, 0x00);
      vTaskDelay(pdMS_TO_TICKS(100));
      i2c_ssd1306_buffer_fill(&i2c_ssd1306, false);
      i2c_ssd1306_buffer_to_ram(&i2c_ssd1306);
      uint8_t PINS[12] = {TX_PIN, RX_PIN, ENTER_PIN, UP_PIN, DOWN_PIN, LEFT_PIN, 
                          RIGHT_PIN, DIN_PIN, CLK_PIN, CS_PIN, SDA_PIN, SCL_PIN};
      for (uint8_t gpio = 0; gpio < 12; gpio++)
        gpio_reset_pin(PINS[gpio]);
      uart_driver_delete(UART_NUM);
      esp_deep_sleep_start();
    }
  }
}


/*
   init oled1302 and showing startup text
*/
void startupText(){
  i2c_new_master_bus(&i2c_master_bus_config, &i2c_master_bus);
  i2c_ssd1306_init(i2c_master_bus, &i2c_ssd1306_config, &i2c_ssd1306);
  i2c_ssd1306_buffer_fill(&i2c_ssd1306, false);
  // show animation

  // show Wood
  for(uint8_t x = 90; x > 0; x-=10){
    vTaskDelay(pdMS_TO_TICKS(20));
    i2c_ssd1306_buffer_fill(&i2c_ssd1306, false);
    i2c_ssd1306_buffer_image(&i2c_ssd1306, x, 0, (const uint8_t *)wood, 128, 32, false);
    i2c_ssd1306_buffer_to_ram(&i2c_ssd1306);
  }
  for(uint8_t x = 0; x < 8; ++x){
    vTaskDelay(pdMS_TO_TICKS(50));
    i2c_ssd1306_buffer_fill(&i2c_ssd1306, false);
    i2c_ssd1306_buffer_image(&i2c_ssd1306, x, 0, (const uint8_t *)wood, 128, 32, false);
    i2c_ssd1306_buffer_to_ram(&i2c_ssd1306);
  }

  // show pecker
  vTaskDelay(pdMS_TO_TICKS(500));

  i2c_ssd1306_buffer_image(&i2c_ssd1306, 41, 21, (const uint8_t *)pecker, 86, 8, false);
  i2c_ssd1306_buffer_to_ram(&i2c_ssd1306);
}

/*
   draw volumes for columns when not in menu
*/
void drawVolumes(){
  i2c_ssd1306_buffer_fill(&i2c_ssd1306, false);
  
  // woodpecker animation when playing
  if(frame) i2c_ssd1306_buffer_image(&i2c_ssd1306, 0, 0, (const uint8_t *)woodpeckerFrame1, 32, 32, false);
  else i2c_ssd1306_buffer_image(&i2c_ssd1306, 0, 0, (const uint8_t *)woodpeckerFrame2, 32, 32, false);

  // show current pattern and current column volume
  i2c_ssd1306_buffer_text(&i2c_ssd1306, 106, 0, "P", false);
  i2c_ssd1306_buffer_int(&i2c_ssd1306, 114, 0, PATTERN, false);                               // show volume for current column
  i2c_ssd1306_buffer_int(&i2c_ssd1306, 106, 24, DRUMSVOL[PATTERN][COORDS[0]], false);         // show volume for current column

  for(uint8_t x = 0; x < 16; ++x){
    for(uint8_t y = 0; y < DRUMSVOL[PATTERN][x] - 4; ++y) {                                   // draw volume for each column
      i2c_ssd1306_buffer_fill_pixel(&i2c_ssd1306, 36 + x * 4, 28 - y, true);  
    }
  }
        
  // draw cursor on oled 1306
  i2c_ssd1306_buffer_fill_pixel(&i2c_ssd1306, 36 + PRECOORDS[0] * 4, 30, false);
  i2c_ssd1306_buffer_fill_pixel(&i2c_ssd1306, 36 + PRECOORDS[0] * 4, 31, false);
  i2c_ssd1306_buffer_fill_pixel(&i2c_ssd1306, 36 + COORDS[0] * 4, 30, true);
  i2c_ssd1306_buffer_fill_pixel(&i2c_ssd1306, 36 + COORDS[0] * 4, 31, true);
       
  // sending buffer to screen
  i2c_ssd1306_buffer_to_ram(&i2c_ssd1306);
}

/*
  draw menu on oled display
*/
void drawMenu(){
  MENUVALUES[4] = PATTERN;                         // update what pattern playing
  i2c_ssd1306_buffer_fill_space(&i2c_ssd1306, 0, 127, 0, 31, false);
  // menu first page
  if(MENUINDEX < 4){
    for(uint8_t i = 0; i < 4; ++i){
      if(MENUINDEX == i)
        i2c_ssd1306_buffer_text(&i2c_ssd1306, 0, i * 8, ">", false);
      i2c_ssd1306_buffer_text(&i2c_ssd1306, 10, i * 8, MENUITEMS[i], false);
      i2c_ssd1306_buffer_int(&i2c_ssd1306, 80, i * 8, MENUVALUES[i], false);
    }
  }
  
  //menu second page
  else if(MENUINDEX > 3){
    for(uint8_t i = 4; i < 8; ++i){
      if(MENUINDEX == i)
        i2c_ssd1306_buffer_text(&i2c_ssd1306, 0, (i - 4) * 8, ">", false);
      i2c_ssd1306_buffer_text(&i2c_ssd1306, 10, (i - 4) * 8, MENUITEMS[i], false);
      i2c_ssd1306_buffer_int(&i2c_ssd1306, 80, (i - 4) * 8, MENUVALUES[i], false);
    }
  }
  i2c_ssd1306_buffer_to_ram(&i2c_ssd1306);
}

/*
   Draw menu or volume sets for drum columns on OLED 1306
*/
void OledTask(void *arg){
  ESP_LOGI(TAG, "Task %s stack high water mark: %u", pcTaskGetName(NULL), uxTaskGetStackHighWaterMark(NULL));
  startupText();
  while(1){
    if(NEED_REFRESH_OLED){
      NEED_REFRESH_OLED = false;
      // draw menu
      if(MENU) drawMenu();
      else drawVolumes();
    }
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}


/* 
   This function initializate UART once,
   sets startup drum preset (via setPreset function)
   and sending signals to mp3 player when device start playing (PLAY = true)
   DRUMS[PATTERN][y][x]:
       PATTERN - current pattern (selected in menu)
       y - sets which drum [0 - 7]
       x - sets which column
*/
void play(void *arg){
  ESP_LOGI(TAG, "Task %s stack high water mark: %u", pcTaskGetName(NULL), uxTaskGetStackHighWaterMark(NULL));
  initUart(9600, TX_PIN, RX_PIN);
  setVolume(CURRENTVOLUME);                              // global sets volume
  initVol();                                             // sets default volumes for drum columns
  setPreset(PRESETS[0]);                                 // sets default drum preset
  uint8_t preX = 0;

  while(1){
    if(PLAY){
      for(uint8_t pat = 0; pat < PATTERNS; ++pat){
        PATTERN = pat;
        if(!PLAY) break;                                   // stop if PLAY state changed when playing

        for(uint8_t x = 0; x < 16; ++x) {
          if(x % 4 == 0){                                  // update oled for woodpecker animation when playing
            NEED_REFRESH_OLED = true;
            frame = !frame;                                // switch woodpeckerFrame1 and woodpeckerFrame2
          }
          NOW_PLAYING = x;                                 // save for column playing to global for other functions
          blinkPlayingHit(&preX, &x);

          if(x % 2 && MENUVALUES[2] != 0)                  // add shuffle
            vTaskDelay(pdMS_TO_TICKS(MENUVALUES[2] * 2));     

          for(uint8_t y = 0; y < 8; ++y){
            if(DRUMS[pat][y][x]){
              sendVolume();            
              playMp3(MENUVALUES[1]+1, y+1);           // sending tx message to mp3player (#preset, #sample)
              continue;                                // playing ONLY ONE DRUM per column 
                                                      // (DFPlayer can't play more drums in one time)
            }
          }
          
          vTaskDelay(pdMS_TO_TICKS(BPM_PAUSE));        // pause between hits (for bpm regulation)
        }
        NEED_REFRESH_MAX = true;                       // we need refresh drums on MAX7219 when PATTERN change
        NEED_REFRESH_OLED = true;                      // also we need refresh oled info when PATTERN change
      }
    }
    else vTaskDelay(pdMS_TO_TICKS(500));              // delay between checking PLAY state
  }
}


void app_main(){
  BaseType_t ret;

  ret = xTaskCreate(checkButtons, "Check Buttons", 4096, NULL, 4, &Buttons);
  if (ret != pdPASS) {
    ESP_LOGE(TAG, "Failed to create checkButtons task");
  }

  ret = xTaskCreate(CursorBlink, "Blink Cursor", 2048, NULL, 3, &Blink);
  if (ret != pdPASS) {
    ESP_LOGE(TAG, "Failed to create CursorBlink task");
  }

  ret = xTaskCreate(DrawDrums, "Draw drums on MAX7219", 2048, NULL, 1, &Drums);
  if (ret != pdPASS) {
    ESP_LOGE(TAG, "Failed to create DrawDrums task");
  }

  ret = xTaskCreate(OledTask, "Draw info on OLED 1306", 4096, NULL, 2, &Oled);
  if (ret != pdPASS) {
    ESP_LOGE(TAG, "Failed to create OledTask task");
  }

  ret = xTaskCreate(play, "Play drums via mp3 player", 4096, NULL, 5, &Play);
  if (ret != pdPASS) {
    ESP_LOGE(TAG, "Failed to create play task");
  }
  if(SLEEPMODE){                                    // if SLEEPMODE activated - sleeping after SLEEPAFTER min
    ret = xTaskCreate(sleepTask, "Sleep timer", 4096, NULL, 1, &Sleep);
    if (ret != pdPASS) {
      ESP_LOGE(TAG, "Failed to create Sleep timer task");
    }
  }
}
