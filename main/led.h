#include "led_strip.h"

#define LED_STRIP_RGB_GPIO  15
#define LED_STRIP_LED_NUMBERS 8
/*
 * 初始化那8个ws2812
 * 底层由espressif/led_strip实现，
 * 用的spi模拟时序
 * SPI3_HOST
 */
led_strip_handle_t led_init(void);