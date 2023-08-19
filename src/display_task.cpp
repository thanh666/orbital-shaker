#include "main.h"

QueueHandle_t display_queue = NULL;
counter time_queue_buf;
Speed speed_buf;

Display display;
status STATE;

void vCreateDisplayQueue(void)
{
  display_queue = xQueueCreate(1, sizeof(display));
}

void get_RTC_task(void *arg)
{
  while (1)
  {
    while (!timeClient.update())
    {
      timeClient.forceUpdate();
    }
    display.time_info.tm_hour = timeClient.getHours();
    display.time_info.tm_min = timeClient.getMinutes();
    display.time_info.tm_sec = timeClient.getSeconds();
    int time_number = 100 * display.time_info.tm_hour + display.time_info.tm_min;
    // Serial.printf("Time: %d", time_number);
    display.current_time = time_number;
    xQueueSend(display_queue, (void *)&display, (TickType_t)0);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
tm1637_led_t *lcd = tm1637_init(LED_CLK, LED_DTA);

void lcd_tm1637_task(void *arg)
{
  // setenv("TZ", "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", 1);
  // tzset();
  // int speed_buf = 0;
  tm1637_set_brightness(lcd, 5);
  int time_dis[4], second = 0, min = 0;
  Display data;
  while (true)
  {

    // for (uint8_t x = 0; x < 3; ++x)
    // {
    //   // Set random system time
    //   struct timeval tm_test = {1517769863 + (x * 3456), 0};
    //   settimeofday(&tm_test, NULL);

    //   // Get current system time
    //   time_t now = 0;
    //   struct tm timeinfo = {0};
    //   time(&now);
    //   localtime_r(&now, &timeinfo);
    //   int time_number = 100 * timeinfo.tm_hour + timeinfo.tm_min;

    //   // Display time with blinking dots
    //   for (int z = 0; z < 5; ++z)
    //   {
    //     tm1637_set_number_lead_dot(lcd, time_number, true, z % 2 ? 0xFF : 0x00);
    //     // Serial.printf("Time: %d\n", time_number);
    //     vTaskDelay(500 / portTICK_RATE_MS);
    //   }
    // }

    /**Hien thi thoi gian thuc*/
    // if (xQueueReceive(display_queue, &data, 200 / portTICK_PERIOD_MS) == pdTRUE)
    // {
    // tm1637_set_number_lead_dot(lcd, data.current_time, true, data.time_info.tm_sec % 2 ? 0xFF : 0x00);
    //   vTaskDelay(500 / portTICK_PERIOD_MS);
    // }

    switch (STATE)
    {
    case Set_time:
      if (xQueueReceive(time_queue, &time_queue_buf, 0) == pdTRUE)
      {
        tm1637_set_number(lcd, counter_time.set_time);
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }
      break;
    case Set_Speed:
      // if (xQueueReceive(speed_queue, &speed_buf, 0) == pdTRUE)
      // {
      tm1637_set_number(lcd, raw);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      // }
      // Serial.println(raw);
      break;
    case Running:
      stepper.runSpeed();
      if (xQueueReceive(time_queue, &time_queue_buf, 50 / portTICK_PERIOD_MS) == pdTRUE)
      {
        time_queue_buf.display_time = time_queue_buf.remain_time;
        if (time_queue_buf.display_time < 60)
        {
          time_dis[0] = 0;
          time_dis[1] = 0;
          time_dis[2] = time_queue_buf.display_time / 10;
          time_dis[3] = time_queue_buf.display_time % 10;
          for (int i = 0; i < 4; i++)
          {
            tm1637_set_segment_number(lcd, i, time_dis[i], true);
          }
        }
        if (time_queue_buf.display_time >= 60)
        {
          min = time_queue_buf.display_time / 60;
          second = time_queue_buf.display_time % 60;
          time_dis[0] = min / 10;
          time_dis[1] = min % 10;
          time_dis[2] = second / 10;
          time_dis[3] = second % 10;
          for (int i = 0; i < 4; i++)
          {
            tm1637_set_segment_number(lcd, i, time_dis[i], true);
          }
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }
      break;
    case Finish:
      // uint8_t seg_data[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20};
      // for (uint8_t x = 0; x < 32; ++x)
      // {
      //   uint8_t v_seg_data = seg_data[x % 6];
      //   tm1637_set_segment_raw(lcd, 0, v_seg_data);
      //   tm1637_set_segment_raw(lcd, 1, v_seg_data);
      //   tm1637_set_segment_raw(lcd, 2, v_seg_data);
      //   tm1637_set_segment_raw(lcd, 3, v_seg_data);
      //   vTaskDelay(100 / portTICK_PERIOD_MS);
      // }
      // Serial.println(STATE);
      stepper.disableOutputs();
      digitalWrite(STEPPER_EN, 0);
      if (xQueueReceive(display_queue, &data, 200 / portTICK_PERIOD_MS) == pdTRUE)
      {
        tm1637_set_number_lead_dot(lcd, data.current_time, true, data.time_info.tm_sec % 2 ? 0xFF : 0x00);
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }
      break;
    }
  }
}