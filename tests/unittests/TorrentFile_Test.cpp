
#include <TorrentFile.h>

#include <fstream>

#include "gtest/gtest.h"

TEST(TorrentFile_Test, BuildFromStream) {
  std::ifstream file_stream("archlinux.iso.torrent");

  TorrentFile torrentFile = TorrentFile::BuildFromStream(file_stream);

  EXPECT_EQ(torrentFile.getAnnounce(),
            "http://tracker.archlinux.org:6969/announce");
  EXPECT_EQ(torrentFile.getName(), "archlinux-2019.12.01-x86_64.iso");
  // EXPECT_EQ(torrentFile.getInfoHash(), );
  // EXPECT_EQ(torrentFile.getLength(), );
  // EXPECT_EQ(torrentFile.getPieceLength(), );
  // EXPECT_EQ(torrentFile.getPiecesHash(), );
}
