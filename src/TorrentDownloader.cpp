
#include <TorrentDownloader.h>
#include <TorrentFile.h>
#include <curl/curl.h>

#include <memory>

namespace {
std::string urlEscapeString(std::string str) {
  CURL* curl = curl_easy_init();

  if (!curl) {
    throw "Could not instantiate curl.";
  }

  char* output = curl_easy_escape(curl, str.c_str(), str.size());

  if (!output) {
    throw "Could not escape url.";
  }

  std::string res(output);

  curl_free(output);

  return res;
}
}  // namespace

namespace bittorrent {
TorrentDownloader::TorrentDownloader(std::array<char, 20> peerId, uint16_t port)
    : peerId(peerId), port(port) {}

std::array<char, 20>& TorrentDownloader::getPeerId() { return peerId; }

uint16_t& TorrentDownloader::getPort() { return port; }

std::string TorrentDownloader::buildUrlToGetPeers(
    const TorrentFile& torrentFile) const {
  std::string url = torrentFile.getAnnounce();
  url += "?";

  url += ("compact=" + std::to_string(1) + "&");

  url += ("downloaded=" + std::to_string(0) + "&");

  auto infoHash = torrentFile.getInfoHash();
  url += ("info_hash=" +
          urlEscapeString(std::string(infoHash.begin(), infoHash.end())) + "&");

  url += ("left=" + std::to_string(torrentFile.getLength()) + "&");

  url +=
      ("peer_id=" + urlEscapeString(std::string(peerId.begin(), peerId.end())) +
       "&");

  url += ("port=" + std::to_string(port) + "&");

  url += ("uploaded=" + std::to_string(0));

  return url;
}

void TorrentDownloader::downloadTorrentToFile(const TorrentFile& torrentFile,
                                              const std::string& path) {}

}  // namespace bittorrent
