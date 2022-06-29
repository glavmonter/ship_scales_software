#include "RTOS_tasks.h"


// scl = 14
// si = 13
// cs = 15
// rs = 12
// rse = 27
U8G2_ST7565_ERC12864_1_4W_SW_SPI u8g2 ( U8G2_R0, /* scl=*/  14 , /* si=*/  13 , /* cs=*/  15 , /* rs=*/  12 , /* rse=*/  27 ) ;

SemaphoreHandle_t btnSemaphore; // assign semaphore
int step_snowman_vertical = 50;
int step_snowman_horizontal = 40;          // assign move step of menu selector


// create function - interrupt handler
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

  btnSemaphore = xSemaphoreCreateBinary(); // create simple binary semaphore

  xSemaphoreTake(btnSemaphore, 100); // taking semaphore to avoid false trigger of the button

  attachInterrupt(BUTTON_DOWN, ISR_btn, FALLING); // launch four interrupt handlers (button short-circuit to GND)
  attachInterrupt(BUTTON_UP, ISR_btn, FALLING);
  attachInterrupt(BUTTON_RIGHT, ISR_btn, FALLING);
  attachInterrupt(BUTTON_LEFT, ISR_btn, FALLING);

  while (true)
  {
    if (isISR) // interrupt handlers 
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
          step_snowman_vertical = step_snowman_vertical + 15;
          step_snowman_horizontal = step_snowman_horizontal + 15;
        }
        else
        {
          step_snowman_vertical = 50;
          step_snowman_horizontal = 40;
        }
      }

      if  (st2 != state_btn2)
      {
        state_btn2 = st2;
        if (st2 == LOW)
        {
          step_snowman_vertical = step_snowman_vertical + 15;
          step_snowman_horizontal = step_snowman_horizontal - 15;
        }
        else
        {
          step_snowman_vertical = 50;
          step_snowman_horizontal = 40;
        }
      }

      if  (st3 != state_btn3)
      {
        state_btn3 = st3;
        if (st3 == LOW)
        {
          step_snowman_vertical = step_snowman_vertical - 15;
          step_snowman_horizontal = step_snowman_horizontal - 15;
        }
        else
        {
          step_snowman_vertical = 50;
          step_snowman_horizontal = 40;
        }
      }

      if  (st4 != state_btn4)
      {
        state_btn4 = st4;
        if (st4 == LOW)
        {
          step_snowman_vertical = step_snowman_vertical - 15;
          step_snowman_horizontal = step_snowman_horizontal + 15;
        }
        else
        {
          step_snowman_vertical = 50;
          step_snowman_horizontal = 40;
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
      //u8g2.setFont(u8g2_font_fur30_tn);
      //u8g2.setCursor(10, 45);
      //u8g2.print(celcius);
      u8g2.setFont(u8g2_font_unifont_t_symbols);
      u8g2.drawGlyph(step_snowman_vertical, step_snowman_horizontal, 0x2603);
      u8g2.setFont(u8g2_font_fivepx_tr);
      u8g2.setCursor(0, 7);
      u8g2.print("BARCODE");
      u8g2.setFont(u8g2_font_fivepx_tr);
      u8g2.setCursor(0, 64);
      u8g2.print("SAVE DATA");
      u8g2.setFont(u8g2_font_fivepx_tr);
      u8g2.setCursor(85, 7);
      u8g2.print("CALIBRATE");
      u8g2.setFont(u8g2_font_fivepx_tr);
      u8g2.setCursor(85, 64);
      u8g2.print("WEIGHTING");
    }
    while ( u8g2.nextPage() );
    vTaskDelay(100);
    }
  }
}

//void save_data(void *pvParameters)


//void get_RTC_time(void *pvParameters)


//void barcode_scanner(void *pvParameters)