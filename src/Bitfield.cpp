
#include <Bitfield.h>

namespace bittorrent {
Bitfield::Bitfield(std::vector<unsigned char> bits) : bits(bits) {}

bool Bitfield::hasPiece(size_t index) const {
  size_t byteIndex = index / 8;
  size_t offset = index % 8;
  return (bits[byteIndex] >> (7 - offset) & 1) != 0;
}

void Bitfield::setPiece(size_t index) {
  size_t byteIndex = index / 8;
  size_t offset = index % 8;
  bits[byteIndex] |= 1 << (7 - offset);
}
}  // namespace bittorrent
