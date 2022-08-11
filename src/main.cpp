#include "RTOS_tasks.h"

void setup ( void )  
{ 
  //esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  //esp_task_wdt_add(NULL); //add current thread to WDT watch
  //esp_task_wdt_reset();
  //xTaskCreate(start_screen, "start_screen", 4096, NULL, 2, NULL);
  xTaskCreate(task_button, "buttons", 4096, NULL, 2, NULL);
  xTaskCreate(show_display, "show_display", 2048, NULL, 15, NULL);
  xTaskCreate(getweight1, "getweight1", 2048, NULL, 2, NULL);
  xTaskCreate(getweight2, "getweight2", 2048, NULL, 2, NULL);
}
 
void loop ( void )  
{

}

