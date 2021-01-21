/*!
 * \file filesystem.h
 */
#pragma once

#include <evol/common/ev_types.h>

/*!
 * \brief A function that iterates recursively over files in a directory and
 * returns files that contain a certain query in their names.
 * \param search_dir The directory to search
 * \param query The query string
 * \param res The vector where the results will be stored
 */
void
find_contains_r(const char *module_dir, const char *query, vec_t *res_modules);