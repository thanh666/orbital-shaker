#include "main.h"

AccelStepper stepper(AccelStepper::DRIVER, STEPPER_PULSE, STEPPER_DIR);
Speed stepper_speed;
QueueHandle_t speed_queue = NULL;

void IRAM_ATTR dem_xung()
{
  // intr_pulse++;
}

void vCreateSpeedQueue(void)
{
  speed_queue = xQueueCreate(
      /* The number of items the queue can hold. */
      1,
      /* Size of each item is big enough to hold the
      whole structure. */
      sizeof(stepper_speed));
}

void speed_cal_task(void *arg)
{
  int rpm = 0;

  while (1)
  {
    // rpm = (intr_pulse / 20) * 60;
    // stepper_speed.current_speed = rpm;
    // xQueueSend(speed_queue, (void *)&stepper_speed, (TickType_t)0);
    // intr_pulse = 0;
  }
}

void stepper_run(void *arg)
{
  while (1)
  {
    switch (STATE)
    {
    case Running:
      stepper.runSpeed();
      // vTaskDelay(30 / portTICK_PERIOD_MS);
      break;
    }
  }
}
