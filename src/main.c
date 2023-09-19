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
          "<meta name='description' content='Personal memex of Matthew Dawber.'>"
          "<link rel='shortcut icon' type='image/svg+xml' "
          "href='data:image/svg+xml,<svg "
          "xmlns=%22http://www.w3.org/2000/svg%22 viewBox=%220 0 100 "
          "100%22><text y=%22.9em%22 font-size=%2290%22>🦝</text></svg>'>"
          "<style>"
          "@font-face {"
          "font-family: barlow;"
          "font-display: auto;"
          "font-weight: 500;"
          "src: url('media/fonts/Barlow-Medium.woff2');"
          "}"
          "@font-face {"
          "font-family: barlow;"
          "font-display: auto;"
          "src: url('media/fonts/Barlow-Bold.woff2');"
          "font-weight: 700;"
          "}"
          "*, ::after, ::before {"
          "box-sizing: border-box;"
          "border-width: 0;"
          "border: 0 solid white;"
          "color: black;"
          "}"
          "::selection {"
          "background-color: black;"
          "color: white;"
          "}"
          "a {"
          "text-decoration-line: underline;"
          "font-weight: bold;"
          "}"
          "a:hover {"
          "background-color: black;"
          "color: white;"
          "}"
          "body {"
          "margin: 0;"
          "line-height: inherit;"
          "font-family: barlow, Roboto, Arial, sans-serif;"
          "}"
          "code {"
          "font-family: Courier, monospace;"
          "font-size: 1em;"
          "color: white;"
          "}"
          "figure {"
          "margin: 0;"
          "}"
          "footer {"
          "margin-top: auto;"
          "width: 100%;"
          "background-color: black;"
          "}"
          "footer span {"
          "color: white;"
          "}"
          "html {"
          "line-height: 1.5;"
          "tab-size: 4;"
          "}"
          "h2 {"
          "margin-top: 1.5rem;"
          "margin-bottom: 1.5rem;"
          "font-size: 1.875rem;"
          "line-height: 2.25rem;"
          "font-weight: 700;"
          "}"
          "h3 {"
          "margin-top: 1.5rem;"
          "margin-bottom: 1.5rem;"
          "font-size: 1.5rem;"
          "line-height: 2rem;"
          "font-weight: 700;"
          "}"
          "img {"
          "display: block;"
          "height: auto;"
          "}"
          "main {"
          "width: 100%;"
          "padding: 0 1.25rem;"
          "margin: 1.25rem 0;"
          "max-width: 700px;"
          "}"
          "nav {"
          "padding: 1.25rem;"
          "display: grid;"
          "position: relative;"
          "grid-template-columns: repeat(2, min-content);"
          "gap: 0 1.25rem;"
          "border-bottom: 5px solid black;"
          "}"
          "nav a {"
          "padding: 0 0.25rem;"
          "}"
          "footer a {"
          "margin-right: 1rem;"
          "color: white"
          "}"
          "footer a:hover {"
          "background-color: white;"
          "color: black;"
          "}"
          "p {"
          "margin: 1.5rem 0;"
          "}"
          "pre {"
          "margin-top: 1.5rem 0;"
          "white-space: pre-wrap;"
          "overflow-wrap: break-word;"
          "background-color: black;"
          "padding: 0.75rem;"
          "}"
          "ul {"
          "padding-left: 1.25rem;"
          "margin-top: 1.5rem;"
          "margin-bottom: 1.5rem;"
          "list-style-type: square;"
          "}"
          ".bold {"
          "font-weight: bold;"
          "}"
          ".border-2 {"
          "border: 5px solid black;"
          "max-width: 600px;"
          "padding: 1.5rem;"
          "}"
          ".columns-2 {"
          "columns: 2;"
          "}"
          ".decorative:after {"
          "left: 0;"
          "right: 0;"
          "top: 0;"
          "bottom: 0;"
          "display: block;"
          "height: 10px;"
          "content: ' ';"
          "background: url('media/content/decorative.png');"
          "}"
          ".flex {"
          "display: flex;"
          "}"
          ".flex-col {"
          "flex-direction: column;"
          "}"
          ".grid {"
          "display: grid;"
          "grid-template-columns: repeat(2, min-content);"
          "gap: 1.25rem;"
          "}"
          ".grayscale {"
          " filter: gray;"
          "-webkit-filter: grayscale(1);"
          "filter: grayscale(1);"
          "}"
          ".grayscale:hover {"
          "-webkit-filter: grayscale(0);"
          "filter: none;"
          "}"
          ".max-w-25 {"
          "max-width: 25rem;"
          "}"
          ".max-w-30 {"
          "max-width: 30rem;"
          "}"
          ".max-w-40 {"
          "max-width: 40rem;"
          "}"
          ".min-h-screen {"
          "min-height: 100vh;"
          "}"
          ".p-10 {"
          "padding: 1.25rem;"
          "}"
          ".pb-10 {"
          "padding-bottom: 1.25rem;"
          "}"
          ".pt-2 {"
          "padding-top: 0.5rem;"
          "}"
          ".w-full {"
          "width: 100%;"
          "}"
          "</style>"
          "<link "
          "href='data:text/"
          "css,%0A%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20ddg-runtime-"
          "checks%20%7B%0A%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%"
          "20%20%20display%3A%20none%3B%0A%20%20%20%20%20%20%20%20%20%20%20%20%"
          "20%20%20%20%7D%0A%20%20%20%20%20%20%20%20%20%20%20%20' "
          "rel='stylesheet' type='text/css'>"
          "</head>"
          "<body class='flex flex-col min-h-screen'>"
          "<nav>"
          "<a href='index.html'>Home</a>"
          "<a href='about.html'>About</a>"
          "<a href='illustrations.html'>Illustrations</a>"
          "<a href='reading.html'>Reading</a>"
          "<a href='watching.html'>Watching</a>"
          "<a href='bookmarks.html'>Bookmarks</a>"
          "<a href='meta.html'>Meta</a>"
          "<a href='log.html'>Log</a>"
          "</nav>"
          "<div class='decorative'></div>"
          "<main>";
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

      if (strcmp("./pages/index.html", src_path) == 0) {
        FILE *log_file = fopen("pages/log.html", "r");

        if (log_file == NULL) {
          perror("fopen");
          exit(EXIT_FAILURE);
        }

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
      strcat(footer_text, "<footer class='p-10'>");
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
      strcat(footer_text, "</html>");

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
  const char *content_dir = "./media/content";
  const char *fonts_dir = "./media/fonts";
  const char *build_dir = "./build";
  const char *build_media_dir = "./build/media";
  const char *build_content_dir = "./build/media/content";
  const char *build_fonts_dir = "./build/media/fonts";
  const char *pages_dir = "./pages";

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
  // Copy the assets
  if (copy_dir(fonts_dir, build_fonts_dir) != 0) {
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
