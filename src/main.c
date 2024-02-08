#include <stdlib.h>

#include "main.h"

const char *CONTENT_DIR = "./media/content";
const char *PAGES_DIR = "./pages";
const char *BUILD_DIR = "./build";
const char *BUILD_MEDIA_DIR = "./build/media";
const char *BUILD_CONTENT_DIR = "./build/media/content";

int main() {
  // Delete the build directory
  if (delete_directory(BUILD_DIR) != 0) {
    fprintf(stderr, "Error deleting build directory\n");
    return 1;
  }

  // Create the build directory
  if (create_directory(BUILD_DIR) != 0) {
    fprintf(stderr, "Error creating build directory\n");
    return 1;
  }

  // Create the media directory
  if (create_directory(BUILD_MEDIA_DIR) != 0) {
    fprintf(stderr, "Error creating media directory\n");
    return 1;
  }

  // Copy the assets
  if (copy_directory(CONTENT_DIR, BUILD_CONTENT_DIR) != 0) {
    fprintf(stderr, "Error copying assets\n");
    return 1;
  }

  // Copy the pages
  if (copy_pages(PAGES_DIR, BUILD_DIR) != 0) {
    fprintf(stderr, "Error copying pages\n");
    return 1;
  }

  printf("\033[1;32mThe build was successful\033[0m\n");
  exit(EXIT_SUCCESS);
}
