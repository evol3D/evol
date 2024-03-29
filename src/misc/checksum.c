/*!
 * \file checksum.c
 */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <evol/misc/checksum.h>

/*!
 * \private
 * \returns If memory is stored as "Least Significant Bit first", then this
 * function returns `true`. Returns `false` otherwise.
 */
bool
lsb_first()
{
  int32_t x    = 1;
  void *  p_x  = &x;
  char *  cp_x = (char *)p_x;
  if (*cp_x == x)
    return true;
  return false;
}

uint32_t
checksum_crc32(
  const void *data, 
  size_t sz)
{
  if (!data) {
    return 0;
  }

  uint32_t       crc      = 0xFFFFFFFF;
  const uint32_t crcgen   = lsb_first() ? 0xEDB88320 : 0x04C11DB7;
  char *         bytedata = (char *)data;
  for (size_t i = 0; i < sz; ++i) {
    char ch = bytedata[i];
    for (size_t j = 0; j < 8; j++) {
      uint32_t b = (ch ^ crc) & 1;
      crc >>= 1;
      if (b)
        crc ^= crcgen;
      ch >>= 1;
    }
  }
  return ~crc;
}
