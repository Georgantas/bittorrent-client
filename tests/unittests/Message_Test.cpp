
#include <Message.h>

#include "gtest/gtest.h"

namespace unit_tests {
TEST(MessageTest, serializeAndDeserialize) {
  std::vector<char> toDeserialize{0x00, 0x00, 0x00, 0x05, 0x02,
                                  0x01, 0x02, 0x03, 0x04};

  int fd[2];

  pipe(fd);

  write(fd[1], &toDeserialize[0], toDeserialize.size());

  auto res = bittorrent::Message::read(fd[0]);

  EXPECT_EQ(res->messageID, bittorrent::MessageType::msgInterested);

  std::vector<char> expectedPayload(std::next(toDeserialize.begin(), 5),
                                    toDeserialize.end());

  EXPECT_EQ(res->payload, expectedPayload);

  EXPECT_EQ(res->serialize(), toDeserialize);

  close(fd[0]);
  close(fd[1]);
}
}  // namespace unit_tests
