#if !defined(_MAIN_H_)
#define _MAIN_H_

#include <Arduino.h>
#include <AccelStepper.h>
#include "esp_wifi.h"
#include "http_parser.h"
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "tm1637.h"
#include "rotary_encoder.h"
#include "Button2.h"

extern const char *ssid;
extern const char *password;

/* The periods assigned to the one-shot and auto-reload timers are 6 second and one econd respectively. */
#define ROT_ENC_A_GPIO GPIO_NUM_18
#define ROT_ENC_B_GPIO GPIO_NUM_19
#define ROT_BUTTON GPIO_NUM_21
#define ENCODER_A GPIO_NUM_22
#define ENCODER_B GPIO_NUM_23
#define STEPPER_PULSE GPIO_NUM_32
#define STEPPER_DIR GPIO_NUM_35
#define STEPPER_EN GPIO_NUM_34
#define BIT_CLICK (1 << 0)
#define BIT_DOUBLE (1 << 1)
#define BIT_LONGCLICK (1 << 2)

const gpio_num_t LED_CLK = GPIO_NUM_25;
const gpio_num_t LED_DTA = GPIO_NUM_26;
#define ENABLE_HALF_STEPS false // Set to true to enable tracking of rotary encoder at half step resolution
#define RESET_AT 201            // Set to a positive non-zero number to reset the position if this value is exceeded
#define FLIP_DIRECTION false    // Set to true to reverse the clockwise/counterclockwise sense

enum status
{
  Start,
  Idle,
  Set_time,
  Set_Speed,
  Running,
  Stop,
  Finish
};

struct rotary_pos
{
  int pos, last_pos;
};

struct Display
{
  struct tm time_info;
  int current_time;
};

struct Speed
{
  int set_speed, current_speed, raw;
};

struct counter
{
  int remain_time;
  int set_time;
  int display_time;
};

extern QueueHandle_t rotary_queue;
extern QueueHandle_t display_queue;
extern QueueHandle_t event_queue;
extern QueueHandle_t time_queue;
extern QueueHandle_t speed_queue;
extern WiFiUDP ntpUDP;
extern NTPClient timeClient;
extern AccelStepper stepper;
extern Button2 rot_button;
extern counter counter_time;
extern EventGroupHandle_t xButtonEventGroup;
extern tm1637_led_t *lcd;
extern rotary_encoder_info_t info;
extern status STATE;
extern rotary_pos r_pos;
extern Speed stepper_speed;
extern int raw;
// extern int intr_pulse;

void vCreateRotaryQueue(void);
void encoder_task(void *PvParameters);
void vCreateDisplayQueue(void);
void lcd_tm1637_task(void *arg);
void wifi_init();
void get_RTC_task(void *arg);
void button_init();
void button_task(void *arg);
void count_down_task(void *arg);
void set_time(void *arg);
void create_time_queue();
// void IRAM_ATTR dem_xung();
void vCreateSpeedQueue(void);
void speed_cal_task(void *arg);
void set_speed(void *arg);
void stepper_run(void *arg);

#endif // _MAIN_H_
