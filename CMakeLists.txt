cmake_minimum_required(VERSION 3.27)
project(muduo_net)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(base)
include_directories(net)

#aux_source_directory(base BASE_LIST)
#aux_source_directory(net NET_LIST)

#add_library(muduo_net ${BASE_LIST} ${NET_LIST})
#install(TARGETS muduo_net)
add_subdirectory(net)
add_subdirectory(base)
