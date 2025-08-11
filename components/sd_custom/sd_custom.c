
#include "sd_custom.h"
#include <dirent.h>
#include "esp_random.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "esp_system.h"
#if SOC_SDMMC_IO_POWER_EXTERNAL
#include "sd_pwr_ctrl_by_on_chip_ldo.h"
#endif

#define EXAMPLE_MAX_CHAR_SIZE    64

static const char *TAG = "example";

#define MOUNT_POINT "/sdcard"

// Pin assignments can be set in menuconfig, see "SD SPI Example Configuration" menu.
// You can also change the pin assignments here by changing the following 4 lines.
#define PIN_NUM_MISO  GPIO_NUM_5
#define PIN_NUM_MOSI  GPIO_NUM_6
#define PIN_NUM_CLK   GPIO_NUM_4
#define PIN_NUM_CS    GPIO_NUM_7

char *file_custom_list[500]={};
int file_custom_count = 0;

void sd_custom_mount(void)
{
    esp_err_t ret;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    sdmmc_card_t *card;
    const char mount_point[] = MOUNT_POINT;
    ESP_LOGI(TAG, "Initializing SD card");
    ESP_LOGI(TAG, "Using SPI peripheral");
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };

    ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return;
    }

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return;
    }
    ESP_LOGI(TAG, "Filesystem mounted");

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);

    DIR *dir;
    struct dirent *entry;

    dir=opendir(MOUNT_POINT);
    if(!dir){
        ESP_LOGE(TAG, "Failed to open directory %s", MOUNT_POINT);
        return;
    }

    while ((entry = readdir(dir)) != NULL && file_custom_count < 500) {
        if (entry->d_type == DT_REG) { // 일반 파일만
            file_custom_list[file_custom_count] = strdup(entry->d_name); // 복사
            file_custom_count++;
        }
    }
    closedir(dir);

    if (file_custom_count == 0) {
        ESP_LOGW(TAG, "No files found in directory: %s", MOUNT_POINT);
        return;
    }
}

void sd_random_file(char *buffer, size_t len)
{
    uint32_t rand_idx = esp_random() % file_custom_count;
    ESP_LOGI(TAG, "Selected file: %s", file_custom_list[rand_idx]);
    snprintf(buffer, len, "%s/%s", MOUNT_POINT, file_custom_list[rand_idx]);
}