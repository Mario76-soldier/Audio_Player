#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>

#ifndef _SD_CUSTOM_H_
#define _SD_CUSTOM_H_

extern char *file_custom_list[500];
extern int file_custom_count;

void sd_custom_mount(void);

void sd_random_file(char *buffer, size_t len);

#endif