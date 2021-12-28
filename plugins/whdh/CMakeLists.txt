# Minimum required version of CMake for a project
cmake_minimum_required(VERSION 3.20)

# Declare project
project("whdh")

# Add a library target to be built from the source files
add_library(${PROJECT_NAME} INTERFACE)

# Add include directories to a target
target_include_directories(${PROJECT_NAME} INTERFACE "include")

# Add header and source files to target sources list
target_sources(${PROJECT_NAME} INTERFACE
    # Headers
    "include/whdh/cvars.h"
    "src/beams.h"
    "src/data.h"
    "src/marker.h"
    "src/plugin.h"
    "src/preferences.h"
    "src/preferences_bin.h"
    "src/resources.h"

    # Sources
    "src/beams.cpp"
    "src/cvars.cpp"
    "src/data.cpp"
    "src/main.cpp"
    "src/marker.cpp"
    "src/plugin.cpp"
    "src/preferences.cpp"
    "src/preferences_bin.cpp"
    "src/resources.cpp"
)
