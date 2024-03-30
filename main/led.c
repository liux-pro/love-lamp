#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "led.h"


static const char *TAG = "led strip";
led_strip_handle_t led;


void led_init(void) {
    // LED strip general initialization, according to your led board design
    led_strip_config_t strip_config = {
            .strip_gpio_num = LED_STRIP_RGB_GPIO,   // The GPIO that connected to the LED strip's data line
            .max_leds = LED_STRIP_LED_NUMBERS,        // The number of LEDs in the strip,
            .led_pixel_format = LED_PIXEL_FORMAT_GRB, // Pixel format of your LED strip
            .led_model = LED_MODEL_WS2812,            // LED strip model
            .flags.invert_out = false,                // whether to invert the output signal
    };

    // LED strip backend configuration: SPI
    led_strip_spi_config_t spi_config = {
            .clk_src = SPI_CLK_SRC_DEFAULT, // different clock source can lead to different power consumption
            .flags.with_dma = true,         // Using DMA can improve performance and help drive more LEDs
            .spi_bus = SPI3_HOST,           // SPI bus ID
    };

    ESP_ERROR_CHECK(led_strip_new_spi_device(&strip_config, &spi_config, &led));
    ESP_LOGI(TAG, "Created LED strip object with SPI backend");
}

void led_set_all(uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < LED_STRIP_LED_NUMBERS; ++i) {
        led_strip_set_pixel(led, i, r, g, b);
    }
    led_strip_refresh(led);
}

void led_set_all_hsv(uint16_t h, uint8_t s, uint8_t v) {
    for (int i = 0; i < LED_STRIP_LED_NUMBERS; ++i) {
        led_strip_set_pixel_hsv(led, i, h, s, v);
    }
    led_strip_refresh(led);
}