
#include <Bitfield.h>
#include <ClientConnection.h>
#include <Message.h>
#include <TorrentDownloader.h>
#include <TorrentFile.h>
#include <TrackerResponse.h>
#include <concurrentqueue.h>
#include <curl/curl.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <queue>
#include <scope_guard.hpp>
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

struct work {
  size_t index;
  bittorrent::Sha1Hash hash;
  size_t length;
};

struct piece {
  size_t index;
  std::vector<char> content;
};

std::atomic<size_t> activeWorkers;

moodycamel::ConcurrentQueue<work> workQueue;

moodycamel::ConcurrentQueue<piece> resultsQueue;

std::atomic<bool> close;  // set to true to signal all pieces were processed

// max number of bytes that can be requested
const uint32_t maxBlockSize = 16384;

// max number of requests to a client, we can make at once
const uint32_t maxBacklog = 5;
}  // namespace

namespace bittorrent {
std::vector<char> downloadPiece(
    const std::unique_ptr<ClientConnection>& clientConnection, const work& w) {
  std::vector<char> piece(w.length, '0');
  size_t bytesDownloaded = 0;
  size_t backlog = 0;
  size_t bytesRequested = 0;
  while (bytesDownloaded < w.length) {
    if (!clientConnection->isChocked()) {
      // make requests for pieces
      while (backlog < maxBacklog && bytesRequested < w.length) {
        size_t blockSize = maxBlockSize;
        // trim the block size if the remaining bytes is lower than the block
        // size
        if ((w.length - bytesRequested) < blockSize) {
          blockSize = w.length - bytesRequested;
        }

        clientConnection->sendRequest(w.index, bytesRequested, blockSize);

        backlog++;
        bytesRequested += blockSize;
      }
    }

    // read message
    auto msg = clientConnection->readMessage();
    if (!msg) {
      // keep-alive received
      continue;
    }

    switch (msg->messageID) {
      case MessageType::msgUnchoke:
        /// \todo possible bug: some of the above requests are discared because
        /// the client made too many requests at once
        clientConnection->setChoked(false);
        break;
      case MessageType::msgChoke:
        clientConnection->setChoked(true);
        break;
      case MessageType::msgHave:
        clientConnection->setBit(Message::parseHave(msg.value()));
        break;
      case MessageType::msgPiece:
        bytesDownloaded += Message::parsePiece(msg.value(), w.index, piece);
        backlog--;
        break;
      default:
        continue;
    }
  }

  return piece;
}

void TorrentDownloader::startDownloadWorker(const peer& peer,
                                            const TorrentFile& torrentFile) {
  sg::make_scope_guard([] { activeWorkers--; });

  std::unique_ptr<ClientConnection> clientConnection;

  // fix 🤮
  try {
    clientConnection = std::make_unique<ClientConnection>(
        peer, getPeerId(), torrentFile.getInfoHash());
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
    return;
  }

  clientConnection->sendUnchoke();
  clientConnection->sendInterested();

  while (!close.load()) {
    if (work w; workQueue.try_dequeue(w)) {
      if (!clientConnection->getBitfield().hasPiece(w.index)) {
        // put back
        workQueue.enqueue(w);
        continue;
      }

      std::vector<char> downloadedPiece;
      try {
        downloadedPiece = downloadPiece(clientConnection, w);
      } catch (std::exception& e) {
        // put back
        workQueue.enqueue(w);
        std::cout << e.what() << std::endl;
        return;
      }

      if (calculateSha1Hash(downloadedPiece) != w.hash) {
        workQueue.enqueue(w);
        printf("Piece %lu failed integrity check.\n", w.index);
        // drop client instead? malicious? tcp data should be correct
        continue;
      }

      clientConnection->sendHave(w.index);
      resultsQueue.enqueue(piece{w.index, downloadedPiece});
    }
  }
}

/// \todo Shorten this function
std::vector<char> TorrentDownloader::downloadTorrent(
    const TorrentFile& torrentFile) {
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
    workers.push_back(
        std::thread(std::bind(&TorrentDownloader::startDownloadWorker, this,
                              std::cref(peer), std::cref(torrentFile))));
  }

  // combine pieces
  std::vector<char> ret(torrentFile.getLength(), 0);
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

size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data) {
  data->append((char*)ptr, size * nmemb);
  return size * nmemb;
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

  /*auto writeFunction = [](void* ptr, size_t size, size_t nmemb,
                          std::string* data) -> size_t {
    data->append((char*)ptr, size * nmemb);
    return size * nmemb;
  };*/

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
