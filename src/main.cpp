#include "RTOS_tasks.h"

void setup ( void )  
{ 
  xTaskCreate(task_button, "buttons", 4096, NULL, 2, NULL);
  xTaskCreate(move_snowman, "move_snowman", 4096, NULL, 2, NULL);
  xTaskCreate(get_temp, "gettemp", 4096, NULL, 2, NULL);
}
 
void loop ( void )  
{

}

