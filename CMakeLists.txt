#-------------------------------------------------------------------------------------------
# Compile with MSVC (Windows):
#   cmake --preset win32-msvc-release && cmake --build --preset win32-msvc-release
#
# Compile with Clang-CL (Windows):
#   cmake --preset win32-clang-release && cmake --build --preset win32-clang-release
#
# Compile with GCC (Linux):
#   cmake --preset linux-gcc-release && cmake --build --preset linux-gcc-release
#
# Compile with Clang (Linux):
#   cmake --preset linux-clang-release && cmake --build --preset linux-clang-release
#
# Compile with Intel C++ Compiler (Linux):
#   cmake --preset linux-intel-release && cmake --build --preset linux-intel-release
#-------------------------------------------------------------------------------------------

# Minimum required version of CMake for a project
cmake_minimum_required(VERSION 3.21)

#-------------------------------------------------------------------------------------------
# Project configuration
#-------------------------------------------------------------------------------------------

set(INFO_PROJECT_NAME          "Admins Kit")
set(INFO_PROJECT_DESCRIPTION   "AMXX Module")
set(INFO_PROJECT_AUTHOR        "the_hunter")
set(INFO_PROJECT_URL           "https://dev-cs.ru")
set(INFO_VERSION_MAJOR         "2")
set(INFO_VERSION_MINOR         "0")
set(INFO_VERSION_PATCH         "0")
set(BINARY_NAME_SUFFIX_WINDOWS "_amxx.dll")
set(BINARY_NAME_SUFFIX_LINUX   "_amxx_i386.so")
set(BINARY_NAME_SUFFIX_DARWIN  "_amxx.dylib")

string(TOLOWER "${INFO_PROJECT_NAME}" PROJECT_NAME)
string(REGEX REPLACE "[ \t\r\n]" "" PROJECT_NAME "${PROJECT_NAME}")

# Declare project
project(${PROJECT_NAME}
    VERSION ${INFO_VERSION_MAJOR}.${INFO_VERSION_MINOR}.${INFO_VERSION_PATCH}
    DESCRIPTION "${INFO_PROJECT_DESCRIPTION}"
    HOMEPAGE_URL "${INFO_PROJECT_URL}"
)

#-------------------------------------------------------------------------------------------
# Options
#-------------------------------------------------------------------------------------------

# Common options
option(ENABLE_EXCEPTIONS       "Enable exception handling table generation."       OFF)
option(ENABLE_RTTI             "Enable RTTI support."                              OFF)
option(RUN_CLANG_TIDY          "Run clang-tidy checks."                            OFF)
option(RUN_PVS_STUDIO          "Run PVS-Studio analyzer."                          OFF)
option(APPEND_GIT_HASH         "Append a Git commit hash to the project version."  OFF)

# MSVC Options
option(MSVC_CODE_ANALYSIS      "Enable MSVC code analysis."                        OFF)
option(MSVC_AUTO_PARALLELIZER  "Enable auto-parallelizer feature of the compiler." ON)

# GCC options
option(LINKER_GOLD             "Use the Gold linker."                              ON)
option(GCC_ANALYZER            "Run GCC static analysis (GCC v10.0 and newer)."    OFF)

# Clang options
option(LINKER_LLD              "Use the LLD linker."                               ON)

# Intel C++ options
option(LINK_LIB_INTEL          "Static linking with the intel library."            OFF)
option(INTEL_ALL_DIAG_GROUPS   "Enable all diagnostic groups."                     OFF)

# GCC, Clang, Intel C++ options
option(RUN_CPPCHECK            "Run cppcheck."                                     OFF)
option(LINK_LIB_C              "Linking with the libc library."                    ON)
option(LINK_LIB_M              "Linking with the libm library."                    ON)
option(LINK_LIB_DL             "Linking with the libdl library."                   ON)
option(LINK_LIB_GCC            "Static linking with the libgcc library."           ON)
option(LINK_LIB_STDCPP         "Static linking with the libstdc++ library."        ON)

