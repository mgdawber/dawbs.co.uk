#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "main.h"

int build_pages(const char *src_dir, const char *dest_dir) {
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
      FILE *src_file, *dest_file, *style_file;

      char buffer[BUFSIZ];
      char style_buffer[10000];
      size_t output_buffer_size = BUFSIZ * sizeof(char);
      size_t total_size = 0;
      char *output_buffer = malloc(output_buffer_size);

      // Open the files
      src_file = openFile(src_path, "rb");
      dest_file = openFile(dest_path, "wb");
      style_file = openFile("./base.css", "rb");

      char header_text[10000] = "";
      strcat(header_text, "<!DOCTYPE html>");
      strcat(header_text, "<html lang='en'>");
      strcat(header_text, "<head>");
      strcat(header_text, "<meta charset='utf-8'>");
      strcat(header_text, "<title>Dawbs — Home</title>");
      strcat(header_text, "<meta name='viewport' content='width=device-width, initial-scale=1'>");
      strcat(header_text, "<meta name='description' content='Personal memex of Matthew Dawber.'>");
      strcat(header_text, "<link rel='shortcut icon' type='image/svg+xml' ");
      strcat(header_text, "href='data:image/svg+xml,<svg ");
      strcat(header_text, "xmlns=%22http://www.w3.org/2000/svg%22 viewBox=%220 0 100 ");
      strcat(header_text, "100%22><text y=%22.9em%22 font-size=%2290%22>🦝</text></svg>'>");
      strcat(header_text, "<style>");

      /* // Copy the styles to the header. */ 
      while (fgets(style_buffer, sizeof(style_buffer), style_file) != NULL) {
          char *colon = strchr(style_buffer, ';');
          if (colon != NULL) {
              strncat(header_text, style_buffer, colon - buffer + 1);
              for (int i = colon - style_buffer + 1; i < strlen(style_buffer); i++) {
                  if (!isspace(style_buffer[i])) {
                      strncat(header_text, &style_buffer[i], 1);
                  }
              }
          } else {
              strcat(header_text, style_buffer);
          }
      }

      strcat(header_text, "</style>");
      strcat(header_text, "<link href='data:text/");
      strcat(header_text, "css,%0A%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20ddg-runtime-");
      strcat(header_text, "checks%20%7B%0A%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%");
      strcat(header_text, "20%20%20display%3A%20none%3B%0A%20%20%20%20%20%20%20%20%20%20%20%20%");
      strcat(header_text, "20%20%20%20%7D%0A%20%20%20%20%20%20%20%20%20%20%20%20'");
      strcat(header_text, "rel='stylesheet' type='text/css'>");
      strcat(header_text, "</head>");
      strcat(header_text, "<body class='min-h-screen'>");
      strcat(header_text, "<div>");
      strcat(header_text, "<button id='navbar_button' class='navbar-button sm:hidden'>");
      strcat(header_text, "<img src='media/content/navbar_icon.svg' />");
      strcat(header_text, "</button>");
      strcat(header_text, "<div id='mobile_navbar' class='mobile-navbar hidden'>");
      strcat(header_text, "<nav class='text-sm flex flex-col'>");
      strcat(header_text, "<a href='index.html'>Home</a>");
      strcat(header_text, "<a href='about.html'>About</a>");
      strcat(header_text, "<a href='watching.html'>Watching</a>");
      strcat(header_text, "<a href='log.html'>Log</a>");
      strcat(header_text, "</nav>");
      strcat(header_text, "</div>");
      strcat(header_text, "<div class='flex'>");
      strcat(header_text, "<nav class='flex-col hidden sm:flex'>");
      strcat(header_text, "<a href='index.html'>Home</a>");
      strcat(header_text, "<a href='about.html'>About</a>");
      strcat(header_text, "<a href='watching.html'>Watching</a>");
      strcat(header_text, "<a href='log.html'>Log</a>");
      strcat(header_text, "</nav>");
      strcat(header_text, "<main>");

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
        FILE *log_file = openFile("pages/log.html", "r");

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
      char* timeStr = ctime(&(st.st_mtime));

      strcat(footer_text, "</main>");
      strcat(footer_text, "</div>");
      strcat(footer_text, "<footer>");
      strcat(footer_text, "<div>");
      strcat(footer_text, "<span class='bold'>© 2023</span>");
      strcat(footer_text, "<span> - <a href='https://creativecommons.org/licenses/by-nc-sa/4.0/'>BY-NC-SA 4.0</a></span>");
      strcat(footer_text, "</div>");
      strcat(footer_text, "<span>Last edited: ");
      strcat(footer_text, timeStr);
      strcat(footer_text, "<a href='https://git.sr.ht/~dawbs/dawbs.co.uk'>Edit</a>");
      strcat(footer_text, "</span>");
      strcat(footer_text, "</footer>");
      strcat(footer_text, "</body>");
      strcat(footer_text, "<script src='media/content/navbar-toggle.js'></script>");
      strcat(footer_text, "</html>");

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
      if (closeFile(src_file) || closeFile(dest_file) || closeFile(style_file)) {
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
  const char *content_dir = "./media/content";
  const char *pages_dir = "./pages";
  const char *build_dir = "./build";
  const char *build_media_dir = "./build/media";
  const char *build_content_dir = "./build/media/content";

  // Delete the build directory
  delete_dir(build_dir);

  // Create the build directory
  if (mkdir(build_dir, 0777) == -1) {
    perror("mkdir");
    return 1;
  }
  // Create the media directory
  if (mkdir(build_media_dir, 0777) == -1) {
    perror("mkdir");
    return 1;
  }

  // Copy the assets
  if (copy_dir(content_dir, build_content_dir) != 0) {
    fprintf(stderr, "Error copying directory\n");
    return 1;
  }

  // Copy the pages
  if (build_pages(pages_dir, build_dir) != 0) {
    fprintf(stderr, "Error copying directory\n");
    return 1;
  }

  printf("\033[1;32mThe build was successful\033[0m\n");
  exit(EXIT_SUCCESS);
}
