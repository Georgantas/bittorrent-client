
#include <iostream>
#include <string>

#pragma once

class TorrentFile {
 public:
  static TorrentFile BuildFromStream(std::istream& stream);

  std::string get_announce();

 private:
  TorrentFile(std::string announce);

  std::string announce;
};
