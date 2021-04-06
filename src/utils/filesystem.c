/*!
 * \file filesystem.c
 */
#include <evol/utils/filesystem.h>

#include <string.h>

#include <stdio.h>

#include <cute_files.h>

struct SearchStruct {
  const char *extension;
  vec_t      *vector;
};

// If the file's extension matches the extension we're looking for, then it is added
// to the vector
void 
extcheck_add2vec(
  cf_file_t* file, 
  PTR udata)
{
  if (file->is_dir) {
    return;
  }

  struct SearchStruct *res = udata;
  if (cf_match_ext(file, res->extension)) {
    char *path_p = file->path;
    vec_push(res->vector, &path_p);
  }
}

void
find_contains_r(
  CONST_STR search_dir, 
  CONST_STR query, 
  vec_t *res)
{
  cf_traverse(search_dir, extcheck_add2vec, &(struct SearchStruct){
    .extension = query,
    .vector = res,
  });
}