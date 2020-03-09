
#include <Message.h>

namespace bittorrent {

std::string Message::serialize() {
  std::string result;
  uint32_t payloadSize = payload.size();
  result += payloadSize >> 24;
  result += payloadSize >> 16;
  result += payloadSize >> 8;
  result += payloadSize;
  result += messageID;
  result += payload;

  return result;
}

Message Message::deserialize(std::string serial) {
  uint32_t payloadSize = uint32_t(serial[0]) << 24;
  payloadSize |= (uint32_t(serial[1]) << 16);
  payloadSize |= (uint32_t(serial[2]) << 8);
  payloadSize |= uint32_t(serial[3]);
  MessageType messageID = static_cast<MessageType>(serial[4]);
  std::string payload = std::string(&serial[5], &serial[5] + payloadSize);

  return {messageID, payload};
}
}  // namespace bittorrent
