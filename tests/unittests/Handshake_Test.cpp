
#include <Handshake.h>

#include "gtest/gtest.h"

namespace unit_tests {
TEST(HandshakeTest, serializeAndDeserialize) {
  std::vector<char> toDeserialize{
      char(0x13), 'B',        'i',        't',        'T',        'o',
      'r',        'r',        'e',        'n',        't',        ' ',
      'p',        'r',        'o',        't',        'o',        'c',
      'o',        'l',        char(0x00), char(0x00), char(0x00), char(0x00),
      char(0x00), char(0x00), char(0x00), char(0x00), char(0x86), char(0xd4),
      char(0xc8), char(0x00), char(0x24), char(0xa4), char(0x69), char(0xbe),
      char(0x4c), char(0x50), char(0xbc), char(0x5a), char(0x10), char(0x2c),
      char(0xf7), char(0x17), char(0x80), char(0x31), char(0x00), char(0x74),
      '-',        'T',        'R',        '2',        '9',        '4',
      '0',        '-',        'k',        '8',        'h',        'j',
      '0',        'w',        'g',        'e',        'j',        '6',
      'c',        'h'};
  int fd[2];
  pipe(fd);

  write(fd[1], &toDeserialize[0], toDeserialize.size());
  auto res = bittorrent::Handshake::read(fd[0]);

  EXPECT_EQ(res.pstr,
            std::vector<char>({'B', 'i', 't', 'T', 'o', 'r', 'r', 'e', 'n', 't',
                               ' ', 'p', 'r', 'o', 't', 'o', 'c', 'o', 'l'}));
  bittorrent::Sha1Hash expectedInfoHash;
  std::copy(toDeserialize.begin() + 28, toDeserialize.begin() + 48,
            expectedInfoHash.begin());
  EXPECT_EQ(res.infoHash, expectedInfoHash);

  bittorrent::Sha1Hash expectedPeerId;
  std::copy(toDeserialize.begin() + 48, toDeserialize.end(),
            expectedPeerId.begin());
  EXPECT_EQ(res.peerId, expectedPeerId);

  EXPECT_EQ(res.serialize(), toDeserialize);

  close(fd[0]);
  close(fd[1]);
}
}  // namespace unit_tests
