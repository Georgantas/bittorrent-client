
find_package(GTest)
add_executable(unit_tests test.cpp)
# target_include_directories(unit_tests PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
target_link_libraries(unit_tests PRIVATE gtest_main bittorrent_lib) # gtest_main provides the main
add_test(NAME Unit_Tests COMMAND unit_tests)
