
#include <Message.h>
#include <Utilities.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <memory>
#include <optional>

namespace bittorrent {

std::vector<char> Message::serialize() {
  std::vector<char> result;
  uint32_t payloadSize = payload.size() + 1;  // + 1 for id
  std::vector<char> payloadSizeVector = toVector(payloadSize);
  result.insert(result.end(), payloadSizeVector.begin(),
                payloadSizeVector.end());
  result.push_back(messageID);
  result.insert(result.end(), payload.begin(), payload.end());

  return result;
}

size_t Message::parsePiece(Message message, size_t index,
                           std::vector<char> &buf) {
  if (message.messageID != MessageType::msgPiece) {
    throw std::runtime_error("Wrong message id!");
  }
  if (message.payload.size() < 8) {
    throw std::runtime_error("Payload too short.");
  }
  uint32_t parsedIndex = toUnsignedInt(
      std::vector<char>(message.payload.begin(), message.payload.begin() + 4));
  if (parsedIndex != index) {
    throw std::runtime_error("Wrong index!");
  }
  uint32_t begin = toUnsignedInt(std::vector<char>(
      message.payload.begin() + 4, message.payload.begin() + 8));
  if (begin >= buf.size()) {
    throw std::runtime_error("Offset too high!");
  }
  /*if begin+len(data) > len(buf) {
                return 0, fmt.Errorf("Data too long [%d] for offset %d with
     length %d", len(data), begin, len(buf))
        }*/
  std::copy(std::next(message.payload.begin(), 8), message.payload.end(),
            &buf[begin]);
  return message.payload.size() - 8;
}

size_t Message::parseHave(Message message) {
  if (message.messageID != MessageType::msgHave) {
    throw std::runtime_error("Wrong message id!");
  }
  if (message.payload.size() != 4) {
    throw std::runtime_error("Payload length not 4!");
  }
  uint32_t index = toUnsignedInt(message.payload);
  return index;
}

/// https://stackoverflow.com/questions/2295737/case-when-blocking-recv-returns-less-than-requested-bytes
int readn(int f, void *av, int n) {
  char *a;
  int m, t;

  a = (char *)av;
  t = 0;
  while (t < n) {
    m = read(f, a + t, n - t);
    if (m <= 0) {  // <= or < ? 0 signifies connection closed? fail on timeout?
      if (t == 0) return m;
      break;
    }
    t += m;
  }
  return t;
}

std::optional<Message> Message::read(int fd) {
  std::vector<char> length(sizeof(uint32_t));

  int test = readn(fd, &length[0], sizeof(uint32_t));
  int test2;
  if (test != sizeof(uint32_t)) {
    test2 = errno;
    throw std::runtime_error("Error reading bytes. (message #1)");
  }

  // if (::read(fd, length, sizeof(uint32_t)) <= 0) {
  //  throw std::runtime_error("Error reading bytes.");
  //}

  uint32_t payloadSize = toUnsignedInt(length);

  if (payloadSize == 0) {
    // keep-alive message received
    return std::nullopt;
  }

  std::unique_ptr<char[]> data(new char[payloadSize]);

  if (readn(fd, data.get(), payloadSize) != payloadSize) {
    throw std::runtime_error("Error reading bytes. (message #2)");
  }
  // if (::read(fd, data.get(), payloadSize) <= 0) {
  //  throw std::runtime_error("Error reading bytes.");
  //}

  MessageType messageID = static_cast<MessageType>(data[0]);
  std::vector<char> payload(&data[1], &data[payloadSize]);

  return std::make_optional(Message{messageID, payload});
}
}  // namespace bittorrent
