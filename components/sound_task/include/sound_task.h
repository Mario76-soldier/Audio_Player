#ifndef _SOUND_TASK_H_
#define _SOUND_TASK_H_

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SOUND_NAME_SIZE 256

#define SOUND_SAMPLE_RATE 16000

#define SOUND_BASE_DIR "/spiflash/sound/"

extern TaskHandle_t sound_handle;

void sound_init(void);

void sound_task(void);

#endif