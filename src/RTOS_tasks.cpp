#include "RTOS_tasks.h"
#include <U8g2lib.h>
#include <DallasTemperature.h>
#include <OneWire.h>

// scl = 14
// si = 13
// cs = 15
// rs = 12
// rse = 27
U8G2_ST7565_ERC12864_1_4W_SW_SPI u8g2 ( U8G2_R0, /* scl=*/  14 , /* si=*/  13 , /* cs=*/  15 , /* rs=*/  12 , /* rse=*/  27 ) ;

SemaphoreHandle_t btnSemaphore; // assign semaphore
int step_snowman = 15;          // assign move step of menu selector

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


float celcius;

void IRAM_ATTR ISR_btn() // IRAM_ATTR means, that we use RAM (wich more faster and recommended for interrupts). ISR - interrupt service routine
{
  xSemaphoreGiveFromISR( btnSemaphore, NULL ); // Macro to release a semaphore from interruption. The semaphore must have previously been created with a call to xSemaphoreCreateBinary() or xSemaphoreCreateCounting().
}

void task_button(void *pvParameters) // create button RTOS task
{
  bool isISR = true;
  bool state_btn1 = true, state_btn2 = true, state_btn3 = true, state_btn4 = true; // assign four buttons
  
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(STD_LED, OUTPUT);

  btnSemaphore = xSemaphoreCreateBinary(); // create simple binary semaphore

  xSemaphoreTake(btnSemaphore, 100); // taking semaphore to avoid false trigger of the button

  attachInterrupt(BUTTON_DOWN, ISR_btn, FALLING); // launch four interrupt handlers
  attachInterrupt(BUTTON_UP, ISR_btn, FALLING);
  attachInterrupt(BUTTON_RIGHT, ISR_btn, FALLING);
  attachInterrupt(BUTTON_LEFT, ISR_btn, FALLING);

  while (true)
  {
    if (isISR) // 
    {
      xSemaphoreTake(btnSemaphore, portMAX_DELAY);
      detachInterrupt(BUTTON_DOWN);
      detachInterrupt(BUTTON_UP);
      detachInterrupt(BUTTON_RIGHT);
      detachInterrupt(BUTTON_LEFT);
      isISR = false;
    }
    else
    {
      bool st1 = digitalRead(BUTTON_DOWN);
      bool st2 = digitalRead(BUTTON_UP);
      bool st3 = digitalRead(BUTTON_RIGHT);
      bool st4 = digitalRead(BUTTON_LEFT);

      if  (st1 != state_btn1)
      {
        state_btn1 = st1;
        if (st1 == LOW)
        {
          digitalWrite(STD_LED, HIGH);
        }
        else
        {
          digitalWrite(STD_LED, LOW);
        }
      }

      if  (st2 != state_btn2)
      {
        state_btn2 = st2;
        if (st2 == LOW)
        {
          digitalWrite(STD_LED, HIGH);
        }
        else
        {
          digitalWrite(STD_LED, LOW);
        }
      }

      if  (st3 != state_btn3)
      {
        state_btn3 = st3;
        if (st3 == LOW)
        {
          digitalWrite(STD_LED, HIGH);
          step_snowman = step_snowman + 15;
        }
        else
        {
          digitalWrite(STD_LED, LOW);
          if (step_snowman == 75) step_snowman = 15;
        }
      }

      if  (st4 != state_btn4)
      {
        state_btn4 = st4;
        if (st4 == LOW)
        {
          digitalWrite(STD_LED, HIGH);
        }
        else
        {
          digitalWrite(STD_LED, LOW);
        }
      }
      if (st1 == HIGH && st2 == HIGH && st3 == HIGH && st4 == HIGH)
      {
        attachInterrupt(BUTTON_DOWN, ISR_btn, FALLING);
        attachInterrupt(BUTTON_UP, ISR_btn, FALLING);
        attachInterrupt(BUTTON_RIGHT, ISR_btn, FALLING);
        attachInterrupt(BUTTON_LEFT, ISR_btn, FALLING);
        isISR = true;
      }
      vTaskDelay(100); // this function calls the task manager, which set this task to WAIT mode for 100 system ticks.
    }

  }
}

void move_snowman(void *pvParameters) // create display menu task
{
  u8g2. begin ( ) ;
  u8g2. setContrast  (10) ;
  u8g2. enableUTF8Print ( ) ;
  while (true)
  {
    {
      u8g2. firstPage ( ) ;
    do  
    {
      u8g2.setFont(u8g2_font_unifont_t_symbols);
      u8g2.drawGlyph(0, step_snowman, 0x2603);
      u8g2.setFont(u8g2_font_10x20_te);
      u8g2.setCursor(25, 15);
      u8g2.print("snowman");
      u8g2.setFont(u8g2_font_10x20_te);
      u8g2.setCursor(25, 30);
      u8g2.print("fireman");
      u8g2.setFont(u8g2_font_10x20_te);
      u8g2.setCursor(25, 45);
      u8g2.print("blowman");
      u8g2.setFont(u8g2_font_10x20_te);
      u8g2.setCursor(25, 60);
      //u8g2.print("whistleman");
      u8g2.print(celcius);
    }
    while ( u8g2.nextPage() );
    vTaskDelay(100);
    }
  }
}

void get_temp(void *pvParameters) // get temperature from D18B20 sensor
{
  sensors.begin();
  while (1)
  {
    sensors.requestTemperatures();
    celcius = sensors.getTempCByIndex(0);

  }
   
}