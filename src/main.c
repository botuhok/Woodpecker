/*
TODO:
 - is it possible change UART speed to 115200?      // Maybe use DFPlayer Pro? https://wiki.dfrobot.com/DFPlayer_PRO_SKU_DFR0768
 - add animation in startup function
 - adjust volume on samples and may be choose better samples
 - False set drums when use combination keys ! 

drumMachine v0.4
  -- added play drum when drum hit set by enter key
  -- added empty preset
  -- renamed "Sound" to "Kit" in menu

drumMachine v0.3
  -- added presets, now you can choose it in menu!
     MENUVALUES[5] = index in PRESETS array
  -- added change pattern to 0 or to PATTERNS (max pattern number) when cursor moving on left/right edge
  -- added vTaskDelay if for cycle when processed buttons, but Enter still sometimes set/unset drum when you go to menu or start playing.

drumMachine v0.2
  -- added patterns (global PATTERNS = 4), which change in menu or moving cursor on left/right edge.
     PATTERN = MENUVALUES[4]
     MENUVALUES[4] = PATTERN
*/


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"  
#include <stdbool.h>
#include "max.h"
#include "buttons.h"
#include <driver/i2c_master.h>
#include "esp_ssd1306.h"
#include "serial.h"
#include "esp_log.h"
#include "esp_sleep.h"
#define TAG "DRUM_MACHINE"

static const uint8_t logo[4][32] = {
    {0x00, 0x00, 0x00, 0xC0, 0x60, 0x18, 0x00, 0x00, 0x70, 0x78, 0x78, 0x78, 0xF8, 0xF8, 0xF0, 0xF0,
     0xF2, 0xE6, 0xE6, 0xCE, 0x9E, 0x9C, 0x3C, 0x78, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00},
    {0x00, 0xFC, 0x07, 0x60, 0xF8, 0xFC, 0xFE, 0xFE, 0x9E, 0x9E, 0x9E, 0x3E, 0x3E, 0x7C, 0x7C, 0xF9,
     0xF9, 0xF3, 0xE7, 0xCF, 0x9F, 0x3F, 0x7F, 0xFE, 0xFC, 0xF1, 0xE3, 0x8F, 0x1F, 0xFE, 0xF8, 0x00},
    {0x00, 0x07, 0x3C, 0xE0, 0x81, 0x03, 0x07, 0xC7, 0xE7, 0xC7, 0xCF, 0x1F, 0x7F, 0xFE, 0xFC, 0xF8,
     0xE1, 0x07, 0x3F, 0xFF, 0xFF, 0xFE, 0xF0, 0x01, 0x0F, 0xFF, 0xFF, 0xFF, 0x3C, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x06, 0x0D, 0x19, 0x11, 0x30, 0x20, 0x24, 0x4F, 0x4F, 0x4F,
     0x4F, 0x40, 0x40, 0x4F, 0x4F, 0x6F, 0x27, 0x20, 0x10, 0x10, 0x08, 0x0C, 0x04, 0x00, 0x00, 0x00},
};

