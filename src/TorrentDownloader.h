

#pragma once

#include <array>
#include <string>

namespace unit_tests {
class TorrentDownloaderTest_buildUrlToGetPeers_Test;
}

namespace bittorrent {

class TorrentFile;

class TorrentDownloader {
 public:
  // peerId is a 20 character id to identify ourselves: example:
  // -TR2940-k8hj0wgej6ch
  TorrentDownloader(std::array<char, 20> peerId, uint16_t port);

  std::array<char, 20>& getPeerId();

  uint16_t& getPort();

  void downloadTorrentToFile(const TorrentFile& torrentFile,
                             const std::string& path);

 private:
  friend class unit_tests::TorrentDownloaderTest_buildUrlToGetPeers_Test;
  std::string buildUrlToGetPeers(const TorrentFile& torrentFile) const;

  std::array<char, 20> peerId;
  uint16_t port;
};

}  // namespace bittorrent
