
#include <TorrentFile.h>
#include <openssl/sha.h>

#include <bencode.hpp>
#include <memory>

namespace {
bittorrent::Sha1Hash calculateInfoHash(std::string bencodeInfo) {
  bittorrent::Sha1Hash infoHash;

  SHA1((const uint8_t*)bencodeInfo.c_str(), bencodeInfo.size(),
       infoHash.begin());

  return infoHash;
}

std::vector<bittorrent::Sha1Hash> buildPieces(std::string hashStream) {
  std::size_t numberOfHashes = hashStream.size() / 20;

  std::vector<bittorrent::Sha1Hash> pieces;
  pieces.reserve(numberOfHashes);

  for (size_t i = 0; i < numberOfHashes; ++i) {
    bittorrent::Sha1Hash hash;
    std::copy(&hashStream[i * 20], &hashStream[i * 20 + hash.size()],
              hash.begin());
    pieces.push_back(hash);
  }

  return pieces;
}
}  // namespace

namespace bittorrent {
// integer = long long
// string = std::string
// list = std::vector<bencode::data>
// and dict = std::map<std::string, bencode::data>

/// \todo georgantas 2020-Feb-10 Try using boost::serialization
/// https://www.boost.org/doc/libs/1_72_0/libs/serialization/doc/tutorial.html
/// Archive implementations:
/// https://sourceforge.net/p/protoc/code/ci/master/tree/
TorrentFile TorrentFile::buildFromStream(std::istream& stream) {
  auto data = bencode::decode(stream);
  auto torrent = boost::get<bencode::dict>(data);
  auto announce = boost::get<bencode::string>(torrent["announce"]);
  auto bencodeInfo = boost::get<bencode::dict>(torrent["info"]);

  std::string piecesHash = boost::get<bencode::string>(bencodeInfo["pieces"]);
  std::uint64_t pieceLength =
      boost::get<bencode::integer>(bencodeInfo["piece length"]);
  std::uint64_t length = boost::get<bencode::integer>(bencodeInfo["length"]);
  std::string name = boost::get<bencode::string>(bencodeInfo["name"]);

  Sha1Hash infoHash = calculateInfoHash(bencode::encode(bencodeInfo));
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

const std::string& TorrentFile::getAnnounce() const { return announce; }
const Sha1Hash& TorrentFile::getInfoHash() const { return infoHash; }
const std::vector<Sha1Hash>& TorrentFile::getPiecesHash() const {
  return piecesHash;
}
const std::uint64_t& TorrentFile::getPieceLength() const { return pieceLength; }
const std::uint64_t& TorrentFile::getLength() const { return length; }
const std::string& TorrentFile::getName() const { return name; }
}  // namespace bittorrent
