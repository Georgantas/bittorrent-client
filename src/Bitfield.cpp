
#include <Bitfield.h>

namespace bittorrent {
Bitfield::Bitfield(std::vector<char> bits) : bits(bits) {}

bool Bitfield::hasPiece(uint32_t index) const {
  uint32_t byteIndex = index / 8;
  uint32_t offset = index % 8;
  return ((bits[byteIndex] >> (7 - offset) & 1)) != 0;
}

void Bitfield::setPiece(uint32_t index) {
  uint32_t byteIndex = index / 8;
  uint32_t offset = index % 8;

  // silently discard invalid bounded index
  if (byteIndex >= bits.size()) {
    return;
  }

  bits[byteIndex] |= 1 << (7 - offset);
}
}  // namespace bittorrent
