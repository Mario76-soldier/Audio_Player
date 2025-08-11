#include "button_event.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "Button";

void button_event_init(void)
{
    // Initialize GPIO for button events
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = ((1ULL << BUTTON1)),
        .pull_up_en = GPIO_PULLUP_ENABLE,   // LOW 트리거 시 풀업 필요
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    // Configure the GPIO pins
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    ESP_LOGI(TAG, "Button event initialization complete.");
}