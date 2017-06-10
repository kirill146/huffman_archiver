cmake_minimum_required(VERSION 3.7)
project(huffman_archiver)

set(CMAKE_CXX_FLAGS "-std=c++11 -O2")

add_subdirectory (huffman)

add_executable (huffman_archiver main.cpp)
target_link_libraries (huffman_archiver LINK_PUBLIC huffman)