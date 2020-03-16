
#pragma once

#include <Sha1Hash.h>
#include <TrackerResponse.h>

namespace bittorrent {
class Bitfield;
class TorrentFile;

class ClientConnection {
 public:
  ClientConnection(const peer& peer, const Sha1Hash& peerId,
                   const Sha1Hash& infoHash, const TorrentFile& torrentFile);

  ~ClientConnection();

  ClientConnection(const ClientConnection&) = delete;
  ClientConnection& operator=(const ClientConnection&) = delete;

  void sendUnchoke();

  void sendInterested();

  void sendHave(size_t index);

  const Bitfield& getBitfield() const;

 private:
  int sock;
  peer _peer;
  Sha1Hash peerId;
  Sha1Hash infoHash;
  Bitfield bitfield;
  bool chocked;
};
}  // namespace bittorrent
