
#include <Handshake.h>
#include <unistd.h>
#include <memory>

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

Handshake Handshake::read(int fd) {
  char pstrLength;

  if (::read(fd, &pstrLength, sizeof(char)) <= 0) {
    throw std::runtime_error("Error reading bytes.");
  }

  std::unique_ptr<char[]> data(new char[48 + pstrLength]);
  if (::read(fd, data.get(), 48 + pstrLength) <= 0) {
    throw std::runtime_error("Error reading bytes.");
  }

  std::string pstr(pstrLength, 0);
  std::copy(&data[0], &data[pstrLength], pstr.begin());

  Sha1Hash infoHash;
  std::copy(&data[pstrLength + 8], &data[pstrLength + 28], infoHash.begin());

  Sha1Hash peerId;
  std::copy(&data[pstrLength + 28], &data[pstrLength + 48], peerId.begin());

  return {pstr, infoHash, peerId};
}

}  // namespace bittorrent
