
#include <TorrentFile.h>

#include <fstream>

#include "gtest/gtest.h"

/// \todo unit_tests -> bittorrent::unit_tests
namespace unit_tests {
TEST(TorrentFileTest, buildFromStream) {
  std::ifstream file_stream("archlinux.iso.torrent");

  bittorrent::TorrentFile torrentFile =
      bittorrent::TorrentFile::buildFromStream(file_stream);

  EXPECT_EQ(torrentFile.getAnnounce(),
            "http://tracker.archlinux.org:6969/announce");
  EXPECT_EQ(torrentFile.getName(), "archlinux-2019.12.01-x86_64.iso");
  EXPECT_EQ(torrentFile.getPiecesHash().at(999).at(19), 119);
  EXPECT_EQ(
      torrentFile.getInfoHash(),
      bittorrent::Sha1Hash({222, 232, 106, 127, 166, 242, 134, 169, 215, 76,
                            54,  32,  20,  97,  106, 15,  245, 228, 132, 61}));
  EXPECT_EQ(torrentFile.getLength(), 670040064);
  EXPECT_EQ(torrentFile.getPieceLength(), 524288);
}
}  // namespace unit_tests
