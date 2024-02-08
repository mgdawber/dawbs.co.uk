#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "main.h"

#define _GNU_SOURCE
#define PATH_MAX 4096

int create_directory(const char *dir_path) {
  struct stat st = {0};
  if (stat(dir_path, &st) == -1) {
    if (mkdir(dir_path, 0777) == -1) {
      fprintf(stderr, "Error creating directory %s: %s\n", dir_path,
              strerror(errno));
      return 1;
    }
  }
  return 0;
}

int copy_directory(const char *src_dir, const char *dst_dir) {
  DIR *dir;
  struct dirent *entry;
  struct stat statbuf;
  char src_path[PATH_MAX];
  char dst_path[PATH_MAX];

  // Open the source directory
  if ((dir = opendir(src_dir)) == NULL) {
    perror("opendir");
    return 1;
  }

  // Create the destination directory
  if (mkdir(dst_dir, 0777) == -1) {
    perror("mkdir");
    return 1;
  }

  // Read the contents of the source directory
  while ((entry = readdir(dir)) != NULL) {
    // Skip "." and ".." entries
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    // Construct the source and destination paths
    snprintf(src_path, sizeof(src_path), "%s/%s", src_dir, entry->d_name);
    snprintf(dst_path, sizeof(dst_path), "%s/%s", dst_dir, entry->d_name);

    // Get information about the file or directory
    if (stat(src_path, &statbuf) == -1) {
      perror("stat");
      return 1;
    }

    // Recursively copy directories
    if (S_ISDIR(statbuf.st_mode)) {
      copy_directory(src_path, dst_path);
    }
    // Copy files
    else {
      FILE *src_file, *dst_file;
      char buffer[BUFSIZ];
      size_t n;

      src_file = open_file(src_path, "rb");
      dst_file = open_file(dst_path, "wb");

      while ((n = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        if (fwrite(buffer, 1, n, dst_file) != n) {
          perror("fwrite");
          return 1;
        }
      }

      // Close the files
      if (close_file(src_file) || close_file(dst_file)) {
        exit(EXIT_FAILURE);
      }
    }
  }

  // Close the source directory
  if (closedir(dir) == -1) {
    perror("closedir");
    return 1;
  }

  return 0;
}

int delete_directory(const char *dir_name) {
  DIR *dir;
  struct dirent *entry;
  char path[PATH_MAX];

  // Open the directory
  if ((dir = opendir(dir_name)) == NULL) {
    return 0;
  }

  // Delete the contents of the directory
  while ((entry = readdir(dir)) != NULL) {
    // Skip "." and ".." entries
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    // Construct the path to the file or directory
    snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);

    // Recursively delete directories
    if (entry->d_type == DT_DIR) {
      delete_directory(path);
    }
    // Delete files
    else {
      if (remove(path) == -1) {
        perror("remove");
        return 1;
      }
    }
  }

  // Close the directory
  if (closedir(dir) == -1) {
    perror("closedir");
    return 1;
  }

  // Delete the directory itself
  if (rmdir(dir_name) == -1) {
    perror("rmdir");
    return 1;
  }

  return 0;
}
