
#include <TorrentFile.h>
#include <openssl/sha.h>

#include <bencode.hpp>
#include <memory>

Sha1Hash TorrentFile::calculateInfoHash(std::string piecesHash,
                                        uint64_t pieceLength, uint64_t length,
                                        std::string name) {
  std::string bencodeInfoString;

  std::string piecesLengthBasicString(&pieceLength,
                                      &pieceLength + sizeof(pieceLength));
  std::string lengthBasicString(&length, &length + sizeof(length));

  bencodeInfoString += piecesHash;
  bencodeInfoString += piecesLengthBasicString;
  bencodeInfoString += lengthBasicString;
  bencodeInfoString += name;

  Sha1Hash infoHash;

  SHA1((const uint8_t*)bencodeInfoString.c_str(), bencodeInfoString.size(),
       infoHash.begin());

  return infoHash;
}

std::vector<Sha1Hash> TorrentFile::buildPieces(std::string hashStream) {
  std::size_t numberOfHashes = hashStream.size() / 20;

  std::vector<Sha1Hash> pieces;
  pieces.reserve(numberOfHashes);

  for (size_t i = 0; i < numberOfHashes; ++i) {
    Sha1Hash hash;
    std::copy(&hashStream[i], &hashStream[i + hash.size()], hash.begin());
    pieces.push_back(hash);
  }

  return pieces;
}

// integer = long long
// string = std::string
// list = std::vector<bencode::data>
// and dict = std::map<std::string, bencode::data>

/// \todo georgantas 2020-Feb-10 Try using boost::serialization
/// https://www.boost.org/doc/libs/1_72_0/libs/serialization/doc/tutorial.html
TorrentFile TorrentFile::BuildFromStream(std::istream& stream) {
  auto data = bencode::decode(stream);
  auto torrent = boost::get<bencode::dict>(data);
  auto announce = boost::get<bencode::string>(torrent["announce"]);
  auto bencodeInfo = boost::get<bencode::dict>(torrent["info"]);

  std::string piecesHash = boost::get<bencode::string>(bencodeInfo["pieces"]);
  std::uint64_t pieceLength =
      boost::get<bencode::integer>(bencodeInfo["piece length"]);
  std::uint64_t length = boost::get<bencode::integer>(bencodeInfo["length"]);
  std::string name = boost::get<bencode::string>(bencodeInfo["name"]);

  Sha1Hash infoHash = calculateInfoHash(piecesHash, pieceLength, length, name);
  std::vector<Sha1Hash> pieces = buildPieces(piecesHash);

  return TorrentFile(announce, infoHash, pieces, pieceLength, length, name);
}

TorrentFile::TorrentFile(std::string announce, Sha1Hash infoHash,
                         std::vector<Sha1Hash> piecesHash,
                         std::uint64_t pieceLength, std::uint64_t length,
                         std::string name)
    : announce(announce),
      infoHash(infoHash),
      piecesHash(piecesHash),
      pieceLength(pieceLength),
      length(length),
      name(name) {}

std::string& TorrentFile::get_announce() { return announce; }
