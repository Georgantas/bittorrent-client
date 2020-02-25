
#include <TrackerResponse.h>

#include "gtest/gtest.h"

namespace unit_tests {
TEST(TrackerResponseTest, buildFromBencode) {
  std::string bencode(
      "d8:intervali900e5:peers12:" +
      std::string({
          char(192), 0, 2, 123, 0x1A, char(0xE1),  // 0x1AE1 = 6881
          127, 0, 0, 1, 0x1A, char(0xE9),          // 0x1AE9 = 6889
      }) +
      "e");
  bittorrent::TrackerResponse trackerResponse =
      bittorrent::TrackerResponse::buildFromBencode(bencode);

  EXPECT_EQ(trackerResponse.getRefreshInterval(), 900);
  EXPECT_EQ(trackerResponse.getPeers().size(), 2);
  EXPECT_EQ(trackerResponse.getPeers()[0].ip[2], 2);
  EXPECT_EQ(trackerResponse.getPeers()[0].port, 6881);
}
}  // namespace unit_tests
