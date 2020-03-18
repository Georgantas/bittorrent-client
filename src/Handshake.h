
#pragma once

#include <Sha1Hash.h>

#include <vector>

namespace bittorrent {
struct Handshake {
  const std::vector<char> pstr;
  const Sha1Hash infoHash;
  const Sha1Hash peerId;

  std::vector<char> serialize();
  static Handshake read(int fd);
};
}  // namespace bittorrent
