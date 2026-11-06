#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

// Define GPIO pins for the LEDs
#define LED_RED_PIN  GPIO_NUM_4
#define LED_GREEN_PIN  GPIO_NUM_5
#define LED_BLUE_PIN  GPIO_NUM_6

// LED blinking intervals in milliseconds
#define LED_RED_DELAY_MS  200
#define LED_GREEN_DELAY_MS  500
#define LED_BLUE_DELAY_MS  1000

// Task related constants
#define LED_TASK_STACK_BYTES 2048
#define LED_TASK_PRIORITY 2

// Enum to represent the LED mode (on or off)
typedef enum {
    LED_MODE_OFF = 0,
    LED_MODE_ON  = 1,
} led_mode_t;

// Structure to hold the state of an LED
typedef struct {
    gpio_num_t pin;
    led_mode_t mode;
} led_t;

// Structure to hold the configuration for each LED blinking task
typedef struct {
    led_t   *led;           // which LED (pointer, not a copy)
    uint32_t period_ms;     // how this task blinks it
} blink_led_t;

// Prototypes for LED control functions
static void led_init(led_t *led, gpio_num_t pin);
static void led_toggle(led_t *led);
static void led_set_mode(led_t *led, led_mode_t mode);

// Initialize the LED structure and configure the GPIO pin
static void led_init(led_t *led, gpio_num_t pin) {
    led->pin = pin;
    led_set_mode(led, LED_MODE_OFF);

    gpio_set_direction(led->pin, GPIO_MODE_OUTPUT);
}

// Toggle the LED state and update the GPIO level accordingly
static void led_toggle(led_t *led) {
    if (led->mode == LED_MODE_OFF) {
        gpio_set_level(led->pin, 1);
        led->mode = LED_MODE_ON;
    } else {
        gpio_set_level(led->pin, 0);
        led->mode = LED_MODE_OFF;
    }
}

// Set the LED mode and update the GPIO level accordingly
static void led_set_mode(led_t *led, led_mode_t mode) {
    led->mode = mode;
    gpio_set_level(led->pin, (mode == LED_MODE_ON) ? 1 : 0);
}

// Task function for blinking an LED with a specified period
static void led_blink_task(void *param) {
    // Cast the parameter to the blink_led_t structure
    blink_led_t *cfg = (blink_led_t *)param;
    led_t *led = cfg->led;
    uint32_t *delay_ms = &cfg->period_ms;

    TickType_t last_wake = xTaskGetTickCount();

    for (;;) {
        led_toggle(led);
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(*delay_ms));
    }
}

// Helper function to check if a task was created successfully if not asserts and halts the program
static inline void check_task_created(BaseType_t result) {
  configASSERT(result == pdPASS);
}

static led_t led_red;
static led_t led_green;
static led_t led_blue;

void app_main(void) {
    // Initialize the LED structures and configure the GPIO pins
    led_init(&led_red, LED_RED_PIN);
    led_init(&led_green, LED_GREEN_PIN);
    led_init(&led_blue, LED_BLUE_PIN);

    // Create blink_led_t configurations for each LED
    static const blink_led_t red_cfg = {.led = &led_red, .period_ms = LED_RED_DELAY_MS};
    static const blink_led_t green_cfg = {.led = &led_green, .period_ms = LED_GREEN_DELAY_MS};
    static const blink_led_t blue_cfg = {.led = &led_blue, .period_ms = LED_BLUE_DELAY_MS};

    // Create tasks for each LED blinking with their respective configurations
    check_task_created(xTaskCreate(led_blink_task, "led_red_task", LED_TASK_STACK_BYTES, (void *)&red_cfg, LED_TASK_PRIORITY, NULL));
    check_task_created(xTaskCreate(led_blink_task, "led_green_task", LED_TASK_STACK_BYTES, (void *)&green_cfg, LED_TASK_PRIORITY, NULL));
    check_task_created(xTaskCreate(led_blink_task, "led_blue_task", LED_TASK_STACK_BYTES, (void *)&blue_cfg, LED_TASK_PRIORITY, NULL));
}