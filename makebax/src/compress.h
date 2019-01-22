#pragma once

#include <cstdint>

uint8_t *CompressData(const uint8_t *in, size_t in_sz, size_t& out_sz, int lvl);
