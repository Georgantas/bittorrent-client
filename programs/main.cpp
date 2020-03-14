
#include <TorrentDownloader.h>
#include <TorrentFile.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
  std::vector<std::string> arguments(argv + 1, argv + argc);

  std::string inputPath(arguments[0]);
  std::string outputPath(arguments[1]);

  bittorrent::Sha1Hash peerId(
      {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20});
  uint16_t port(6881);

  bittorrent::TorrentDownloader torrentDownloader(peerId, port);

  std::ifstream inputStream(inputPath);

  bittorrent::TorrentFile torrentFile =
      bittorrent::TorrentFile::buildFromStream(inputStream);

  std::string res = torrentDownloader.downloadTorrent(torrentFile);

  std::cout << res << std::endl;
}
