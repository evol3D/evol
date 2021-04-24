/*!
 * \file checksum.h
 */
#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/*!
 * \brief A function that takes a pointer to a block of data and its size, and
 * returns its cyclic redundancy check using the [CRC32] algorithm.
 * \param data Pointer to data to checksum
 * \param sz size of `data` (in bytes)
 * \returns Cyclic redundancy check of the passed data. If passed data is NULL,
 * then 0 is returned.
 *
 * [CRC32]: https://en.wikipedia.org/wiki/Cyclic_redundancy_check#CRC-32_algorithm "CRC32"
 */
uint32_t
checksum_crc32(
    const void *data, 
    size_t sz);

#if defined(__cplusplus)
}
#endif
