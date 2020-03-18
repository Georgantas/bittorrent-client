
#include <Utilities.h>

#include <stdexcept>

namespace bittorrent {
std::vector<char> toVector(uint32_t number) {
  return {char(number >> 24), char(number >> 16), char(number >> 8),
          char(number)};
}

uint32_t toUnsignedInt(std::vector<char> vec) {
  if (vec.size() != 4) {
    throw std::runtime_error("Vector not size 4.");
  }

  std::vector<unsigned char> vec2(vec.begin(), vec.end());

  uint32_t ret = (uint32_t(vec2[0]) << 24) | (uint32_t(vec2[1]) << 16) |
                 (uint32_t(vec2[2]) << 8) | uint32_t(vec2[3]);
  return ret;
}
}  // namespace bittorrent
