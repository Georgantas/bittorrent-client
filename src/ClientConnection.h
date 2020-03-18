
#pragma once

#include <Sha1Hash.h>
#include <TrackerResponse.h>

#include <optional>

namespace bittorrent {
class Bitfield;
class TorrentFile;
class Message;

class ClientConnection {
 public:
  ClientConnection(const peer& peer, const Sha1Hash& peerId,
                   const Sha1Hash& infoHash);

  ~ClientConnection();

  ClientConnection(const ClientConnection&) = delete;
  ClientConnection& operator=(const ClientConnection&) = delete;

  void sendUnchoke();

  void sendInterested();

  void sendNotInterested();

  void sendHave(uint32_t index);

  void sendRequest(uint32_t index, uint32_t begin, uint32_t length);

  std::optional<Message> readMessage();

  const Bitfield& getBitfield() const;

  bool isChocked() const;

  void setChoked(bool choked);

  void setBit(size_t index);

 private:
  int sock;
  peer _peer;
  Sha1Hash peerId;
  Sha1Hash infoHash;
  Bitfield bitfield;
  bool chocked;
};
}  // namespace bittorrent
