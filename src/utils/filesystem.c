/*!
 * \file filesystem.c
 */
#include <evol/utils/filesystem.h>

#include <dirent.h>
#include <string.h>

#include <stdio.h>

/*!
 * \brief A function that iterates recursively over files in a directory and
 * returns files that contain a certain query in their names.
 * \param search_dir The directory to search
 * \param query The query string
 * \param res The vector where the results will be stored
 */
void
find_contains_r(const char *search_dir, const char *query, vec_t *res)
{
  char           path[1000];
  char *         path_p = path;
  struct dirent *dp;
  DIR *          dir = opendir(search_dir);

  if (!dir)
    return;

  while ((dp = readdir(dir)) != NULL) {
    if (strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..")) {
      strcpy(path, search_dir);
      strcat(path, "/");
      strcat(path, dp->d_name);

      if (strstr(path, query)) {
        vec_push(res, &path_p);
      }

      find_contains_r(path, query, res);
    }
  }

  closedir(dir);
}
