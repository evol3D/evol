#pragma once

#include <stdint.h>
#include <stddef.h>

uint32_t
checksum_crc32(const void *data, size_t sz);
