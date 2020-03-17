
#include <Message.h>
#include <unistd.h>
#include <memory>
#include <optional>
#include <sys/types.h>
#include <sys/socket.h>

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

std::optional<Message> Message::read(int fd) {
  unsigned char length[sizeof(uint32_t)];

  if (::read(fd, length, sizeof(uint32_t)) <= 0) {
    throw std::runtime_error("Error reading bytes.");
  }

  uint32_t payloadSize = (uint32_t(length[0]) << 24) |
                         (uint32_t(length[1]) << 16) |
                         (uint32_t(length[2]) << 8) | uint32_t(length[3]);

  if (payloadSize == 0) {
    // keep-alive message received
    return std::nullopt;
  }

  std::unique_ptr<char[]> data(new char[payloadSize + 1]);
  if (::read(fd, data.get(), payloadSize + 1) <= 0) {
    throw std::runtime_error("Error reading bytes.");
  }

  MessageType messageID = static_cast<MessageType>(data[0]);
  std::string payload = std::string(&data[1], &data[1] + payloadSize);

  return std::make_optional(Message{messageID, payload});
}
}  // namespace bittorrent
