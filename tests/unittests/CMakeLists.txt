
find_package(GTest)
add_executable(unit_tests TorrentFile_Test.cpp TorrentDownloader_Test.cpp TrackerResponse_Test.cpp)
# target_include_directories(unit_tests PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
target_link_libraries(unit_tests PRIVATE gtest_main bittorrent_lib) # gtest_main provides the main
add_test(NAME Unit_Tests COMMAND unit_tests)

file(COPY archlinux.iso.torrent DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
