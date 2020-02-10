
add_library(bencode INTERFACE)
target_include_directories(bencode INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
find_package(Boost)
