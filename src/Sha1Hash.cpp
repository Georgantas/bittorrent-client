
#include <Sha1Hash.h>

namespace bittorrent {
Sha1Hash calculateSha1Hash(std::vector<char> data) {
  Sha1Hash hash;

  SHA1((const uint8_t*)&data[0], data.size(), hash.begin());

  return hash;
}
}  // namespace bittorrent
