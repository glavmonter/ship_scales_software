#include "RTOS_tasks.h"


const int LOADCELL1_DOUT_PIN = 32;
const int LOADCELL1_SCK_PIN = 33;
const int LOADCELL2_DOUT_PIN = 25;
const int LOADCELL2_SCK_PIN = 26;
HX711 scale1;
HX711 scale2;
double reading1;
double reading2;

int push_barcode;
int push_save_data;
int push_calibrate;
int push_weighting;

// PINOUT: scl = 14 // si = 13 // cs = 15 // rs = 12 // rse = 27
U8G2_ST7565_ERC12864_1_4W_SW_SPI u8g2 ( U8G2_R0, /* scl=*/  14 , /* si=*/  13 , /* cs=*/  15 , /* rs=*/  12 , /* rse=*/  27 ) ;

void start_page();

SemaphoreHandle_t btnSemaphore; // assign semaphore

// create function - interrupt handler
void IRAM_ATTR ISR_btn() // IRAM_ATTR means, that we use RAM (wich more faster and recommended for interrupts). ISR - interrupt service routine
{
  xSemaphoreGiveFromISR( btnSemaphore, NULL ); // Macro to release a semaphore from interruption. The semaphore must have previously been created with a call to xSemaphoreCreateBinary() or xSemaphoreCreateCounting().
}

void task_button(void *pvParameters) // create button RTOS task
{
  bool isISR = true;
  bool state_btn1 = true, state_btn2 = true, state_btn3 = true, state_btn4 = true; // assign four buttons
  
  pinMode(BUTTON_DOWN, INPUT);
  pinMode(BUTTON_UP, INPUT);
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
          push_weighting = 1;
        }
        else
        {
          push_weighting = 0;
        }
      }

      if  (st2 != state_btn2)
      {
        state_btn2 = st2;
        if (st2 == LOW)
        {
          push_calibrate = 1;
        }
        else
        {
          push_calibrate = 0;
        }
      }

      if  (st3 != state_btn3)
      {
        state_btn3 = st3;
        if (st3 == LOW)
        {
          push_barcode = 1;
        }
        else
        {
          push_barcode = 0;
        }
      }

      if  (st4 != state_btn4)
      {
        state_btn4 = st4;
        if (st4 == LOW)
        {
          push_save_data = 1;
        }
        else
        {
          push_save_data = 0;
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

void show_display(void *pvParameters) // create display menu task
{
  //Serial.begin(57600);
  u8g2. begin ( ) ;
  u8g2. setContrast  (10) ;
  u8g2. enableUTF8Print ( ) ;
  double final_weight;
  int i = 0;
  while (true)
  {
    if (i < 1)
    {
      start_page();
      vTaskDelay(5000);
      i++;
    }
    u8g2. firstPage ( ) ;
    do  
    {
      u8g2.setFont(u8g2_font_7x13B_tf);
      u8g2.setCursor(5, 25);
      u8g2.print(reading1, 5);

      u8g2.setFont(u8g2_font_7x13B_tf);
      u8g2.setCursor(70, 25);
      u8g2.print(reading2, 5);

      final_weight = reading2*(20/reading1);
      u8g2.setFont(u8g2_font_7x13B_tf);
      u8g2.setCursor(30, 45);
      u8g2.print(final_weight, 5);

      u8g2.setFont(u8g2_font_fivepx_tr);
      if (push_barcode == 1)
      {
        u8g2.drawButtonUTF8(0, 7, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "BARCODE" );
      }
      u8g2.setCursor(0, 7);
      u8g2.print("BARCODE");
      u8g2.setFont(u8g2_font_fivepx_tr);
      if (push_save_data == 1)
      {
        u8g2.drawButtonUTF8(0, 64, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "SAVE DATA" );
      }
      u8g2.setCursor(0, 64);
      u8g2.print("SAVE DATA");
      if (push_calibrate == 1)
      {
        u8g2.drawButtonUTF8(85, 7, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "CALIBRATE" );
      }
      u8g2.setFont(u8g2_font_fivepx_tr);
      u8g2.setCursor(85, 7);
      u8g2.print("CALIBRATE");
      if (push_weighting == 1)
      {
        u8g2.drawButtonUTF8(85, 64, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "WEIGHTING" );
      }
      u8g2.setFont(u8g2_font_fivepx_tr);
      u8g2.setCursor(85, 64);
      u8g2.print("WEIGHTING");
    }
    while ( u8g2.nextPage() );
    vTaskDelay(100);
  }
}

void getweight1(void *pvParameters)
{
  scale1.begin(LOADCELL1_DOUT_PIN, LOADCELL1_SCK_PIN);
  scale1.set_scale(19000);
  scale1.tare();

  while (1)
  {
    reading1 = scale1.get_units(5);
    vTaskDelay(250);
  }
   
}

void getweight2(void *pvParameters)
{
  scale2.begin(LOADCELL2_DOUT_PIN, LOADCELL2_SCK_PIN);
  scale2.set_scale(18050);
  scale2.tare();

  while (1)
  {
    reading2 = scale2.get_units(5);
    vTaskDelay(250);
  }
   
}

void start_page()
{
  u8g2. firstPage ( ) ;
  do
  {
    u8g2.setFont(u8g2_font_t0_16b_tf);
    u8g2.setCursor(10, 15);
    u8g2.print("Digital Marine");
    u8g2.setFont(u8g2_font_t0_16b_tf);
    u8g2.setCursor(15, 30);
    u8g2.print("Balances 100g");
    u8g2.setFont(u8g2_font_fivepx_tr);
    u8g2.setCursor(5, 45);
    u8g2.print("Institute of Oceanology RAS");
    u8g2.setFont(u8g2_font_fivepx_tr);
    u8g2.setCursor(50, 53);
    u8g2.print("2022");
    u8g2.setFont(u8g2_font_fivepx_tr);
    u8g2.setCursor(25, 61);
    u8g2.print("t.me/coreblogger");
  }
  while ( u8g2.nextPage() );
}