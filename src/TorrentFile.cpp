
#include <TorrentFile.h>

#include <bencode.hpp>

TorrentFile TorrentFile::BuildFromStream(std::istream& stream) {
  auto data = bencode::decode(stream);
  auto data2 = boost::get<bencode::dict>(data);
  auto announce = boost::get<bencode::string>(data2["announce"]);
  return TorrentFile(announce);
}

TorrentFile::TorrentFile(std::string announce) : announce(announce) {}

std::string TorrentFile::get_announce() { return announce; }
