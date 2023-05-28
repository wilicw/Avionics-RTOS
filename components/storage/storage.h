#ifndef __STOR_H__
#define __STOR_H__

#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>

#include "bsp.h"

#define STOR_PREFIX "FT_"

void storage_init(char *);
void storage_read(char *, size_t);
void storage_write(uint8_t *, size_t);
void storage_flush();
FILE *storage_fetch();
void storage_deinit();
void storage_write_config(const char *, void *, size_t);
void storage_read_config(const char *, void *, size_t);

#endif
