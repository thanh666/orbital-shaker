#if !defined(_INPUT_IOT_H_)
#define _INPUT_IOT_H_

#include <stdio.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_err.h"
// #include "hal/gpio_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

typedef enum
{
  LO_TO_HI = 1,
  HI_TO_LO = 2,
  ANY_EDGE = 3
} interrupt_type_edge;

typedef void (*input_callback_t)(int, uint64_t);
typedef void (*timeoutFcn)(int);

void input_io_init(gpio_num_t gpio_num, interrupt_type_edge type);
int input_io_get_level(gpio_num_t gpio_num);
void input_set_callback(void *cb);
void input_set_timeout_callback(void *cb);

#endif // _INPUT_IOT_H_
