cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(MiniSQL CXX)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wno-deprecated-declarations")

# boost
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED OFF)
find_package(Boost 1.58 REQUIRED COMPONENTS system filesystem)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

# glog
find_package(Glog)
include_directories(${Glog_INCLUDE_DIRS})
link_directories(${Glog_LIBRARY_DIRS})

# gperftools
find_package(Gperftools)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/minisql)

add_subdirectory(minisql)
add_subdirectory(examples)
add_subdirectory(test)