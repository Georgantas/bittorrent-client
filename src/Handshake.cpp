
#include <Handshake.h>

namespace bittorrent {
std::string Handshake::serialize() {
  std::string result;
  result += static_cast<char>(pstr.size());
  result += pstr;
  result += std::string(8, 0);  // 8 reserved bytes
  result += std::string(infoHash.begin(), infoHash.end());
  result += std::string(peerId.begin(), peerId.end());
  return result;
}

Handshake Handshake::deserialize(std::string serial) {
  unsigned char pstrLength = serial[0];
  std::string pstr(&serial[1], &serial[1 + pstrLength]);
  Sha1Hash infoHash;
  std::copy(&serial[pstrLength + 9], &serial[pstrLength + 29],
            infoHash.begin());
  Sha1Hash peerId;
  std::copy(&serial[pstrLength + 29], &serial[pstrLength + 49], peerId.begin());
  return {pstr, infoHash, peerId};
}

}  // namespace bittorrent
