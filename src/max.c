#include "max.h"

// Буферы для двух матриц: left (x=0-7, дальняя в цепочке), right (x=8-15, ближняя)
unsigned short leftBuffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};  // Дальняя матрица
unsigned short rightBuffer[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // Ближняя матрица

// Функция отправки команды в цепочку (для двух матриц: сначала far, потом near)
void sendToChain(unsigned int address, unsigned int dataFar, unsigned int dataNear) {
  gpio_set_level(CS_PIN, 0); // CS low для начала

  
  // Отправляем address + dataFar (для дальней матрицы, MSB first)
  for (int i = 7; i >= 0; i--) {
    gpio_set_level(CLK_PIN, 0);
    gpio_set_level(DIN_PIN, (address & (1 << i)) ? 1 : 0);
    gpio_set_level(CLK_PIN, 1);
  }
  for (int i = 7; i >= 0; i--) {
    gpio_set_level(CLK_PIN, 0);
    gpio_set_level(DIN_PIN, (dataFar & (1 << i)) ? 1 : 0);
    gpio_set_level(CLK_PIN, 1);
  }
  
  // Отправляем address + dataNear (для ближней матрицы)
  for (int i = 7; i >= 0; i--) {
    gpio_set_level(CLK_PIN, 0);
    gpio_set_level(DIN_PIN, (address & (1 << i)) ? 1 : 0);
    gpio_set_level(CLK_PIN, 1);
  }
  for (int i = 7; i >= 0; i--) {
    gpio_set_level(CLK_PIN, 0);
    gpio_set_level(DIN_PIN, (dataNear & (1 << i)) ? 1 : 0);
    gpio_set_level(CLK_PIN, 1);
  }
  
  gpio_set_level(CS_PIN, 1);  // CS high for finishing
}

/*
  Initialization 2 matrix of MAX7219
*/
void initMax7219() {
  printf("initialization MAX7219\n");
  esp_rom_gpio_pad_select_gpio(DIN_PIN);           // Sets pins to GPIO mode
  esp_rom_gpio_pad_select_gpio(CLK_PIN);
  esp_rom_gpio_pad_select_gpio(CS_PIN);
  gpio_set_direction(DIN_PIN, GPIO_MODE_OUTPUT);   // Sets pins to output mode
  gpio_set_direction(CLK_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(CS_PIN, GPIO_MODE_OUTPUT);
 
  gpio_set_level(CS_PIN, 1);                        // CS high for default

  // Exit from test mode
  sendToChain(0x0F, 0x00, 0x00);
  // set brightness for MAX7219
  sendToChain(0x0A, BRIGHTNESS_MAX7219, BRIGHTNESS_MAX7219);
  // Количество сканируемых строк (7 = 8 строк)
  sendToChain(0x0B, 0x07, 0x07);
  // Включаем дисплей
  sendToChain(0x0C, 0x01, 0x01);
  // Очищаем дисплей
  for (int row = 1; row <= 8; row++) {
    sendToChain(row, 0x00, 0x00);
  }
}

// Функция обновления дисплея (отправляет все строки)
void updateDisplay() {
  for (unsigned int row = 1; row <= 8; row++) {
    // left — far, right — near
    sendToChain(row, leftBuffer[row - 1], rightBuffer[row - 1]);
  }
}

// Функция установки/сброса пикселя и обновления дисплея
void setPixel(unsigned short x, unsigned short y, bool state, bool UPDATE) {
  if (x > 15 || y > 7) return;  // Защита от неверных координат
  
  unsigned short* buffer = (x < 8) ? leftBuffer : rightBuffer;
  unsigned int localX = (x < 8) ? x : x - 8;
  
  // Обновляем буфер: localX — бит в строке y (MSB — localX=0)
  if (state) {
    buffer[y] |= (1 << (7 - localX));  // Зажечь бит
  } else {
    buffer[y] &= ~(1 << (7 - localX));  // Погасить бит
  }
  if(UPDATE)
    // Обновляем дисплей
    updateDisplay();
}