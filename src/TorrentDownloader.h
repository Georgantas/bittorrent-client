

#pragma once

#include <array>
#include <string>
#include <Sha1Hash.h>

namespace unit_tests {
class TorrentDownloaderTest_buildUrlToGetPeers_Test;
class TorrentDownloaderTest_requestPeers_Test;
}  // namespace unit_tests

namespace bittorrent {

class TrackerResponse;
class TorrentFile;

class TorrentDownloader {
 public:
  // peerId is a 20 character id to identify ourselves: example:
  // -TR2940-k8hj0wgej6ch
  TorrentDownloader(Sha1Hash peerId, uint16_t port);

  Sha1Hash& getPeerId();

  uint16_t& getPort();

  void downloadTorrentToFile(const TorrentFile& torrentFile,
                             const std::string& path);

 private:
  friend class unit_tests::TorrentDownloaderTest_buildUrlToGetPeers_Test;
  std::string buildUrlToGetPeers(const TorrentFile& torrentFile) const;

  TrackerResponse requestPeers(const TorrentFile& torrentFile) const;

  Sha1Hash peerId;
  uint16_t port;
};

}  // namespace bittorrent
