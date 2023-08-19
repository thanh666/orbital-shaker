#include "main.h"

EventGroupHandle_t xButtonEventGroup;
// static EventGroupHandle_t xEventGroup;
Button2 rot_button;

void handler(Button2 &btn)
{
  switch (btn.getClickType())
  {
  case SINGLE_CLICK:
    xEventGroupSetBits(xButtonEventGroup, BIT_CLICK);
    break;
  case DOUBLE_CLICK:
    xEventGroupSetBits(xButtonEventGroup, BIT_DOUBLE);
    Serial.print("double ");
    break;
  case TRIPLE_CLICK:
    Serial.print("triple ");
    break;
  case LONG_CLICK:
    xEventGroupSetBits(xButtonEventGroup, BIT_LONGCLICK);
    Serial.print("long");
    break;
  }
  Serial.print("click");
  Serial.print(" (");
  Serial.print(btn.getNumberOfClicks());
  Serial.println(")");
}

void button_init()
{
  rot_button.begin(ROT_BUTTON);
  rot_button.setClickHandler(handler);
  rot_button.setLongClickHandler(handler);
  rot_button.setDoubleClickHandler(handler);
  rot_button.setTripleClickHandler(handler);
  xButtonEventGroup = xEventGroupCreate();
  pinMode(STEPPER_EN, OUTPUT);
  // attachInterrupt(ENCODER_A, dem_xung, FALLING);
}

void button_task(void *arg)
{
  while (1)
  {
    rot_button.loop();
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}

void set_time(void *arg)
{
  EventBits_t uxBits;
  // Speed buf;
  // long speed;
  // QueueHandle_t time_queue_buf;
  // rot_event = (QueueHandle_t)arg;
  while (1)
  {
    uxBits = xEventGroupWaitBits(xButtonEventGroup, BIT_DOUBLE, pdFALSE, pdFALSE, portMAX_DELAY);
    if ((uxBits & BIT_DOUBLE) != 0)
    {
      Serial.println("settime");
      STATE = Set_time;
      // if (xQueueReceive(rotary_queue, &buf, 600 / portTICK_PERIOD_MS) == pdTRUE)
      // {
      //   Serial.printf("event: %d\n", counter_time.set_time);
      //   if (buf.pos == ROTARY_ENCODER_DIRECTION_CLOCKWISE)
      //     counter_time.set_time++;
      //   if (buf.pos == ROTARY_ENCODER_DIRECTION_COUNTER_CLOCKWISE)
      //     counter_time.set_time--;
      //   xQueueSend(time_queue, &counter_time, 200 / portTICK_PERIOD_MS);
      // }
      if ((xEventGroupWaitBits(xButtonEventGroup, BIT_CLICK, pdFALSE, pdFALSE, portMAX_DELAY) & BIT_CLICK) != 0)
      {
        counter_time.remain_time = counter_time.set_time;
        STATE = Set_Speed;
        xEventGroupClearBits(xButtonEventGroup, BIT_DOUBLE);
        xQueueSend(time_queue, &counter_time, 20 / portTICK_PERIOD_MS);
        rotary_encoder_reset(&info);
      }
    }
    // counter_time.set_time = r_pos.pos;
  }
}

void set_speed(void *arg)
{
  // Speed buf;
  long speed, set_speed;
  while (1)
  {
    if (STATE == Set_Speed)
    {
      // if ((xEventGroupWaitBits(xButtonEventGroup, BIT_CLICK, pdFALSE, pdFALSE, portMAX_DELAY) & BIT_CLICK) != 0)
      // {
      // if (xQueueReceive(speed_queue, &stepper_speed, 20 / portTICK_PERIOD_MS) == pdTRUE)

      // }
      if ((xEventGroupWaitBits(xButtonEventGroup, BIT_LONGCLICK, pdTRUE, pdFALSE, portMAX_DELAY) & BIT_LONGCLICK) != 0)
      {
        speed = raw;
        Serial.println(speed);
        set_speed = map(speed, 20, 300, 50, 1000);
        Serial.printf("xung stepper: %d\n", set_speed);
        stepper.setMaxSpeed(1000);
        stepper.setSpeed(set_speed);
        xEventGroupClearBits(xButtonEventGroup, BIT_CLICK);
        STATE = Running;
      }
    }
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}