
add_executable(bittorrent main.cpp)
target_include_directories(bittorrent PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# Setting this to the library only avoids errors in 3rd party dependencies which are compile with e.g. -Werror
target_compile_options(bittorrent PUBLIC -Wall -pedantic)
# Include code-coverage settings:
target_link_libraries(bittorrent PRIVATE bittorrent_lib)
