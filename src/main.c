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
          "*, ::after, ::before {"
          "box-sizing: border-box;"
          "border-width: 0;"
          "border: 0 solid #e5e7eb;"
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
          "}"
          "code {"
          "font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "
          "'Liberation Mono', 'Courier New', monospace;"
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
          "-webkit-text-size-adjust: 100%;"
          "-moz-tab-size: 4;"
          "-o-tab-size: 4;"
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
          "padding-right: 1.25rem;"
          "padding-left: 1.25rem;"
          "margin-top: 1.25rem;"
          "margin-bottom: 1.25rem;"
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
          "border: 2px solid black;"
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
          "background: url('data:image/png;base64,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');"
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
          ".italic {"
          "font-style: italic;"
          "}"
          ".max-w-30-rem {"
          "max-width: 30rem;"
          "}"
          ".max-w-40rem {"
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
  const char *src_dir = "./media/content";
  const char *build_dir = "./build";
  const char *media_dir = "./build/media";
  const char *dst_dir = "./build/media/content";
  const char *pages_dir = "./pages";

  // Delete the build directory
  delete_dir(build_dir);

  // Create the build directory
  if (mkdir(build_dir, 0777) == -1) {
    perror("mkdir");
    return 1;
  }

  // Create the media directory
  if (mkdir(media_dir, 0777) == -1) {
    perror("mkdir");
    return 1;
  }

  // Copy the assets
  if (copy_dir(src_dir, dst_dir) != 0) {
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
