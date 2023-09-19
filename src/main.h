#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *openFile(const char *path, const char *mode);
int closeFile(FILE *file);
int copy_file(const char *src_path, const char *dst_path);

int delete_dir(const char *dir_name);
int copy_dir(const char *src_dir, const char *dst_dir);
