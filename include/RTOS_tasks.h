#include <Arduino.h> 

//SCLK = 18, MISO = 19, MOSI = 23, SS = 5 
#include <SPI.h>

#define BUTTON_UP 26
#define BUTTON_DOWN 27
#define BUTTON_RIGHT 32
#define BUTTON_LEFT 33
#define STD_LED 2

#define ONE_WIRE_BUS 4

void task_button(void *pvParameters);
void move_snowman(void *pvParameters);
void get_temp(void *pvParameters);

 
