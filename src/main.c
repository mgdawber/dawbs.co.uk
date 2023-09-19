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
          "}"
          "html {"
          "line-height: 1.5;"
          "-webkit-text-size-adjust: 100%;"
          "-moz-tab-size: 4;"
          "-o-tab-size: 4;"
          "tab-size: 4;"
          "font-family: ui-sans-serif, system-ui, -apple-system, "
          "BlinkMacSystemFont, 'Segoe UI', Roboto, 'Helvetica Neue', Arial, "
          "'Noto Sans', sans-serif, 'Apple Color Emoji', 'Segoe UI Emoji', "
          "'Segoe UI Symbol', 'Noto Color Emoji';"
          "font-feature-settings: normal;"
          "font-variation-settings: normal;"
          "}"
          "body {"
          "margin: 0;"
          "line-height: inherit;"
          "}"
          "main {"
          "width: 100%;"
          "padding-right: 1.25rem;"
          "padding-left: 1.25rem;"
          "margin-top: 1.25rem;"
          "margin-bottom: 1.25rem;"
          "max-width: 700px;"
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
          "a {"
          "--tw-text-opacity: 1;"
          "color: rgb(0 0 0 / var(--tw-text-opacity));"
          "text-decoration-line: underline;"
          "}"
          "a:hover {"
          "--tw-text-opacity: 1;"
          "background-color: rgb(0 0 0 / var(--tw-text-opacity));"
          "color: rgb(255 255 255 / var(--tw-text-opacity));"
          "}"
          "::selection {"
          "--tw-text-opacity: 1;"
          "background-color: rgb(0 0 0 / var(--tw-text-opacity));"
          "color: rgb(255 255 255 / var(--tw-text-opacity));"
          "}"
          "img {"
          "display: block;"
          "height: auto;"
          "}"
          "hr {"
          "margin-left: 1.25rem;"
          "margin-right: 1.25rem;"
          "margin-top: 0;"
          "margin-bottom: 0;"
          "height: 0.25rem;"
          "border-top-width: 1px;"
          "--tw-border-opacity: 1;"
          "border-color: rgb(0 0 0 / var(--tw-border-opacity));"
          "}"
          "figure {"
          "margin: 0;"
          "}"
          "pre {"
          "font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "
          "'Liberation Mono', 'Courier New', monospace;"
          "font-size: 1em;"
          "margin-top: 1.5rem;"
          "margin-bottom: 1.5rem;"
          "white-space: pre-wrap;"
          "overflow-wrap: break-word;"
          "--tw-bg-opacity: 1;"
          "background-color: rgb(0 0 0 / var(--tw-bg-opacity));"
          "padding: 0.75rem;"
          "}"
          "small {"
          "font-size: 80%;"
          "}"
          "code {"
          "font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "
          "'Liberation Mono', 'Courier New', monospace;"
          "font-size: 1em;"
          "--tw-border-opacity: 1;"
          "color: rgb(255 255 255 / var(--tw-border-opacity));"
          "}"
          ".max-w-30-rem {"
          "max-width: 30rem;"
          "}"
          ".pb-10 {"
          "padding-bottom: 1.25rem;"
          "}"
          ".pt-2 {"
          "padding-top: 0.5rem;"
          "}"
          ".border-2 {"
          "border: 2px solid black;"
          "max-width: 600px;"
          "padding: 1.5rem;"
          "}"
          "ul {"
          "padding-left: 1.25rem;"
          "margin-top: 1.5rem;"
          "margin-bottom: 1.5rem;"
          "list-style-type: disc;"
          "}"
          "p {"
          "margin-top: 1.5rem;"
          "margin-bottom: 1.5rem;"
          "}"
          "footer {"
          "margin-top: auto;"
          "width: 100%;"
          "}"
          "nav {"
          "padding-left: 1.25rem;"
          "padding-right: 1.25rem;"
          "margin-top: 1.25rem;"
          "margin-bottom: 1.25rem;"
          "--tw-text-opacity: 1;"
          "background-color: rgb(0 0 0 / var(--tw-text-opacity));"
          "}"
          "nav > a {"
          "margin-right: 1rem;"
          "--tw-text-opacity: 1;"
          "color: rgb(255 255 255 / var(--tw-text-opacity));"
          "}"
          "nav > a:hover {"
          "--tw-text-opacity: 1;"
          "color: rgb(0 0 0 / var(--tw-text-opacity));"
          "background-color: rgb(255 255 255 / var(--tw-text-opacity));"
          "}"
          ".italic {"
          "font-style: italic;"
          "}"
          ".bold {"
          "font-weight: bold;"
          "}"
          ".w-full {"
          "width: 100%;"
          "}"
          ".max-w-40rem {"
          "max-width: 40rem;"
          "}"
          ".columns-2 {"
          "columns: 2;"
          "}"
          ".flex {"
          "display: flex;"
          "}"
          ".flex-col {"
          "flex-direction: column;"
          "}"
          ".flex-row {"
          "flex-direction: row;"
          "}"
          ".min-h-screen {"
          "min-height: 100vh;"
          "}"
          ".p-10 {"
          "padding: 1.25rem;"
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
          "<a href='log.html'>Log</a>"
          "</nav>"
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
      strcat(footer_text, "</div>");
      strcat(footer_text, "<footer>");
      strcat(footer_text, "<div class='p-10'>");
      strcat(footer_text, "Last edited on: ");
      strcat(footer_text, timeStr);
      strcat(footer_text, " ");
      strcat(footer_text, "[<a href='https://git.sr.ht/~dawbs/dawbs.co.uk'>Edit</a>]");
      strcat(footer_text, "</div>");
      strcat(footer_text, "<hr>");
      strcat(footer_text, "<div class='p-10'>");
      strcat(footer_text, "<div>");
      strcat(footer_text, "<span class='bold'>Dawbs © 2023</span>");
      strcat(footer_text, "<span> - <a href='https://creativecommons.org/licenses/by-nc-sa/4.0/'>BY-NC-SA 4.0</a></span>");
      strcat(footer_text, "<div>");
      strcat(footer_text, "</div>" );
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
