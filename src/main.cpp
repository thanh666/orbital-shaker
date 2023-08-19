#include "main.h"

const char *ssid = "PoPo 1";
const char *password = "supim0404";
int intr_pulse = 0; // dem xung

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600);
void pos_exec(void *Parameter)
{
  rotary_pos pos_buffer;
  while (1)
  {
    if (xQueueReceive(rotary_queue, &(pos_buffer), 500 / portTICK_PERIOD_MS) == pdTRUE)
    {
      Serial.printf("pos: %d, l_pos: %d\n", pos_buffer.pos, pos_buffer.last_pos);
      vTaskDelay(500 / portTICK_RATE_MS);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  vCreateDisplayQueue();
  wifi_init();
  ESP_ERROR_CHECK(gpio_install_isr_service(0));
  vCreateRotaryQueue();
  vCreateSpeedQueue();
  // static StaticTimer_t tmr1_state;
  // TimerHandle_t tmr1_h = xTimerCreateStatic(
  //     "TMR1",
  //     pdMS_TO_TICKS(1000),
  //     pdTRUE,
  //     NULL,
  //     speed_cal_task,
  //     &tmr1_state);
  // xTimerStart(tmr1_h, 0);
  button_init();
  create_time_queue();
  STATE = Finish;
  xTaskCreate(&button_task, "BUTTON", 4096, NULL, 10, NULL);
  xTaskCreate(&set_time, "set_time", 2048, (void *)event_queue, 4, NULL);
  xTaskCreate(&set_speed, "set_speed", 2048, NULL, 6, NULL);
  xTaskCreate(&stepper_run, "running", 2048, NULL, 4, NULL);
  xTaskCreate(&encoder_task, "encoder", 4096, NULL, 5, NULL);
  xTaskCreate(&count_down_task, "counter", 2048, NULL, 5, NULL);
  xTaskCreate(&lcd_tm1637_task, "lcd_tm1637_task", 6144, NULL, 5, NULL);
  // xTaskCreate(&pos_exec, "pos_exec", 2048, NULL, 4, NULL);
  xTaskCreate(&get_RTC_task, "NTP_TIME", 2048, NULL, 6, NULL);
}

void loop()
{
}
