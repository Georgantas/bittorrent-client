
#pragma once

#include <Sha1Hash.h>

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

namespace bittorrent {
class TorrentFile {
 public:
  TorrentFile(std::string announce, Sha1Hash infoHash,
              std::vector<Sha1Hash> piecesHash, std::uint64_t pieceLength,
              std::uint64_t length, std::string name);

  const std::string& getAnnounce() const;
  const Sha1Hash& getInfoHash() const;
  const std::vector<Sha1Hash>& getPiecesHash() const;
  const std::uint64_t& getPieceLength() const;
  const std::uint64_t& getLength() const;
  const std::string& getName() const;

  static TorrentFile buildFromStream(std::istream& stream);

  /// \todo georgantas feb-23-2020 Refactor to use pimpl
 private:
  std::string announce;  // url of the tracker
  Sha1Hash infoHash;
  std::vector<Sha1Hash> piecesHash;  // hashes of all the pieces
  std::uint64_t pieceLength;
  std::uint64_t length;
  std::string name;
};
}  // namespace bittorrent
