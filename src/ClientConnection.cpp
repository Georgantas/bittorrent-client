
#include <Bitfield.h>
#include <ClientConnection.h>
#include <Handshake.h>
#include <Message.h>
#include <TorrentFile.h>
#include <Utilities.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <memory>
#include <vector>

namespace bittorrent {
ClientConnection::ClientConnection(const peer& peer, const Sha1Hash& peerId,
                                   const Sha1Hash& infoHash)
    : _peer(peer),
      peerId(peerId),
      infoHash(infoHash),
      bitfield({}),
      chocked(true) {
  sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock < 0) {
    throw std::runtime_error("Socket creation failure.");
  }

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(peer.port);
  serv_addr.sin_addr =
      in_addr{uint32_t(peer.ip[0]) | (uint32_t(peer.ip[1]) << 8) |
              (uint32_t(peer.ip[2]) << 16) | (uint32_t(peer.ip[3]) << 24)};

  struct timeval timeout;
  timeout.tv_sec = 30;
  timeout.tv_usec = 0;

  if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout,
                 sizeof(timeout)) < 0) {
    throw std::runtime_error("Could not set receive timeout.");
  }

  if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout,
                 sizeof(timeout)) < 0) {
    throw std::runtime_error("Could not set send timeout.");
  }

  if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    throw std::runtime_error("Connection Failed");
  }

  // complete handshake
  Handshake handshake{
      std::vector<char>({'B', 'i', 't', 'T', 'o', 'r', 'r', 'e', 'n', 't', ' ',
                         'p', 'r', 'o', 't', 'o', 'c', 'o', 'l'}),
      infoHash, peerId};
  auto handshakeSerial = handshake.serialize();

  /// \todo create a wrapper for send
  if (send(sock, &handshakeSerial[0], handshakeSerial.size(), 0) < 0) {
    throw std::runtime_error("Error sending bytes.");
  }

  Handshake handshakeResponse = Handshake::read(sock);

  if (handshakeResponse.infoHash != handshake.infoHash) {
    throw std::runtime_error("Handshake failed.");
  }

  // receive the bitfield
  auto message = readMessage();
  if (!message || (message->messageID != MessageType::msgBitfield)) {
    throw std::runtime_error("Could not receive bitfield.");
  }

  bitfield = Bitfield{message->payload};
}

ClientConnection::~ClientConnection() { close(sock); }

void ClientConnection::sendUnchoke() {
  auto msg = Message{MessageType::msgUnchoke}.serialize();
  if (send(sock, &msg[0], msg.size(), 0) < 0) {
    throw std::runtime_error("Error sending bytes.");
  }
}

void ClientConnection::sendInterested() {
  auto msg = Message{MessageType::msgInterested}.serialize();
  if (send(sock, &msg[0], msg.size(), 0) < 0) {
    throw std::runtime_error("Error sending bytes.");
  }
}

void ClientConnection::sendNotInterested() {
  auto msg = Message{MessageType::msgNotInterested}.serialize();
  if (send(sock, &msg[0], msg.size(), 0) < 0) {
    throw std::runtime_error("Error sending bytes.");
  }
}

void ClientConnection::sendHave(uint32_t index) {
  std::vector<char> payload = toVector(index);
  auto msg = Message{MessageType::msgHave, payload}.serialize();
  if (send(sock, &msg[0], msg.size(), 0) < 0) {
    throw std::runtime_error("Error sending bytes.");
  }
}

bool ClientConnection::isChocked() const { return chocked; }

void ClientConnection::sendRequest(uint32_t index, uint32_t begin,
                                   uint32_t length) {
  auto indexVector = toVector(index);
  auto beginVector = toVector(begin);
  auto lengthVector = toVector(length);
  std::vector<char> payload;
  payload.insert(payload.end(), indexVector.begin(), indexVector.end());
  payload.insert(payload.end(), beginVector.begin(), beginVector.end());
  payload.insert(payload.end(), lengthVector.begin(), lengthVector.end());
  auto msg = Message{MessageType::msgRequest, payload}.serialize();
  if (send(sock, &msg[0], msg.size(), 0) < 0) {
    throw std::runtime_error("Error sending request.");
  }
}

std::optional<Message> ClientConnection::readMessage() {
  return Message::read(sock);
}

void ClientConnection::setChoked(bool chocked) { this->chocked = chocked; }

const Bitfield& ClientConnection::getBitfield() const { return bitfield; }

void ClientConnection::setBit(size_t index) { bitfield.setPiece(index); }

}  // namespace bittorrent
