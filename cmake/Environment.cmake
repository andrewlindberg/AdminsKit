#-------------------------------------------------------------------------------------------
# CMake environment configuration
#-------------------------------------------------------------------------------------------

include(ProcessorCount)
ProcessorCount(NCORES)

if(APPLE)
    set(CMAKE_POLICY_DEFAULT_CMP0025 NEW)
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}-${CMAKE_BUILD_TYPE}")

if(NOT CMAKE_PDB_OUTPUT_DIRECTORY)
    set(CMAKE_PDB_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
endif()

if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
endif()

if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
endif()

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
endif()

foreach(config_type ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER "${config_type}" config_type)

    if(NOT CMAKE_PDB_OUTPUT_DIRECTORY_${config_type})
        set(CMAKE_PDB_OUTPUT_DIRECTORY_${config_type} "${OUTPUT_DIR}")
    endif()

    if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${config_type})
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${config_type} "${OUTPUT_DIR}")
    endif()

    if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config_type})
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config_type} "${OUTPUT_DIR}")
    endif()

    if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config_type})
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config_type} "${OUTPUT_DIR}")
    endif()
endforeach()

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_OPTIMIZE_DEPENDENCIES ON)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)

if(MSVC)
    if(NOT ENABLE_EXCEPTIONS)
        string(REPLACE "/EHsc" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
        string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endif()

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /external:W0")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /external:W0")
    endif()
endif()

if(NOT WIN32)
    set(CMAKE_C_FLAGS "-m32 -march=x86-64 -mtune=generic -mmmx -msse -msse2 -msse3 -mssse3 -mfpmath=sse")
    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")

    set(CMAKE_C_FLAGS_RELEASE "-O3 -g0 -fno-stack-protector")
    set(CMAKE_C_FLAGS_MINSIZEREL "-Os -g0 -fno-stack-protector")

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
        set(CMAKE_C_FLAGS_DEBUG "-O -g")
        set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")
    else()
        set(CMAKE_C_FLAGS_DEBUG "-Og -ggdb")
        set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -ggdb")
    endif()

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL}")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")

    set(CMAKE_EXE_LINKER_FLAGS "-m32")
    set(CMAKE_SHARED_LINKER_FLAGS "-m32")
endif()
