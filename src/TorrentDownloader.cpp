
#include <TorrentDownloader.h>
#include <TorrentFile.h>
#include <TrackerResponse.h>
#include <concurrentqueue.h>
#include <curl/curl.h>

#include <algorithm>
#include <atomic>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

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

struct work {
  size_t index;
  bittorrent::Sha1Hash hash;
  size_t length;
};

struct piece {
  size_t index;
  std::vector<char> content;
};

moodycamel::ConcurrentQueue<work> workQueue;

moodycamel::ConcurrentQueue<piece> resultsQueue;

std::atomic<size_t> activeWorkers;
std::atomic<bool> close;

void startDownloadWorker(bittorrent::peer& peer) {
  while (!close.load()) {
    if (work w; workQueue.try_dequeue(w)) {
      if (peer.port % 2 == 0) {
        // process
        resultsQueue.enqueue(
            piece{w.index, std::vector<char>(w.length, w.index)});
      } else {
        // put back
        workQueue.enqueue(w);
        std::this_thread::yield();
      }
    }
  }

  activeWorkers--;
}

}  // namespace

namespace bittorrent {

/// \todo Shorten this function
std::string TorrentDownloader::downloadTorrent(const TorrentFile& torrentFile) {
  std::cout << "Downloading " + torrentFile.getName() << std::endl;

  bittorrent::TrackerResponse trackerResponse = requestPeers(torrentFile);

  auto peers = trackerResponse.getPeers();
  // auto refreshInterval = trackerResponse.getRefreshInterval();

  // fill the work queue
  auto pieceHashes = torrentFile.getPiecesHash();
  auto numberOfPieces = pieceHashes.size();

  for (size_t i = 0; i < numberOfPieces; ++i) {
    // calculate piece length
    size_t begin = i * torrentFile.getPieceLength();
    size_t end = begin + torrentFile.getPieceLength();
    if (end > torrentFile.getLength()) {
      end = torrentFile.getLength();
    }

    size_t pieceLength = end - begin;

    work w{i, pieceHashes[i], pieceLength};
    workQueue.enqueue(w);
  }

  // start workers
  std::vector<std::thread> workers;
  activeWorkers = peers.size();

  for (auto& peer : peers) {
    workers.push_back(std::thread(startDownloadWorker, std::ref(peer)));
  }

  // combine pieces
  std::string ret(torrentFile.getLength(), 0);
  size_t piecesProcessed = 0;
  while (piecesProcessed < numberOfPieces) {
    if (piece result; resultsQueue.try_dequeue(result)) {
      // calculate bounds of piece
      size_t begin = result.index * torrentFile.getPieceLength();
      size_t end = begin + torrentFile.getPieceLength();
      if (end > torrentFile.getLength()) {
        end = torrentFile.getLength();
      }
      std::copy(result.content.begin(), result.content.end(), &ret[begin]);
      piecesProcessed++;

      float percent = static_cast<float>(piecesProcessed) /
                      static_cast<float>(numberOfPieces) * 100.f;

      printf("(%0.2f%%) Downloaded piece %lu from %lu peers\n", percent,
             result.index, activeWorkers.load());
    }
  }
  close = true;
  std::for_each(workers.begin(), workers.end(),
                [](auto& worker) { worker.join(); });

  return ret;
}

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

}  // namespace bittorrent
