
#include <TorrentFile.h>

#include <fstream>

#include "gtest/gtest.h"

TEST(TorrentFile_Test, BuildFromStream) {
  std::ifstream file_stream("archlinux.iso.torrent");

  TorrentFile torrentFile = TorrentFile::BuildFromStream(file_stream);
  EXPECT_EQ(torrentFile.get_announce(), "ARCH LOLZ");
}
