#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"

#define BUFFER_SIZE 4096

FILE *open_file(const char *path, const char *mode) {
  FILE *file = fopen(path, mode);
  if (file == NULL) {
    fprintf(stderr, "Error opening file '%s' with mode '%s': %s\n", path, mode,
            strerror(errno));

    return NULL;
  }
  return file;
}

int close_file(FILE *file) {
  if (file == NULL) {
    fprintf(stderr, "Error: NULL file pointer passed to close_file\n");
    return 1;
  }

  if (fclose(file) == EOF) {
    fprintf(stderr, "Error closing file: fclose failed\n");
    return 1;
  }

  return 0;
}

int copy_file(const char *src_path, const char *dst_path) {
  FILE *src_file = open_file(src_path, "rb");
  if (src_file == NULL) {
    perror("Error opening source file");
    return 1;
  }

  FILE *dst_file = open_file(dst_path, "wb");
  if (dst_file == NULL) {
    perror("Error opening destination file");
    fclose(src_file);
    return 1;
  }

  char buffer[BUFFER_SIZE];
  size_t bytes_read;

  while ((bytes_read = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
    size_t bytes_written = fwrite(buffer, 1, bytes_read, dst_file);
    if (bytes_written != bytes_read) {
      perror("Error writing to destination file");
      fclose(src_file);
      fclose(dst_file);
      return 1;
    }
  }

  if (close_file(src_file) == EOF || close_file(dst_file) == EOF) {
    perror("Error closing files");
    return 1;
  }

  return 0;
}