#-------------------------------------------------------------------------------------------
# Include additional CMake modules
#-------------------------------------------------------------------------------------------

include(Environment)
include(CodeAnalysis)

#-------------------------------------------------------------------------------------------
# Project build setup
#-------------------------------------------------------------------------------------------

if(APPEND_GIT_HASH)
    include(GetGitRevisionDescription)
    get_git_head_revision(GIT_REFSPEC GIT_SHA1)
    string(SUBSTRING "${GIT_SHA1}" 0 7 GIT_SHA1_TRUNC)
    set(CMAKE_PROJECT_VERSION "${CMAKE_PROJECT_VERSION}-${GIT_SHA1_TRUNC}")
endif()

add_subdirectory("adminskit")

if(WIN32)
    configure_file("version.rc.in" "${CMAKE_CURRENT_SOURCE_DIR}/version.rc" @ONLY)
    target_sources(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/version.rc")
endif()

string(TOUPPER "${PROJECT_NAME}" PROJECT_EXPORTS)
string(APPEND PROJECT_EXPORTS "_EXPORTS")

target_compile_definitions(${PROJECT_NAME} PRIVATE
    # Build type Debug
    $<$<CONFIG:Debug>:
        DEBUG _DEBUG
    >

    # Build type Release, MinSizeRel, RelWithDebInfo
    $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>,$<CONFIG:RelWithDebInfo>>:
        NDEBUG RELEASE _RELEASE
    >

    # Platform Windows
    $<$<PLATFORM_ID:Windows>:
        WIN32 _WINDOWS _USRDLL _WINDLL _MBCS
    >

    # Platform Linux, Darwin
    $<$<OR:$<PLATFORM_ID:Linux>,$<PLATFORM_ID:Darwin>>:
        LINUX _LINUX linux

        # Build type Debug
        $<$<CONFIG:Debug>:
            _GLIBCXX_ASSERTIONS=1 _GLIBCXX_DEBUG=1 _GLIBCXX_DEBUG_PEDANTIC=1 _FORTIFY_SOURCE=2
        >
    >
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    # C
    C_STANDARD 17
    C_STANDARD_REQUIRED ON
    C_VISIBILITY_PRESET hidden

    # CPP
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_VISIBILITY_PRESET hidden

    # Common
    OPTIMIZE_DEPENDENCIES ON
    VISIBILITY_INLINES_HIDDEN ON
    POSITION_INDEPENDENT_CODE ON

    # Visual Studio
    VS_GLOBAL_ROOTNAMESPACE "${PROJECT_NAME}"
    VS_JUST_MY_CODE_DEBUGGING "$<IF:$<CONFIG:Debug>,ON,OFF>"
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"

    # IPO
    INTERPROCEDURAL_OPTIMIZATION ON
    INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF
    INTERPROCEDURAL_OPTIMIZATION_RELEASE ON
    INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL ON
    INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON

    # Miscellaneous
    PREFIX ""
    DEFINE_SYMBOL ${PROJECT_EXPORTS}
)

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX "${BINARY_NAME_SUFFIX_DARWIN}")
elseif(UNIX)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX "${BINARY_NAME_SUFFIX_LINUX}")
elseif(WIN32)
    get_filename_component(NAME_SUFFIX_WLE "${BINARY_NAME_SUFFIX_WINDOWS}" NAME_WLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        SUFFIX "${BINARY_NAME_SUFFIX_WINDOWS}"
        PDB_NAME "${PROJECT_NAME}${NAME_SUFFIX_WLE}"
        IMPORT_SUFFIX "${NAME_SUFFIX_WLE}.lib"
    )
endif()

if(WIN32 AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # ClangCL.cmake
    include(ClangCL)
else()
    # MSVC.cmake GNU.cmake, Clang.cmake, Intel.cmake
    include(${CMAKE_CXX_COMPILER_ID})
endif()
