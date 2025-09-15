cmake_minimum_required(VERSION 4.0)
project(JUCE_Audio_Player)

set(CMAKE_CXX_STANDARD 17)

add_executable(JUCE_Audio_Player main.cpp)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
include(cmake/cpm.cmake)