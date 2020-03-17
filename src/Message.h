
#pragma once

#include <string>
#include <optional>

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
  std::string payload;

  std::string serialize();
  static std::optional<Message> read(int fd);
};

}  // namespace bittorrent
