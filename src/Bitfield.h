
#pragma once

#include <vector>
#include <string>

namespace bittorrent {
class Bitfield {
 public:
  Bitfield(std::string bits);

  bool hasPiece(size_t index) const;

  void setPiece(size_t index);

 private:
  std::string bits;
};
}  // namespace bittorrent
