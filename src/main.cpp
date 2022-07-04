#include "RTOS_tasks.h"

void setup ( void )  
{ 
  //esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  //esp_task_wdt_add(NULL); //add current thread to WDT watch
  //esp_task_wdt_reset();
  xTaskCreate(task_button, "buttons", 4096, NULL, 2, NULL);
  xTaskCreate(move_snowman, "move_snowman", 4096, NULL, 2, NULL);
  xTaskCreate(getweight, "getweight", 4096, NULL, 2, NULL);
}
 
void loop ( void )  
{

}

