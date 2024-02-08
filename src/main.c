#include <stdio.h>
#include <stdlib.h>

#include "main.h"

#define CONTENT_DIR "./media/content"
#define PAGES_DIR "./pages"
#define BUILD_DIR "./build"
#define BUILD_MEDIA_DIR "./build/media"
#define BUILD_CONTENT_DIR "./build/media/content"

int main() {
  // Delete the build directory
  if (delete_directory(BUILD_DIR) != 0) {
    fprintf(stderr, "Error deleting build directory '%s'\n", BUILD_DIR);
    return 1;
  }

  // Create the build directory
  if (create_directory(BUILD_DIR) != 0) {
    fprintf(stderr, "Error creating build directory '%s'\n", BUILD_DIR);
    return 1;
  }

  // Create the media directory
  if (create_directory(BUILD_MEDIA_DIR) != 0) {
    fprintf(stderr, "Error creating media directory '%s'\n", BUILD_MEDIA_DIR);
    return 1;
  }

  // Copy the assets
  if (copy_directory(CONTENT_DIR, BUILD_CONTENT_DIR) != 0) {
    fprintf(stderr, "Error copying assets from '%s' to '%s'\n", CONTENT_DIR,
            BUILD_CONTENT_DIR);
    return 1;
  }

  // Copy the pages
  if (copy_pages(PAGES_DIR, BUILD_DIR) != 0) {
    fprintf(stderr, "Error copying pages from '%s' to '%s'\n", PAGES_DIR,
            BUILD_DIR);
    return 1;
  }

  printf("\033[1;32mThe build was successful\033[0m\n");
  exit(EXIT_SUCCESS);
}
