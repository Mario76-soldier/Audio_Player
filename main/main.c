#include <stdio.h>
#include "button_event.h"
#include "sd_custom.h"
#include "sound_task.h"
#include "esp_sleep.h"

void app_main(void)
{

    sd_custom_mount();
    sound_init();
    sound_task();

    esp_deep_sleep_enable_gpio_wakeup(1ULL<<BUTTON1, ESP_GPIO_WAKEUP_GPIO_LOW);

    esp_deep_sleep_start();
}
