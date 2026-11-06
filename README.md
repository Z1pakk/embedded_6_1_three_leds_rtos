# Three LEDs with FreeRTOS tasks

Homework 6.1: blink three LEDs at different rates on an ESP32-S3, each LED in its own FreeRTOS task.

## Hardware

| LED   | GPIO | Toggle period |
|-------|------|---------------|
| Red   | 4    | 200 ms        |
| Green | 5    | 500 ms        |
| Blue  | 6    | 1000 ms       |

Board: ESP32-S3-DevKitC-1. Connect each LED through a resistor (220–330 Ω) to GND.
Pins and periods are set with `#define`s at the top of `src/main.c`.

## How it works

- `led_t` is a small LED driver: GPIO pin and current mode (`LED_MODE_OFF` / `LED_MODE_ON`).
  Functions `led_init`, `led_set_mode` and `led_toggle` take a `led_t *`.
- `blink_led_t` is the task configuration: a pointer to an LED plus its blink period.
  The period is kept out of `led_t`, so the driver knows nothing about timing.
- One task function, `led_blink_task`, is started three times, each with a different
  `blink_led_t` passed through `pvParameters`. It toggles its LED and sleeps with `vTaskDelay`.
- LEDs and configs are `static`, so they stay valid after `app_main()` returns.
- `check_task_created` stops the program (`configASSERT`) if a task can't be created.

## Build and flash

Built with PlatformIO, framework ESP-IDF:

```sh
pio run                 # build
pio run -t upload       # flash
pio device monitor      # serial monitor
```

## Next steps

- Make `led_toggle` call `led_set_mode`
- Store configs in an array and create tasks in a loop
- Move the LED driver into `include/led.h` + `src/led.c`
- Add `LED_MODE_BLINK` and control modes with a button through a queue
