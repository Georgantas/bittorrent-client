
#pragma once

#include <array>
#include <string>
#include <vector>

namespace bittorrent {
struct peer {
  std::array<unsigned char, 4> ip;
  uint16_t port;
};

class TrackerResponse {
 public:
  static TrackerResponse buildFromBencode(std::string bencodeData);

  const std::vector<peer>& getPeers() const;

  const uint64_t& getRefreshInterval() const;

 private:
  TrackerResponse(std::vector<peer> peers, uint64_t refreshInterval);

  std::vector<peer> peers;
  uint64_t refreshInterval;
};
}  // namespace bittorrent
