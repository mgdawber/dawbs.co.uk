#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "main.h"

#define BUFFER_SIZE 4096
#define PATH_MAX 4096
#define MAX_HEADER_LENGTH 10000
#define MAX_FOOTER_LENGTH 1024

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

int copy_pages(const char *src_dir, const char *dest_dir) {
  DIR *dir;
  char src_path[PATH_MAX];
  char dest_path[PATH_MAX];
  struct dirent *entry;
  struct stat st;

  // Open the source directory
  if ((dir = opendir(src_dir)) == NULL) {
    perror("opendir");
    return 1;
  }

  // Read the contents of the source directory
  while ((entry = readdir(dir)) != NULL) {
    // Skip ".";
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
      FILE *src_file, *dest_file;

      char buffer[BUFSIZ];
      size_t output_buffer_size = BUFSIZ * sizeof(char);
      size_t total_size = 0;
      char *output_buffer = malloc(output_buffer_size);

      // Open the files
      src_file = open_file(src_path, "rb");
      dest_file = open_file(dest_path, "wb");

      char header_text[10000] = "";

      snprintf(header_text, MAX_HEADER_LENGTH,
               "<!DOCTYPE html>"
               "<html lang='en'>"
               "<head>"
               "<meta charset='utf-8'>"
               "<title>Dawbs — Home</title>"
               "<meta name='viewport' content='width=device-width, "
               "initial-scale=1'>"
               "<meta name='description' content='Personal memex of "
               "Matthew Dawber.'>"
               "<link rel='stylesheet' href='media/content/base.css'>"
               "</head>"
               "<body>"
               "<nav>"
               "<details open>"
               "<summary>Menu"
               "</summary>"
               "<section class='site-nav'>"
               "<section>"
               "<ul class='no-bullet'>"
               "<li><a href='index.html'>Home</a></li>"
               "<li><a href='about.html'>About</a></li>"
               "<li><a href='log.html'>Log</a></li>"
               "</ul>"
               "</section>"
               "<section>"
               "<h3>travel</h3>"
               "<ul class='no-bullet'>"
               "<li><a href='japan.html'>Japan</a></li>"
               "</ul>"
               "</section>"
               "<section>"
               "<h3>meta</h3>"
               "<ul class='no-bullet'>"
               "<li><a href='meta.html'>Index</a></li>"
               "</ul>"
               "</section>"
               "</ul>"
               "</section>"
               "</details>"
               "</nav>"
               "<main>");

      if (total_size + strlen(header_text) > output_buffer_size) {
        output_buffer_size *= 2;
        output_buffer = realloc(output_buffer, output_buffer_size);
      }
      memcpy(output_buffer + total_size, header_text, strlen(header_text));
      total_size += strlen(header_text);

      // Copy the file contents to the output buffer
      while (fgets(buffer, sizeof(buffer), src_file) != NULL) {
        if (strlen(buffer) > 1) {
          if (total_size + strlen(buffer) > output_buffer_size) {
            output_buffer_size *= 2;
            char *new_buffer = realloc(output_buffer, output_buffer_size);
            if (new_buffer == NULL) {
              // Handle the error
              printf("Error: Failed to allocate memory\n");
              exit(1);
            }
            output_buffer = new_buffer;
          }
          memcpy(output_buffer + total_size, buffer, strlen(buffer));
          total_size += strlen(buffer);
        }
      }

      if (strcmp("./pages/index.html", src_path) == 0) {
        FILE *log_file = open_file("pages/log.html", "r");

        // Copy the log file contents to the output buffer
        while (fgets(buffer, sizeof(buffer), log_file) != NULL) {
          if (strlen(buffer) > 1) {
            if (total_size + strlen(buffer) > output_buffer_size) {
              output_buffer_size *= 2;
              output_buffer = realloc(output_buffer, output_buffer_size);
            }
            memcpy(output_buffer + total_size, buffer, strlen(buffer));
            total_size += strlen(buffer);
          }
        }

        // Close the log file
        fclose(log_file);
      }

      char footer_text[1024] = "";
      char *timeStr = ctime(&(st.st_mtime));

      snprintf(footer_text, MAX_FOOTER_LENGTH,
               "</main>"
               "<footer>"
               "<hr>"
               "<div>"
               "<span class='bold'>© 2023</span>"
               "<span> - <a "
               "href='https://creativecommons.org/licenses/by-nc-sa/"
               "4.0/'>BY-NC-SA 4.0</a></span>"
               "</div>"
               "<span>Last edited: ");

      strcat(footer_text, timeStr);

      strcat(footer_text,
             "<a href='https://git.sr.ht/~dawbs/dawbs.co.uk'>Edit</a>"
             "</span>"
             "</footer>"
             "</body>"
             "</html>");

      if (total_size + strlen(footer_text) > output_buffer_size) {
        output_buffer_size *= 2;
        output_buffer = realloc(output_buffer, output_buffer_size);
      }
      memcpy(output_buffer + total_size, footer_text, strlen(footer_text));
      total_size += strlen(footer_text);

      // Write the output buffer to the dest file
      fwrite(output_buffer, sizeof(char), total_size, dest_file);

      // Free the memory allocated for the output buffer
      if (output_buffer != NULL) {
        free(output_buffer);
        output_buffer = NULL; // Set the pointer to NULL after freeing
      }

      // Close the files
      if (close_file(src_file) || close_file(dest_file)) {
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
