
#include <Bitfield.h>
#include <ClientConnection.h>
#include <Handshake.h>
#include <Message.h>
#include <TorrentFile.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <memory>

namespace bittorrent {
ClientConnection::ClientConnection(const peer& peer, const Sha1Hash& peerId,
                                   const Sha1Hash& infoHash)
    : _peer(peer),
      peerId(peerId),
      infoHash(infoHash),
      bitfield(""),
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

  if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    throw std::runtime_error("Connection Failed");
  }

  /*
  struct timeval timeout;
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;

  if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout,
                 sizeof(timeout)) < 0) {
    throw std::runtime_error("Could not set receive timeout.");
  }

  if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout,
                 sizeof(timeout)) < 0) {
    throw std::runtime_error("Could not set send timeout.");
  }
  */

  // complete handshake
  Handshake handshake{"BitTorrent protocol", infoHash, peerId};
  auto handshakeSerial = handshake.serialize();

  send(sock, handshakeSerial.c_str(), handshakeSerial.size(), 0);

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
  send(sock, msg.c_str(), msg.size(), 0);
}

void ClientConnection::sendInterested() {
  auto msg = Message{MessageType::msgInterested}.serialize();
  send(sock, msg.c_str(), msg.size(), 0);
}

void ClientConnection::sendNotInterested() {
  auto msg = Message{MessageType::msgNotInterested}.serialize();
  send(sock, msg.c_str(), msg.size(), 0);
}

void ClientConnection::sendHave(uint32_t index) {
  std::string payload = {char(index >> 24), char(index >> 16), char(index >> 8),
                         char(index)};
  auto msg = Message{MessageType::msgHave, payload}.serialize();
  send(sock, msg.c_str(), msg.size(), 0);
}

void ClientConnection::SendRequest(uint32_t index, uint32_t begin,
                                   uint32_t length) {
  std::string payload = {
      char(index >> 24),  char(index >> 16),  char(index >> 8),  char(index),
      char(begin >> 24),  char(begin >> 16),  char(begin >> 8),  char(begin),
      char(length >> 24), char(length >> 16), char(length >> 8), char(length)};
  auto msg = Message{MessageType::msgRequest, payload}.serialize();
  send(sock, msg.c_str(), msg.size(), 0);
}

std::optional<Message> ClientConnection::readMessage() {
  return Message::read(sock);
}

const Bitfield& ClientConnection::getBitfield() const { return bitfield; }

}  // namespace bittorrent
