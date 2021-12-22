# Minimum required version of CMake for a project
cmake_minimum_required(VERSION 3.20)

# Add a library target to be built from the source files
add_library(${PROJECT_NAME} SHARED
    # Headers
    "include/adminskit/cvars.h"
    "include/adminskit/localization.h"
    "include/adminskit/temp_entities.h"
    "include/adminskit/utils.h"

    # Sources
    "src/bootstrap.cpp"
    "src/main.cpp"
    "src/temp_entities.cpp"
    "src/utils.cpp"
 )

# Add include directories to a target
target_include_directories(${PROJECT_NAME} PRIVATE
    "include"
)

#-------------------------------------------------------------------------------------------
# AMXX configuration
#-------------------------------------------------------------------------------------------

# Use Metamod SDK (ON/OFF)
set(AMXX_USE_METAMOD ON)

# Uncomment the functions you want to use in your code and specify the desired function names
set(AMXX_QUERY "Main")                                     # void OnAmxxQuery();
set(AMXX_ATTACH "__OnAmxxAttach")                          # AmxxStatus OnAmxxAttach();
#set(AMXX_DETACH "__OnAmxxDetach")                         # void OnAmxxDetach();
#set(AMXX_CHECK_GAME "__OnAmxxCheckGame")                  # AmxxGameStatus OnAmxxCheckGame(const char* game);
#set(AMXX_PLUGINS_LOADED "__OnAmxxPluginsLoaded")          # void OnAmxxPluginsLoaded();
#set(AMXX_PLUGINS_UNLOADED "__OnAmxxPluginsUnloaded")      # void OnAmxxPluginsUnloaded();
#set(AMXX_PLUGINS_UNLOADING "__OnAmxxPluginsUnloading")    # void OnAmxxPluginsUnloading();

#-------------------------------------------------------------------------------------------
# Metamod configuration
#-------------------------------------------------------------------------------------------

# Uncomment the functions you want to use in your code and specify the desired function names
#set(META_INIT "__OnMetaInit")       # void OnMetaInit();
#set(META_QUERY "__OnMetaQuery")     # void OnMetaQuery();
#set(META_ATTACH "__OnMetaAttach")   # MetaStatus OnMetaAttach();
set(META_DETACH "__OnMetaDetach")    # void OnMetaDetach();

#-------------------------------------------------------------------------------------------
# Add dependencies to the build
#-------------------------------------------------------------------------------------------

# Add subdirectories
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/cssdk" "cssdk")
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/metamod" "metamod")
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/amxx" "amxx")
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/core" "core")
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/mhooks" "mhooks")
add_subdirectory("${CMAKE_SOURCE_DIR}/plugins/admin_free_look" "admin_free_look")
add_subdirectory("${CMAKE_SOURCE_DIR}/plugins/invisible_spectator" "invisible_spectator")
add_subdirectory("${CMAKE_SOURCE_DIR}/plugins/server_stats" "server_stats")
add_subdirectory("${CMAKE_SOURCE_DIR}/plugins/unlimited_choose_team" "unlimited_choose_team")
add_subdirectory("${CMAKE_SOURCE_DIR}/plugins/whdh" "whdh")

# Link dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE
    "cssdk"
    "metamod"
    "amxx"
    "core"
    "mhooks"
    "admin_free_look"
    "invisible_spectator"
    "server_stats"
    "unlimited_choose_team"
    "whdh"
)

#-------------------------------------------------------------------------------------------
# Publish build artifacts
#-------------------------------------------------------------------------------------------

if(PUBLISH_ARTIFACTS)
    set(GITHUB_ENV "artifacts_zip_name=${INFO_PROJECT_NAME}_v${CMAKE_PROJECT_VERSION}_${CMAKE_SYSTEM_NAME}")
    string(REGEX REPLACE "[ \t\r\n]" "" GITHUB_ENV "${GITHUB_ENV}")
    file(WRITE "$ENV{PUBLISH_DIR}/github.env" "${GITHUB_ENV}")

    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/third_party" "$ENV{PUBLISH_DIR}"
    )
endif()
