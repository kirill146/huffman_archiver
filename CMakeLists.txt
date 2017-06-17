cmake_minimum_required(VERSION 2.7)
project(huffman_archiver)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++11 -Wall -pedantic")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
endif()

add_subdirectory (huffman)

add_executable (huffman_archiver main.cpp)
target_link_libraries (huffman_archiver LINK_PUBLIC huffman)
