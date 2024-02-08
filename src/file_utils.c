#include <stdlib.h>

#include "main.h"

FILE *open_file(const char *path, const char *mode) {
  FILE *file = fopen(path, mode);
  if (file == NULL) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }
  return file;
}

int close_file(FILE *file) {
  if (fclose(file) == EOF) {
    perror("fclose");
    return 1;
  }
  return 0;
}

int copy_file(const char *src_path, const char *dst_path) {
  FILE *src_file, *dst_file;
  char buffer;
  size_t bytes_read;

  src_file = open_file(src_path, "rb");
  dst_file = open_file(dst_path, "wb");

  while ((bytes_read = fread(&buffer, 1, sizeof(buffer), src_file)) > 0) {
    if (fwrite(&buffer, 1, bytes_read, dst_file) != bytes_read) {
      perror("Error writing to destination file");
      if (fclose(src_file) || fclose(dst_file)) {
          return 1;
      }
    }
  }

  // Close the files
  if (close_file(src_file) || close_file(dst_file)) {
    exit(EXIT_FAILURE);
  }

  return 0;
}
