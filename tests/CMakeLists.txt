
add_executable(tests complex_main.cpp)
target_link_libraries(tests PRIVATE bittorrent_lib)

add_test(NAME SampleTest COMMAND tests)

add_subdirectory(unittests)
