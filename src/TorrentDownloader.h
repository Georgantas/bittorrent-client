

#pragma once

#include <Sha1Hash.h>

#include <array>
#include <string>

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

  std::string downloadTorrent(const TorrentFile& torrentFile);

 private:
  friend class unit_tests::TorrentDownloaderTest_buildUrlToGetPeers_Test;
  std::string buildUrlToGetPeers(const TorrentFile& torrentFile) const;

  TrackerResponse requestPeers(const TorrentFile& torrentFile) const;

  Sha1Hash peerId;
  uint16_t port;
};

}  // namespace bittorrent
