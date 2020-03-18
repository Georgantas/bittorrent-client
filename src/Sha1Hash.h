

#pragma once

#include <openssl/sha.h>

#include <array>
#include <vector>

namespace bittorrent {
using Sha1Hash = std::array<unsigned char, 20>;

Sha1Hash calculateSha1Hash(std::vector<char> data);

}  // namespace bittorrent
