#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include <stdio.h>

FILE *open_file(const char *path, const char *mode);
int close_file(FILE *file);
int copy_file(const char *src_path, const char *dst_path);
int copy_pages(const char *src_dir, const char *dest_dir);

int create_directory(const char *dir_path);
int copy_directory(const char *src_dir, const char *dst_dir);
int delete_directory(const char *dir_name);

#endif /* FILE_OPERATIONS_H */
