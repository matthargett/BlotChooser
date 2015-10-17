cmake_minimum_required(VERSION 3.3)
project(which_inks)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp InkParser.cpp InkParser.h Ink.h RequestedColorParser.cpp RequestedColorParser.h Color.cpp Color.h RequestedColor.h)
add_executable(which_inks ${SOURCE_FILES})