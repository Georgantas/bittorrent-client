
#include <Sha1Hash.h>

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#pragma once

class TorrentFile {
 public:
  static TorrentFile BuildFromStream(std::istream& stream);

  std::string& getAnnounce();
  Sha1Hash& getInfoHash();
  std::vector<Sha1Hash>& getPiecesHash();
  std::uint64_t& getPieceLength();
  std::uint64_t& getLength();
  std::string& getName();

 private:
  TorrentFile(std::string announce, Sha1Hash infoHash,
              std::vector<Sha1Hash> piecesHash, std::uint64_t pieceLength,
              std::uint64_t length, std::string name);

  static Sha1Hash calculateInfoHash(std::string piecesHash,
                                    uint64_t pieceLength, uint64_t length,
                                    std::string name);

  static std::vector<Sha1Hash> buildPieces(std::string hashes);

  std::string announce;  // url of the tracker
  Sha1Hash infoHash;
  std::vector<Sha1Hash> piecesHash;  // hashes of all the pieces
  std::uint64_t pieceLength;
  std::uint64_t length;
  std::string name;
};
