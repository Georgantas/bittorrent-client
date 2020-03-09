
#pragma once

#include <string>

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
  static Message deserialize(std::string serial);
};

}  // namespace bittorrent
