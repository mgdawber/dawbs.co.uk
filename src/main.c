#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "main.h"

int build_templates(const char *src_dir, const char *dest_dir) {
  DIR *dir;
  struct dirent *entry;
  struct stat st;
  char src_path[PATH_MAX];
  char dest_path[PATH_MAX];

  // Open the source directory
  if ((dir = opendir(src_dir)) == NULL) {
    perror("opendir");
    return 1;
  }

  // Read the contents of the source directory
  while ((entry = readdir(dir)) != NULL) {
    // Skip "."
    if (strcmp(entry->d_name, ".") == 0) {
      continue;
    }

    // and ".." entries
    if (strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    // Skip files beginning with "_"
    if (strncmp(entry->d_name, "_", 1) == 0) {
      continue;
    }

    // Construct the source and destination paths
    snprintf(src_path, sizeof(src_path), "%s/%s", src_dir, entry->d_name);
    snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_dir, entry->d_name);

    // Get the file status
    if (stat(src_path, &st) == -1) {
      perror("stat");
      return 1;
    }

    // Copy regular files
    else if (S_ISREG(st.st_mode)) {
      FILE *src_file, *dest_file, *head_file, *footer_file;
      const char *head_path = "./templates/_head.html";
      const char *footer_path = "./templates/_footer.html";
      char buffer[BUFSIZ];
      char *output_buffer = NULL;
      size_t n;
      size_t total_size = 0;

      // Open the files
      head_file = openFile(head_path, "rb");
      src_file = openFile(src_path, "rb");
      footer_file = openFile(footer_path, "rb");
      dest_file = openFile(dest_path, "wb");

      // Copy the head file contents to the output buffer
      while ((n = fread(buffer, 1, sizeof(buffer), head_file)) > 0) {
        output_buffer = realloc(output_buffer, total_size + n);
        memcpy(output_buffer + total_size, buffer, n);
        total_size += n;
      }

      // Copy the file contents to the output buffer
      while ((n = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        output_buffer = realloc(output_buffer, total_size + n);
        memcpy(output_buffer + total_size, buffer, n);
        total_size += n;
      }

      // Copy the footer file contents to the output buffer
      while ((n = fread(buffer, 1, sizeof(buffer), footer_file)) > 0) {
        output_buffer = realloc(output_buffer, total_size + n);
        memcpy(output_buffer + total_size, buffer, n);
        total_size += n;
      }

      size_t output_size = strlen(output_buffer);

      // Write the minimized output buffer to the dest file
      if (fwrite(output_buffer, 1, output_size, dest_file) != output_size) {
        perror("fwrite");
        exit(EXIT_FAILURE);
      }

      free(output_buffer);

      // Close the files
      if (closeFile(head_file) || closeFile(src_file) ||
          closeFile(footer_file) || closeFile(dest_file)) {
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

int main() {
  const char *src_dir = "./static";
  const char *build_dir = "./build";
  const char *dst_dir = "./build/static";
  const char *templates_dir = "./templates";

  // Delete the build directory
  delete_dir(build_dir);

  // Create the build directory
  if (mkdir(build_dir, 0777) == -1) {
    perror("mkdir");
    return 1;
  }

  // Copy the static assets
  if (copy_dir(src_dir, dst_dir) != 0) {
    fprintf(stderr, "Error copying directory\n");
    return 1;
  }

  // Copy the templates
  if (build_templates(templates_dir, build_dir) != 0) {
    fprintf(stderr, "Error copying directory\n");
    return 1;
  }

  printf("\033[1;32mThe build was successful\033[0m\n");
  exit(EXIT_SUCCESS);
}
