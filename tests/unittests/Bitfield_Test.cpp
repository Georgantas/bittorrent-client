
#include <Bitfield.h>

#include "gtest/gtest.h"

namespace unit_tests {
TEST(BitfieldTest, setBitAndHasPiece) {
  std::vector<unsigned char> bits{0x01, 0x02, 0x03};
  bittorrent::Bitfield bitfield(bits);
  EXPECT_EQ(true, bitfield.hasPiece(7));
  EXPECT_EQ(false, bitfield.hasPiece(6));
  bitfield.setPiece(6);
  EXPECT_EQ(true, bitfield.hasPiece(6));
}
}  // namespace unit_tests
