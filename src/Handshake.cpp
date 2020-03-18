
#include <Handshake.h>
#include <sys/socket.h>
#include <unistd.h>

#include <memory>

namespace bittorrent {
std::vector<char> Handshake::serialize() {
  std::vector<char> result;
  result.push_back(pstr.size());
  result.insert(result.end(), pstr.begin(), pstr.end());
  auto zeros = std::vector<char>(8, 0);
  result.insert(result.end(), zeros.begin(), zeros.end());  // 8 reserved bytes
  result.insert(result.end(), infoHash.begin(), infoHash.end());
  result.insert(result.end(), peerId.begin(), peerId.end());
  return result;
}

int readn(int f, void *av, int n);

Handshake Handshake::read(int fd) {
  char pstrLength;

  // if (::read(fd, &pstrLength, sizeof(char)) <= 0) {
  //  throw std::runtime_error("Error reading bytes.");
  //}

  if (readn(fd, &pstrLength, sizeof(char)) != sizeof(char)) {
    throw std::runtime_error("Error reading bytes (handshake)");
  }

  std::unique_ptr<char[]> data(new char[48 + pstrLength]);

  if (readn(fd, data.get(), 48 + pstrLength) != 48 + pstrLength) {
    throw std::runtime_error("Error reading bytes. (handshake #2)");
  }
  // if (::read(fd, data.get(), 48 + pstrLength) <= 0) {
  //  throw std::runtime_error("Error reading bytes.");
  //}

  std::vector<char> pstr(pstrLength, 0);
  std::copy(&data[0], &data[pstrLength], pstr.begin());

  Sha1Hash infoHash;
  std::copy(&data[pstrLength + 8], &data[pstrLength + 28], infoHash.begin());

  Sha1Hash peerId;
  std::copy(&data[pstrLength + 28], &data[pstrLength + 48], peerId.begin());

  return {pstr, infoHash, peerId};
}

}  // namespace bittorrent
