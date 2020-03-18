
#pragma once

#include <string>
#include <vector>

namespace bittorrent {
class Bitfield {
 public:
  Bitfield(std::vector<char> bits);

  bool hasPiece(uint32_t index) const;

  void setPiece(uint32_t index);

 private:
  std::vector<char> bits;
};
}  // namespace bittorrent
