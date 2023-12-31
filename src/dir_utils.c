#define _GNU_SOURCE

#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "main.h"

int delete_dir(const char *dir_name) {
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
      delete_dir(path);
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

int copy_dir(const char *src_dir, const char *dst_dir) {
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
      copy_dir(src_path, dst_path);
    }
    // Copy files
    else {
      FILE *src_file, *dst_file;
      char buffer[BUFSIZ];
      size_t n;

      src_file = openFile(src_path, "rb");
      dst_file = openFile(dst_path, "wb");

      while ((n = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        if (fwrite(buffer, 1, n, dst_file) != n) {
          perror("fwrite");
          return 1;
        }
      }

      // Close the files
      if (closeFile(src_file) || closeFile(dst_file)) {
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
