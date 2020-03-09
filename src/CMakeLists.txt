
find_package(OpenSSL)
find_package(CURL)
add_library(bittorrent_lib TorrentFile.cpp TorrentDownloader.cpp TrackerResponse.cpp Handshake.cpp Message.cpp)
target_include_directories(bittorrent_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# Setting this to the library only avoids errors in 3rd party dependencies which are compile with e.g. -Werror
target_compile_options(bittorrent_lib PUBLIC -Wall -pedantic)
# Include code-coverage settings:
target_link_libraries(bittorrent_lib PUBLIC coverage_config OpenSSL::Crypto PRIVATE bencode ${CURL_LIBRARIES})
