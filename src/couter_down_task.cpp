#include "main.h"
counter counter_time;
Display time_buffer;
QueueHandle_t time_queue = NULL;

void create_time_queue()
{
  time_queue = xQueueCreate(1, sizeof(counter_time));
}

void count_down_task(void *arg)
{
  int second = 0, last_sec = 0;
  while (1)
  {
    if (xQueueReceive(display_queue, &time_buffer, 500 / portTICK_PERIOD_MS) == pdTRUE)
    {
      second = time_buffer.time_info.tm_sec;
    }
    // if ( == pdTRUE)
    // {
    xQueueReceive(time_queue, &counter_time, 0);
    if (second != last_sec && STATE == Running)
    {
      counter_time.remain_time--;
      if (counter_time.remain_time == 0)
      {
        STATE = Finish;
        rotary_encoder_reset(&info);
      }
      xQueueSend(time_queue, (void *)&counter_time, 20 / portTICK_PERIOD_MS);
    }
    // }
    last_sec = second;
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}