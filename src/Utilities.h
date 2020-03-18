
#pragma once

#include <stdint.h>

#include <vector>

namespace bittorrent {
std::vector<char> toVector(uint32_t number);

uint32_t toUnsignedInt(std::vector<char> vec);
}  // namespace bittorrent
