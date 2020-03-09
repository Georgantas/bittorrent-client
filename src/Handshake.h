
#pragma once

#include <string>
#include <Sha1Hash.h>

namespace bittorrent {
struct Handshake {
  const std::string pstr;
  const Sha1Hash infoHash;
  const Sha1Hash peerId;

  std::string serialize();
  static Handshake deserialize(std::string serial);
};
}  // namespace bittorrent