static const uint8_t lcd_help[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 5, 131, 64, 47, 40, 32, 76, 135, 5, 15, 0, 1, 14, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 223, 21, 21, 10, 0, 15, 208, 16, 223, 0, 1, 223, 1, 192, 65, 95, 65, 0, 206, 145, 17, 206, 0, 31, 2, 4, 31, 192, 82, 85, 149, 9, 192, 64,
64, 128, 0, 223, 69, 65, 65, 0, 143, 80, 80, 95, 0, 159, 66, 68, 95, 0, 206, 81, 81, 81, 0, 192, 64, 64, 128, 0, 0, 0, 0, 3, 132, 72, 36, 35, 32, 66, 130, 2, 0, 0, 0, 3, 4,
8, 8, 8, 4, 3, 0, 0, 0, 0, 0, 128, 64, 32, 32, 35, 68, 136, 4, 3, 0, 0, 2, 7, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 241, 82, 20, 23,
4, 242, 33, 64, 247, 1, 1, 7, 0, 7, 5, 245, 68, 160, 23, 0, 241, 87, 80, 16, 0, 16, 32, 199, 32, 18, 1, 0, 7, 2, 2, 5, 0, 7, 5, 5, 4, 0, 4, 5, 5, 2, 0, 4, 5, 5, 2, 0, 7, 5,
5, 4, 0, 7, 4, 4, 3, 0, 0, 0, 0, 0, 3, 4, 8, 8, 8, 4, 3, 0, 0, 0, 0, 192, 32, 16, 16, 16, 32, 192, 0, 0, 0, 0, 0, 3, 4, 8, 8, 8, 4, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 34, 65, 65, 65, 34, 28, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 240, 32, 64, 32, 240, 1, 242, 180, 148, 4, 242, 33, 64, 240, 0, 112, 128, 128, 240, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 5, 1, 0, 15, 2, 4, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

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
  // uint32_t sleepTimeout = 30000;
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

  // show version number
  for(uint8_t i = 130; i > 20; i -= 10){
    i2c_ssd1306_buffer_fill_space(&i2c_ssd1306, 0, 127, 0, 31, false);
    i2c_ssd1306_buffer_text(&i2c_ssd1306, i, 0, "drumMachine", false);
    i2c_ssd1306_buffer_text(&i2c_ssd1306, 0, 16, VERSION, false);
    i2c_ssd1306_buffer_to_ram(&i2c_ssd1306);
    vTaskDelay(pdMS_TO_TICKS(200));
  }

  vTaskDelay(pdMS_TO_TICKS(2000));                     // delay after startup animation before showing help text

  // show keys help
  i2c_ssd1306_buffer_fill(&i2c_ssd1306, false);
  i2c_ssd1306_buffer_image(&i2c_ssd1306, 0, 0, lcd_help, 128, 32, false);
  
  
  //for(uint8_t i = 0; i < 4; ++i){
  //  i2c_ssd1306_buffer_text(&i2c_ssd1306, 0, i * 8, HELPTEXT[i], false);
  //}
  i2c_ssd1306_buffer_to_ram(&i2c_ssd1306); 
}

/*
   draw volumes for columns when not in menu
*/
void drawVolumes(){
  i2c_ssd1306_buffer_fill(&i2c_ssd1306, false);
  i2c_ssd1306_buffer_image(&i2c_ssd1306, 0, 0, (const uint8_t *)logo, 32, 32, false);
  i2c_ssd1306_buffer_text(&i2c_ssd1306, 106, 0, "P", false);
  i2c_ssd1306_buffer_int(&i2c_ssd1306, 114, 0, PATTERN, false);                               // show volume for current column
  i2c_ssd1306_buffer_int(&i2c_ssd1306, 106, 24, DRUMSVOL[PATTERN][COORDS[0]], false);         // show volume for current column

  for(uint8_t x = 0; x < 16; ++x){
    for(uint8_t y = 0; y < DRUMSVOL[PATTERN][x] - 4; ++y) {                                   // draw volume for each column
      i2c_ssd1306_buffer_fill_pixel(&i2c_ssd1306, 40 + x * 4, 28 - y, true);  
    }
  }
        
  // draw cursor on oled 1306
  i2c_ssd1306_buffer_fill_pixel(&i2c_ssd1306, 40 + PRECOORDS[0] * 4, 30, false);
  i2c_ssd1306_buffer_fill_pixel(&i2c_ssd1306, 40 + PRECOORDS[0] * 4, 31, false);
  i2c_ssd1306_buffer_fill_pixel(&i2c_ssd1306, 40 + COORDS[0] * 4, 30, true);
  i2c_ssd1306_buffer_fill_pixel(&i2c_ssd1306, 40 + COORDS[0] * 4, 31, true);
       
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
