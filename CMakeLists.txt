cmake_minimum_required(VERSION 4.0)
project(JUCE_Audio_Player)

set(CMAKE_CXX_STANDARD 20)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

include(cmake/cpm.cmake)

CPMAddPackage(
        NAME JUCE
        GITHUB_REPOSITORY juce-framework/JUCE
        GIT_TAG 8.0.10
        VERSION 8.0.10
        SOURCE_DIR ${LIB_DIR}/juce
)

# Add compiler warning utilities
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
include(cmake/Util.cmake)

# Adds all the targets configured in the "plugin" folder
add_subdirectory(plugin)

if(MSVC)
    add_compile_options(/Wall /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()