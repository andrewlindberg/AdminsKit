# Minimum required version of CMake for a project
cmake_minimum_required(VERSION 3.20)

# Declare project
project("server_stats")

# Add a library target to be built from the source files
add_library(${PROJECT_NAME} INTERFACE)

# Add include directories to a target
target_include_directories(${PROJECT_NAME} INTERFACE "include")

# Add header and source files to target sources list
target_sources(${PROJECT_NAME} INTERFACE
    # Headers
    "include/server_stats/cvars.h"
    "src/plugin.h"

    # Sources
    "src/cvars.cpp"
    "src/main.cpp"
    "src/plugin.cpp"
)
