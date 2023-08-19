# orbital-shaker

Firmware for DIY orbital shaker. Contains functions: setup timer, speed.

## [Demo](https://youtu.be/mrFu4mhjGno)

## Technology used

- **Programming language:** C++
- **Framework:** FreeRTOS
- **Hardware:** stepper motor, ESP32, rotary encoder, 7 LED segment

## Project description

A firmware for DIY orbital shaker using RTOS to control the hardware. The shaker can shake up to 2.5kg.

## Detail

There are 7 tasks when the machine is working

- *Button task* is used to capture button presses event (double click, single click, long click, triple click)
- *Display task* send data to tm1637 ic for each state of the machine (Set time, set speed, running, finish)
- *Counter down task* get the set time and count it down
- *NTP task* get the DATETIME from the NTP server
- *Rotary task* process the rotary event from the user
- *Stepper task* control the stepper motor from the config of other tasks by Queue