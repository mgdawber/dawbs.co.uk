#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include <stdio.h>

FILE *openFile(const char *path, const char *mode);
int closeFile(FILE *file);
int copy_file(const char *src_path, const char *dst_path);

int delete_dir(const char *dir_name);
int copy_dir(const char *src_dir, const char *dst_dir);

#endif /* FILE_OPERATIONS_H */
