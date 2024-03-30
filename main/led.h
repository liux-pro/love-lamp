#include "led_strip.h"

#define LED_STRIP_RGB_GPIO  15
#define LED_STRIP_LED_NUMBERS 10

extern led_strip_handle_t led;

/*
 * 初始化那8个ws2812
 * 底层由espressif/led_strip实现，
 * 用的spi模拟时序
 * SPI3_HOST
 */
void led_init(void);

void led_set_all(uint8_t r, uint8_t g, uint8_t b);

void led_set_all_hsv(uint16_t h, uint8_t s, uint8_t v);
