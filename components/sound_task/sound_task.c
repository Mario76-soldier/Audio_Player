#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_mac.h"
#include "driver/i2s_std.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sound_task.h"
#include "sd_custom.h"
#include "esp_timer.h"
#include "button_event.h"

static const char *TAG = "sound_player";

TaskHandle_t sound_handle=NULL;

// I2S 채널 핸들
#define I2S_BCLK 1
#define I2S_LRCLK 3
#define I2S_DOUT 0
#define AUDIO_BUFFER 2048
static i2s_chan_handle_t tx_handle = NULL;

// I2S + 내장 DAC 초기화
static esp_err_t sound_config_init(uint32_t sample_rate)
{
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &tx_handle, NULL));

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(sample_rate),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk  = I2S_BCLK,
            .ws   = I2S_LRCLK,
            .dout = I2S_DOUT,                     // 핵심: dout 사용 안함 -> DAC 출력 사용
            .din  = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            }
        }
    };

    ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
    return ESP_OK;
}

// WAV 파일 재생 (8bit unsigned PCM, 44바이트 헤더 있음)
static void play_wav(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open %s", path);
        return;
    }

    fseek(f, 44, SEEK_SET); // WAV 헤더 skip

    int16_t *buf = calloc(AUDIO_BUFFER, sizeof(int16_t));
    size_t bytes_read = 0;
    size_t bytes_written = 0;
    bytes_read = fread(buf, sizeof(int16_t), AUDIO_BUFFER, f);

    ESP_ERROR_CHECK(i2s_channel_enable(tx_handle));

    while (bytes_read > 0)
    {
        // write the buffer to the i2s
        i2s_channel_write(tx_handle, buf, bytes_read * sizeof(int16_t), &bytes_written, portMAX_DELAY);
        bytes_read = fread(buf, sizeof(int16_t), AUDIO_BUFFER, f);
        ESP_LOGV(TAG, "Bytes read: %d", bytes_read);
    }
    i2s_channel_disable(tx_handle);
    free(buf);

    fclose(f);
    ESP_LOGI(TAG, "Playback finished.");
}

void sound_init(void){
    sound_config_init(48000);
}

void sound_task(void){
    
    char sound_name[SOUND_NAME_SIZE];
    sd_random_file(sound_name, SOUND_NAME_SIZE);
    play_wav(sound_name);
}