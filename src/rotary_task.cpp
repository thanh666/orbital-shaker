#include "main.h"
QueueHandle_t rotary_queue = NULL;
rotary_pos r_pos;
int raw = 0;
void vCreateRotaryQueue(void)
{
  rotary_queue = xQueueCreate(
      /* The number of items the queue can hold. */
      1,
      /* Size of each item is big enough to hold the
      whole structure. */
      sizeof(r_pos));
}
QueueHandle_t event_queue = rotary_encoder_create_queue();
rotary_encoder_info_t info = {(gpio_num_t)0};

void encoder_task(void *PvParameters)
{
  ESP_ERROR_CHECK(rotary_encoder_init(&info, ROT_ENC_A_GPIO, ROT_ENC_B_GPIO));
  ESP_ERROR_CHECK(rotary_encoder_enable_half_steps(&info, ENABLE_HALF_STEPS));
#ifdef FLIP_DIRECTION
  ESP_ERROR_CHECK(rotary_encoder_flip_direction(&info));
#endif

  // Create a queue for events from the rotary encoder driver.
  // Tasks can read from this queue to receive up to date position information.
  ESP_ERROR_CHECK(rotary_encoder_set_queue(&info, event_queue));
  counter send = {0};
  Speed send_buf = {0};
  // int set_time = 0;
  while (1)
  {
    // Wait for incoming events on the event queue.
    rotary_encoder_event_t event = {0};

    if (xQueueReceive(event_queue, &event, 1000 / portTICK_PERIOD_MS) == pdTRUE)
    {
      Serial.printf("Event: position %d, direction %s\n", event.state.position,
                    event.state.direction ? (event.state.direction == ROTARY_ENCODER_DIRECTION_CLOCKWISE ? "CW" : "CCW") : "NOT_SET");
      r_pos.pos = event.state.direction;
      if (STATE == Set_time)
      {
        send.set_time = event.state.position;
        xQueueSend(time_queue, (void *)&send, (TickType_t)0);
      }
      if (STATE == Set_Speed)
      {
        raw = event.state.position;
        xQueueSend(speed_queue, (void *)&send_buf, (TickType_t)0);
      }
    }
    else
    {
      // Poll current position and direction
      rotary_encoder_state_t state = {0};
      ESP_ERROR_CHECK(rotary_encoder_get_state(&info, &state));

      Serial.printf("Poll: position %d, direction %s\n", state.position,
                    state.direction ? (state.direction == ROTARY_ENCODER_DIRECTION_CLOCKWISE ? "CW" : "CCW") : "NOT_SET");
      r_pos.last_pos = state.position;
      if (STATE == Set_time)
      {
        send.set_time = state.position;
        xQueueSend(time_queue, (void *)&send, (TickType_t)0);
      }
      if (STATE == Set_Speed)
      {
        raw = state.position;
        xQueueSend(speed_queue, (void *)&send_buf, (TickType_t)0);
      }
      // Reset the device
      // if (RESET_AT && (state.position >= RESET_AT || state.position <= -RESET_AT))
      // {
      //   ESP_LOGI(TAG, "Reset");
      //   ESP_ERROR_CHECK(rotary_encoder_reset(&info));
      // }
    }

    if (STATE == Set_time)
      xQueueSend(time_queue, (void *)&send, (TickType_t)0);
    if (STATE == Set_Speed)
      xQueueSend(speed_queue, (void *)&send_buf, (TickType_t)0);
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
  ESP_LOGE(TAG, "queue receive failed");

  ESP_ERROR_CHECK(rotary_encoder_uninit(&info));
}