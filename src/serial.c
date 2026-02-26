#include "serial.h"

void initUart(uint32_t speed, uint8_t tx, uint8_t rx) {
    const uart_config_t uart_config = {
        .baud_rate = speed,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, tx, rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}


// Function to send a command to the MP3-TF-16P module
void playMp3(uint8_t param1, uint8_t param2) {
  uint8_t commandBuffer[8] = {
    0x7E,           // Start byte
    0xFF,           // Version byte
    0x06,           // Command length
    0x0F,        // Command 0x0F for playing
    0x00,           // Feedback (0x00 for no feedback)
    param1,         // folder (preset) name
    param2,         // file mane
    0xEF            // End byte
  };

  // Send the command to the MP3 module
  for (uint8_t i = 0; i < 8; i++)
    uart_write_bytes(UART_NUM_1, &commandBuffer[i], sizeof(commandBuffer[i]));
}
void setVolume(uint8_t volume){                                                  // set volume of column (0x06)
  uint8_t command[8] = {0x7E, 0xFF, 0x06, 0x06, 0x00, 0x00, volume, 0xEF};
  for (uint8_t i = 0; i < 8; i++) {
    uart_write_bytes(UART_NUM, &command[i], sizeof(command[i]));
  }
}
/*
// Function for increase volume by 1
void incGlobalVolume(){
  uint8_t command[8] = {0x7E, 0xFF, 0x06, 0x04, 0x00, 0x00, 0x00, 0xEF};   // increase volume command (0x04)
  for (int i = 0; i < 8; i++) {
    uart_write_bytes(UART_NUM, &command[i], sizeof(command[i]));
  }
}
// Function for decrease volume by 1 
void decGlobalVolume(){
  uint8_t command[8] = {0x7E, 0xFF, 0x06, 0x05, 0x00, 0x00, 0x00, 0xEF};   // decrease volume command (0x05)
  for (int i = 0; i < 8; i++) {
    uart_write_bytes(UART_NUM, &command[i], sizeof(command[i]));
  }
}
*/
