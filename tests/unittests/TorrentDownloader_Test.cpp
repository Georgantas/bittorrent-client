
#include <Sha1Hash.h>
#include <TorrentDownloader.h>
#include <TorrentFile.h>
#include <TrackerResponse.h>
#include <fstream>

#include "gtest/gtest.h"

namespace unit_tests {
TEST(TorrentDownloaderTest, buildUrlToGetPeers) {
  bittorrent::Sha1Hash peerId(
      {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20});
  uint16_t port(6881);
  bittorrent::TorrentDownloader torrentDownloader(peerId, port);

  std::string announce("http://bttracker.debian.org:6969/announce");
  bittorrent::Sha1Hash infoHash({216, 247, 57,  206, 195, 40,  149,
                                 108, 204, 91,  191, 31,  134, 217,
                                 253, 207, 219, 168, 206, 182});
  std::vector<bittorrent::Sha1Hash> piecesHash(
      {{49, 50, 51, 52,  53,  54,  55,  56,  57,  48,
        97, 98, 99, 100, 101, 102, 103, 104, 105, 106},
       {97, 98, 99, 100, 101, 102, 103, 104, 105, 106,
        49, 50, 51, 52,  53,  54,  55,  56,  57,  48}});
  uint64_t pieceLength(262144);
  uint64_t length(351272960);
  std::string name("debian-10.2.0-amd64-netinst.iso");
  bittorrent::TorrentFile torrentFile(announce, infoHash, piecesHash,
                                      pieceLength, length, name);

  std::string res(torrentDownloader.buildUrlToGetPeers(torrentFile));

  EXPECT_EQ(
      res,
      "http://bttracker.debian.org:6969/"
      "announce?compact=1&downloaded=0&info_hash=%D8%F79%CE%C3%28%95l%CC%5B%BF%"
      "1F%86%D9%FD%CF%DB%A8%CE%B6&left=351272960&peer_id=%01%02%03%04%05%06%07%"
      "08%09%0A%0B%0C%0D%0E%0F%10%11%12%13%14&port=6881&uploaded=0");
}

}  // namespace unit_tests
