
#include <Sha1Hash.h>

namespace bittorrent {
Sha1Hash calculateSha1Hash(std::string data) {
  Sha1Hash hash;

  SHA1((const uint8_t*)data.c_str(), data.size(), hash.begin());

  return hash;
}
}  // namespace bittorrent
