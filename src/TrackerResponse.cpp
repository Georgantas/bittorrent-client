
#include <TrackerResponse.h>

#include <bencode.hpp>

namespace bittorrent {
TrackerResponse::TrackerResponse(std::vector<peer> peers,
                                 uint64_t refreshInterval)
    : peers(peers), refreshInterval(refreshInterval) {}

TrackerResponse TrackerResponse::buildFromBencode(std::string bencodeData) {
  auto data = bencode::decode(bencodeData);
  auto response = boost::get<bencode::dict>(data);

  uint64_t interval = boost::get<bencode::integer>(response["interval"]);

  std::string peersBitstream = boost::get<bencode::string>(response["peers"]);

  std::vector<peer> peers;

  // parse peersBitstream
  for (size_t i = 0; i < peersBitstream.size() / 6; ++i) {
    std::array<unsigned char, 4> ip;
    std::copy(&peersBitstream[i * 6], &peersBitstream[i * 6 + 4], ip.begin());
    auto upper = uint16_t((unsigned char)(peersBitstream[i * 6 + 4])) << 8;
    auto lower = uint16_t((unsigned char)(peersBitstream[i * 6 + 5]));
    uint16_t port = (upper | lower);
    peers.push_back(peer({ip, port}));
  }

  return TrackerResponse(peers, interval);
}

const std::vector<peer>& TrackerResponse::getPeers() const { return peers; }

const uint64_t& TrackerResponse::getRefreshInterval() const {
  return refreshInterval;
}

}  // namespace bittorrent
