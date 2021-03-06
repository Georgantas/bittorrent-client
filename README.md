# Bittorrent client
[![Build Status](https://travis-ci.org/Georgantas/bittorrent-client.svg?branch=master)](https://travis-ci.org/Georgantas/bittorrent-client)

[![codecov](https://codecov.io/gh/Georgantas/bittorrent-client/branch/master/graph/badge.svg)](https://codecov.io/gh/Georgantas/bittorrent-client)

This project implements the bittorrent P2P protocol: https://www.bittorrent.org/beps/bep_0003.html

https://wiki.theory.org/index.php/BitTorrentSpecification

# TODO
- Clean code (first do the obvious, and then go through effective c++, keep comments of strategies used)
- Test
- Resolve all "todo" comments.
- Fix active workers.
- Make stable.
- Fix code cov (lcov incomplatible with gcc-8?)
- Improve CMake: https://foonathan.net/2016/03/cmake-install/
- Can use C++ bitfields?
- Can we use std::future as a scope guard?
- Update README.md

# Usage
`torrent-cli [.torrent file] [output file name]`

# Example
`torrent-cli file.torrent output.iso`

First file: Using this client.

Bottom: Using transmission cli daemon.

![pic](pic.png)

Sha1Sums match.
