
#include <TorrentDownloader.h>
#include <TorrentFile.h>
#include <TrackerResponse.h>
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

size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data) {
  data->append((char*)ptr, size * nmemb);
  return size * nmemb;
}
}  // namespace

namespace bittorrent {
TorrentDownloader::TorrentDownloader(Sha1Hash peerId, uint16_t port)
    : peerId(peerId), port(port) {}

Sha1Hash& TorrentDownloader::getPeerId() { return peerId; }

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

TrackerResponse TorrentDownloader::requestPeers(
    const TorrentFile& torrentFile) const {
  std::string url = buildUrlToGetPeers(torrentFile);

  auto curl = curl_easy_init();

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.42.0");
  curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

  std::string response_string;
  std::string header_string;
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
  curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

  char* effective_url;
  long response_code;
  double elapsed;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
  curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
  curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &effective_url);

  curl_easy_perform(curl);
  curl_easy_cleanup(curl);
  curl = NULL;

  return TrackerResponse::buildFromBencode(response_string);
}

void TorrentDownloader::downloadTorrentToFile(const TorrentFile& torrentFile,
                                              const std::string& path) {}

}  // namespace bittorrent
