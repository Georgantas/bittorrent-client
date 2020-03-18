
#pragma once

#include <optional>
#include <vector>

namespace bittorrent {
enum MessageType {
  msgChoke,
  msgUnchoke,
  msgInterested,
  msgNotInterested,
  msgHave,
  msgBitfield,
  msgRequest,
  msgPiece,
  msgCancel
};

struct Message {
  MessageType messageID;
  std::vector<char> payload;

  std::vector<char> serialize();
  static std::optional<Message> read(int fd);
  static size_t parseHave(Message message);
  static size_t parsePiece(Message message, size_t index,
                           std::vector<char>& buf);
};

}  // namespace bittorrent
