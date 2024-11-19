cmake_minimum_required(VERSION 3.10)
project(CacheLibrary)

set(CMAKE_CXX_STANDARD 17)

# GoogleTest
add_subdirectory(extern/googletest)
include_directories(src)

# Cache Interface
add_library(cache_interface INTERFACE)
target_include_directories(cache_interface INTERFACE src)

# LFU Cache
add_library(lfu_cache INTERFACE)
target_include_directories(lfu_cache INTERFACE src)
target_link_libraries(lfu_cache INTERFACE cache_interface)

# LRU Cache
add_library(lru_cache INTERFACE)
target_include_directories(lru_cache INTERFACE src)
target_link_libraries(lru_cache INTERFACE cache_interface)

# Tests
enable_testing()

add_executable(test_lfu tests/test_lfu_cache.cpp)
target_link_libraries(test_lfu lfu_cache gtest gtest_main)
add_test(NAME TestLFU COMMAND test_lfu)

add_executable(test_lru tests/test_lru_cache.cpp)
target_link_libraries(test_lru lru_cache gtest gtest_main)
add_test(NAME TestLRU COMMAND test_lru)
