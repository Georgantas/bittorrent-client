cmake_minimum_required (VERSION 3.1)
project(Bittorrent_client LANGUAGES CXX)

# use set_target_properties instead?
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # for portability

# coverage_config only for adding --coverage flag to linker and compiler
# when DCODE_COVERAGE flag is set
add_library(coverage_config INTERFACE)

option(CODE_COVERAGE "Enable coverage reporting" OFF)
if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  # Add required flags (GCC & LLVM/Clang)
  target_compile_options(coverage_config INTERFACE
    -O0        # no optimization
    -g         # generate debug info
    --coverage # sets all required flags
  )
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    target_link_options(coverage_config INTERFACE --coverage)
  else()
    target_link_libraries(coverage_config INTERFACE --coverage)
  endif()
endif()

# todo: integrate include-what-you-use
# https://stackoverflow.com/questions/30951492/how-to-use-the-tool-include-what-you-use-together-with-cmake-to-detect-unused-he

# todo: integrate clang-tidy
# http://mariobadr.com/using-clang-tidy-with-cmake-36.html

add_subdirectory(libraries)
add_subdirectory(src)

option (BUILD_TESTING "Build the testing tree." ON)
# Only build tests if we are the top-level project
# Allows this to be used by super projects with `add_subdirectory`
if (BUILD_TESTING AND (PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
  enable_testing()
  add_subdirectory(tests)
endif()
