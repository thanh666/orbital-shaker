
#include "input_iot.h"

input_callback_t input_callback = NULL;
timeoutFcn button_timeout_callback = NULL;
static uint64_t _start, _stop, _pressed_tick;
static TimerHandle_t xTimers;

static void IRAM_ATTR gpio_input_handler(void *arg)
{
  int gpio_num = (uint32_t)arg;
  uint32_t rtc = xTaskGetTickCountFromISR();

  if (gpio_get_level(gpio_num) == 0)
  {
    _start = rtc;
    xTimerStart(xTimers, 0);
  }
  else
  {
    xTimerStop(xTimers, 0);
    _stop = rtc;
    _pressed_tick = _stop - _start;
    input_callback(gpio_num, _pressed_tick);
  }
}

void vTimerCallback(TimerHandle_t xTimer)
{
  uint32_t ID;

  /* Optionally do something if the pxTimer parameter is NULL. */
  configASSERT(xTimer);

  /* The number of times this timer has expired is saved as the
  timer's ID.  Obtain the count. */
  ID = (uint32_t)pvTimerGetTimerID(xTimer);
  if (ID == 0)
  {
    button_timeout_callback(0);
  }
}

void input_io_init(gpio_num_t gpio_num, interrupt_type_edge type)
{
  gpio_pad_select_gpio(gpio_num);
  gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
  gpio_set_pull_mode(gpio_num, GPIO_PULLUP_ONLY);
  gpio_set_intr_type(gpio_num, type);
  gpio_install_isr_service(0);
  gpio_isr_handler_add(gpio_num, gpio_input_handler, (void *)gpio_num);
  xTimers = xTimerCreate(/* Just a text name, not used by the RTOS
                         kernel. */
                         "Timeout",
                         /* The timer period in ticks, must be
                         greater than 0. */
                         5000 / portTICK_RATE_MS,
                         /* The timers will auto-reload themselves
                         when they expire. */
                         pdFALSE,
                         /* The ID is used to store a count of the
                         number of times the timer has expired, which
                         is initialised to 0. */
                         (void *)0,
                         /* Each timer calls the same callback when
                         it expires. */
                         vTimerCallback);
}

int input_io_get_level(gpio_num_t gpio_num)
{
  return gpio_get_level(gpio_num);
}

void input_set_callback(void *cb)
{
  input_callback = cb;
}
void input_set_timeout_callback(void *cb)
{
  button_timeout_callback = cb;
}