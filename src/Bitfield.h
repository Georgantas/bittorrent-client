
#pragma once

#include <vector>

namespace bittorrent {
class Bitfield {
 public:
  Bitfield(std::vector<unsigned char> bits);

  bool hasPiece(size_t index) const;

  void setPiece(size_t index);

 private:
  std::vector<unsigned char> bits;
};
}  // namespace bittorrent
