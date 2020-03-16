

#pragma once

#include <openssl/sha.h>

#include <array>

namespace bittorrent {
using Sha1Hash = std::array<unsigned char, 20>;

Sha1Hash calculateSha1Hash(std::string data);

}  // namespace bittorrent
