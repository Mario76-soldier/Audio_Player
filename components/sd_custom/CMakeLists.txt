idf_component_register(
    SRCS "sd_custom.c"
    INCLUDE_DIRS "include"
    REQUIRES fatfs vfs sdmmc esp_system
)