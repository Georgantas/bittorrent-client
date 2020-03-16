
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
                                   const Sha1Hash& infoHash,
                                   const TorrentFile& torrentFile)
    : _peer(peer),
      peerId(peerId),
      infoHash(infoHash),
      bitfield({0}),
      chocked(true) {
  sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock < 0) {
    throw std::runtime_error("Socket creation failure.");
  }

  /*
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout,
                   sizeof(timeout)) < 0) {
      throw std::runtime_error("Could not set receive timeout.");
    }

    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout,
                   sizeof(timeout)) < 0) {
      throw std::runtime_error("Could not set send timeout.");
    }*/

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(peer.port);
  serv_addr.sin_addr =
      in_addr{uint32_t(peer.ip[0]) | (uint32_t(peer.ip[1]) << 8) |
              (uint32_t(peer.ip[2]) << 16) | (uint32_t(peer.ip[3]) << 24)};

  if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    auto test = errno;
    throw std::runtime_error("Connection Failed");
  }

  // complete handshake
  Handshake h{"BitTorrent protocol", infoHash, peerId};
  auto handshakeSerial = h.serialize();

  send(sock, handshakeSerial.c_str(), handshakeSerial.size(), 0);
  std::unique_ptr<char[]> buffer(new char[handshakeSerial.size()]);
  if (read(sock, buffer.get(), handshakeSerial.size()) < 0 ||
      handshakeSerial != std::string(buffer.get())) {
    throw std::runtime_error("Handshake failed.");
  }

  // receive the bitfield
  std::unique_ptr<char[]> buffer2(new char[torrentFile.getPiecesHash().size()]);

  if (read(sock, buffer2.get(), torrentFile.getPiecesHash().size()) < 0) {
    throw std::runtime_error("Could not receive the bitfield.");
  }

  Message message = Message::deserialize(buffer2.get());
  if (message.messageID != MessageType::msgBitfield) {
    throw std::runtime_error("Received wrong message. Bitfield not received.");
  }
}

ClientConnection::~ClientConnection() { close(sock); }

void ClientConnection::sendUnchoke() {}

void ClientConnection::sendInterested() {}

void ClientConnection::sendHave(size_t index) {}

const Bitfield& ClientConnection::getBitfield() const { return bitfield; }

}  // namespace bittorrent
