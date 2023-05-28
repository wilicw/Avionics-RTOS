#include "storage.h"

#define TAG "STOR"

static FILE *f;

void storage_init(char *_fn) {
  ESP_LOGI(TAG, "Init storage");
  char fn[32] = SD_MOUNT "/" STOR_PREFIX "0000.txt";
  f = NULL;
  if (_fn == NULL) {
    uint16_t i = 0;
    struct stat st;
    while (stat(fn, &st) == 0)
      sprintf(fn, SD_MOUNT "/" STOR_PREFIX "%04d.txt", ++i);
    f = fopen(fn, "a+");
  } else {
    f = fopen(fn, "a+");
  }
  if (f == NULL)
    esp_restart();
  printf("Open %s %d\n", fn, fileno(f));
}

void storage_read(char *ptr, size_t len) {
  if (f == NULL) return;
}

void storage_write(uint8_t *ptr, size_t len) {
  if (f == NULL) return;
  fwrite(ptr, len, 1, f);
}

void storage_flush() {
  if (f == NULL) return;
  fflush(f);
  fsync(fileno(f));
}

FILE *storage_fetch() {
  return f;
}

void storage_deinit() {
  if (f == NULL) return;
  fclose(f);
}

void storage_write_config(const char *path, void *data, size_t len) {
  FILE *f = fopen(path, "w+");
  fwrite(data, len, 1, f);
  fclose(f);
}

void storage_read_config(const char *path, void *data, size_t len) {
  FILE *f = fopen(path, "a+");
  fseek(f, SEEK_SET, 0);
  fread(data, len, 1, f);
  fclose(f);
}
