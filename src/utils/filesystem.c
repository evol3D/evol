#include <evol/utils/filesystem.h>

#include <dirent.h>
#include <string.h>

#include <stdio.h>

void
find_contains_r(const char *module_dir, const char *query, vec_t *res_modules)
{
  char  path[1000];
  char *path_p = path;
  struct dirent *dp;
  DIR *dir = opendir(module_dir);

  if (!dir)
    return;

  while ((dp = readdir(dir)) != NULL) {
    if (strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..")) {
      strcpy(path, module_dir);
      strcat(path, "/");
      strcat(path, dp->d_name);

      if (strstr(path, query)) {
        vec_push(res_modules, &path_p);
      }

      find_contains_r(path, query, res_modules);
    }
  }

  closedir(dir);
}
