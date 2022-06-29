#include <Arduino.h> 
#include <U8g2lib.h>
//#include <esp_task_wdt.h>
//SCLK = 18, MISO = 19, MOSI = 23, SS = 5 
#include <SPI.h>

//#define WDT_TIMEOUT 2

#define BUTTON_UP 25
#define BUTTON_DOWN 26
#define BUTTON_RIGHT 2
#define BUTTON_LEFT 4

void task_button(void *pvParameters);
void move_snowman(void *pvParameters);

 
