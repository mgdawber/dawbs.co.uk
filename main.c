#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

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
      char buffer[BUFSIZ];
      size_t n;
      const char *head_path = "./templates/_head.html";
      const char *footer_path = "./templates/_footer.html";

      // Open the source file
      if ((head_file = fopen(head_path, "rb")) == NULL) {
        perror("fopen");
        return 1;
      }

      // Open the source file
      if ((src_file = fopen(src_path, "rb")) == NULL) {
        perror("fopen");
        return 1;
      }

      // Open the source file
      if ((footer_file = fopen(footer_path, "rb")) == NULL) {
        perror("fopen");
        return 1;
      }

      // Open the destination file
      if ((dest_file = fopen(dest_path, "wb")) == NULL) {
        perror("fopen");
        return 1;
      }

      // Copy the head file contents
      while ((n = fread(buffer, 1, sizeof(buffer), head_file)) > 0) {
        if (fwrite(buffer, 1, n, dest_file) != n) {
          perror("fwrite");
          return 1;
        }
      }

      // Copy the file contents
      while ((n = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        if (fwrite(buffer, 1, n, dest_file) != n) {
          perror("fwrite");
          return 1;
        }
      }

      // Copy the footer file contents
      while ((n = fread(buffer, 1, sizeof(buffer), footer_file)) > 0) {
        if (fwrite(buffer, 1, n, dest_file) != n) {
          perror("fwrite");
          return 1;
        }
      }

      // Close the files
      if (fclose(head_file) == EOF) {
        perror("fclose");
        return 1;
      }
      if (fclose(src_file) == EOF) {
        perror("fclose");
        return 1;
      }
      if (fclose(footer_file) == EOF) {
        perror("fclose");
        return 1;
      }
      if (fclose(dest_file) == EOF) {
        perror("fclose");
        return 1;
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

int copy_file(const char *src_path, const char *dst_path) {
  FILE *src_file, *dst_file;
  char buffer;
  size_t bytes_read;

  src_file = fopen(src_path, "rb");
  if (src_file == NULL) {
    perror("Error opening source file");
    return 1;
  }

  dst_file = fopen(dst_path, "wb");
  if (dst_file == NULL) {
    perror("Error opening destination file");
    fclose(src_file);
    return 1;
  }

  while ((bytes_read = fread(&buffer, 1, sizeof(buffer), src_file)) > 0) {
    if (fwrite(&buffer, 1, bytes_read, dst_file) != bytes_read) {
      perror("Error writing to destination file");
      fclose(src_file);
      fclose(dst_file);
      return 1;
    }
  }

  fclose(src_file);
  fclose(dst_file);
  return 0;
}

int copy_dir(const char *src_dir, const char *dst_dir) {
  DIR *dir;
  struct dirent *entry;
  struct stat statbuf;
  char src_path[PATH_MAX];
  char dst_path[PATH_MAX];
  char build_path[PATH_MAX];

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

      if ((src_file = fopen(src_path, "rb")) == NULL) {
        perror("fopen");
        return 1;
      }

      if ((dst_file = fopen(dst_path, "wb")) == NULL) {
        perror("fopen");
        return 1;
      }

      while ((n = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        if (fwrite(buffer, 1, n, dst_file) != n) {
          perror("fwrite");
          return 1;
        }
      }

      if (fclose(src_file) == EOF) {
        perror("fclose");
        return 1;
      }

      if (fclose(dst_file) == EOF) {
        perror("fclose");
        return 1;
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
  return 0;
}
