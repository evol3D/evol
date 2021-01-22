/*!
 * \file evstore.h
 */
#pragma once

#include "evol/common/ev_types.h"

typedef struct evstore evstore_t;

typedef struct evstore_entry_t {
  const char *key;
  ev_type type;
  PTR data;
  FN_PTR free;
} evstore_entry_t;

#define RES_PREFIX EV_STORE_
#define RES_FILE <evol/meta/results/storeresults.h>
#define RES_TYPE EvStoreResult
#include <evol/meta/resdef.h>

/*!
 * \brief Creates a store instance and returns its reference.
 *
 * \returns
 * - Pointer to new instance
 * - If creation failed, `NULL` is returned
 */
evstore_t *
evstore_create();

/*!
 * \brief Frees an `evstore_t` instance
 *
 * \param store pointer to the `evstore_t` instance to be destroyed
 */
void
evstore_destroy(evstore_t *store);

/*!
 * \brief Gets the entry with the specified key
 *
 * \param store pointer to the `evstore_t` instance to be searched
 * \param key string to search for
 * \param res pointer to an `evstore_entry_t` to set
 *
 * \returns
 * - If an entry was found, EV_STORE_ENTRY_FOUND is returned
 * - If an entry was not found, EV_STORE_ENTRY_NOTFOUND is returned
 */
EvStoreResult
evstore_get(evstore_t *store, const char *key, evstore_entry_t *res);

/*!
 * \brief Gets the entry with the specified key and checks for its type
 *
 * \param store pointer to the `evstore_t` instance to be searched
 * \param key string to search for
 * \param type the type expected by the caller
 * \param res pointer to an `evstore_entry_t` to set
 *
 * \returns
 * - If an entry was found, EV_STORE_ENTRY_FOUND is returned
 * - If an entry was found but its not of the expected type, EV_STORE_ENTRY_WRONGTYPE is returned
 * - If an entry was not found, EV_STORE_ENTRY_NOTFOUND is returned
 */
EvStoreResult
evstore_get_checktype(evstore_t *store, const char *key, ev_type type, evstore_entry_t *res);

/*!
 * \brief Sets/Adds the passed entry to the store
 *
 * \param store pointer to the `evstore_t` instance to add the entry to
 * \param entry entry to copy into the store
 *
 * \returns
 * - If the entry was added successfully, EV_STORE_SUCCESS is returned
 * - If an OOM is encountered, EV_STORE_ERROR_OOM is returned
 */
EvStoreResult
evstore_set(evstore_t *store, evstore_entry_t *entry);

/*!
 * \brief Frees all the entries that have a "free" field
 *
 * \param store pointer to the `evstore_t` instance to clear
 *
 * \returns
 * - In all cases, EV_STORE_SUCCESS is returned
 */
EvStoreResult
evstore_clear(evstore_t *store);
