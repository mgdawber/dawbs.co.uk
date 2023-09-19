#include <dirent.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wchar.h>

#include "main.h"

int build_templates(const char *src_dir, const char *dest_dir) {
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
      FILE *src_file, *dest_file;

      char buffer[BUFSIZ];
      size_t output_buffer_size = BUFSIZ * sizeof(char);
      size_t total_size = 0;
      char *output_buffer = malloc(output_buffer_size);

      // Open the files
      src_file = openFile(src_path, "rb");
      dest_file = openFile(dest_path, "wb");

      char *head_text =
          "<!DOCTYPE html>"
          "<html lang='en'>"
          "<head>"
          "<meta charset='utf-8'>"
          "<title>Dawbs — Home</title>"
          "<meta name='viewport' content='width=device-width, initial-scale=1'>"
          "<link href='static/output.css' type='text/css' rel='stylesheet'>"
          "<link rel='icon' type='image/svg' sizes='32x32' "
          "href='static/icon.svg'>"
          "<link rel='icon' type='image/svg' sizes='16x16' "
          "href='static/icon.svg'>"
          "</head>"
          "<body class='flex flex-col min-h-screen'>"
          "<img class='p-10 w-[7rem]' src='static/icon.svg' alt='A black square logo.' />"
          "<hr class='border-t border-black mx-10'>"
          "<div class='flex flex-col sm:flex-row'>"
          "<nav>"
          "<details class='p-10 pb-[0px]' open>"
          "<summary class='sm:mb-6'>Menu</summary>"
          "<ul>"
          "<li>"
          "<a href='index.html'>Home</a>"
          "</li>"
          "<li>"
          "<a href='illustrations.html'>Illustrations</a>"
          "</li>"
          "<h3 class='hidden sm:block'>blog</h3>"
          "<li>"
          "<a href='log.html'>Log</a>"
          "</li>"
          "</ul>"
          "</details>"
          "</nav>";
      if (total_size + strlen(head_text) > output_buffer_size) {
        output_buffer_size *= 2;
        output_buffer = realloc(output_buffer, output_buffer_size);
      }
      memcpy(output_buffer + total_size, head_text, strlen(head_text));
      total_size += strlen(head_text);

      // Copy the file contents to the output buffer
      while (fgets(buffer, sizeof(buffer), src_file) != NULL) {
        if (strlen(buffer) > 1) {
          output_buffer = realloc(output_buffer, total_size + strlen(buffer));
          memcpy(output_buffer + total_size, buffer, strlen(buffer));
          total_size += strlen(buffer);
        }
      }

      // Add the extra text to the output buffer
      char *footer_text =
          "</div> <footer class='w-full h-20 mt-auto mb-8 py-2 px-10 pt-10'>"
          "<hr class='mb-2 h-1 border-t border-black'>"
          "<div class='flex flex-col sm:flex-row'>"
          "<div>"
          "<p class='mb-2 sm:mb-0 font-bold'>Dawbs © 2023</p>"
          "<div>"
          "<a href='https://sr.ht/~dawbs/'>sr.ht </a>"
          "</div> </div> <div class='sm:ml-auto mb-4'> Last "
          "edited on 27st April, 2023</div></div></footer></body></html>";
      if (total_size + strlen(footer_text) > output_buffer_size) {
        output_buffer_size *= 2;
        output_buffer = realloc(output_buffer, output_buffer_size);
      }
      memcpy(output_buffer + total_size, footer_text, strlen(footer_text));
      total_size += strlen(footer_text);

      // Write the output buffer to the dest file
      fwrite(output_buffer, sizeof(char), total_size, dest_file);

      free(output_buffer);

      // Close the files
      if (closeFile(src_file) || closeFile(dest_file)) {
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
