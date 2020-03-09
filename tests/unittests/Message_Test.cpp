
#include "gtest/gtest.h"

#include <Message.h>

namespace unit_tests {
TEST(MessageTest, serializeAndDeserialize) {
  std::string toDeserialize{0x00, 0x00, 0x00, 0x04, 0x02,
                            0x01, 0x02, 0x03, 0x04};
  auto res = bittorrent::Message::deserialize(toDeserialize);

  EXPECT_EQ(res.messageID, bittorrent::MessageType::msgInterested);

  std::string expectedPayload(std::next(toDeserialize.begin(), 5),
                              toDeserialize.end());

  EXPECT_EQ(res.payload, expectedPayload);

  EXPECT_EQ(res.serialize(), toDeserialize);
}
}  // namespace unit_tests
