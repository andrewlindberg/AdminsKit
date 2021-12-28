include(PVS-Studio)

function(DisableCodeAnalysis _target)
    set_target_properties("${_target}" PROPERTIES
        C_CPPCHECK ""
        CXX_CPPCHECK ""
        C_CLANG_TIDY ""
        CXX_CLANG_TIDY ""
        VS_GLOBAL_RunCodeAnalysis false
        VS_GLOBAL_EnableMicrosoftCodeAnalysis false
        VS_GLOBAL_EnableClangTidyCodeAnalysis false
    )
endfunction()

#-------------------------------------------------------------------------------------------
# Clang-Tidy
#-------------------------------------------------------------------------------------------

# Run clang-tidy
if(RUN_CLANG_TIDY)
    find_program(CLANG_TIDY_PROGRAM NAMES clang-tidy)

    if(CLANG_TIDY_PROGRAM)
        if(MSVC AND ENABLE_EXCEPTIONS)
            set(CLANG_TIDY_PROGRAM "${CLANG_TIDY_PROGRAM};--extra-arg=/EHsc")
        endif()

        set(CMAKE_C_CLANG_TIDY ${CLANG_TIDY_PROGRAM})
        set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_PROGRAM})
    else()
        message(WARNING "The RUN_CLANG_TIDY option is ON but clang-tidy program is not found.")
    endif()
endif()

#-------------------------------------------------------------------------------------------
# Cppcheck
#-------------------------------------------------------------------------------------------

# Run cppcheck
if(RUN_CPPCHECK AND NOT WIN32) # TODO: Add windows support
    find_program(CMAKE_C_CPPCHECK NAMES cppcheck)

    if(CMAKE_C_CPPCHECK)
        if(UNIX)
            set(PLATFORM_TYPE "unix32")
        else()
            set(PLATFORM_TYPE "unspecified")
        endif()

        list(APPEND CMAKE_C_CPPCHECK
            "-j ${NCORES}"
            "--quiet"
            "--relative-paths"
            "--platform=${PLATFORM_TYPE}"
            "--language=c++"
            "--std=c++17"
            "--force"
            "--inline-suppr"
            "--max-ctu-depth=32"
            "--enable=warning,style,performance,missingInclude"
            #"--bug-hunting"
            #"--inconclusive"
            "--suppressions-list=${CMAKE_SOURCE_DIR}/CppcheckSuppressions.txt"
        )

        set(CMAKE_CXX_CPPCHECK ${CMAKE_C_CPPCHECK})
    else()
        message(WARNING "The RUN_CPPCHECK option is ON but cppcheck program is not found.")
    endif()
endif()

#-------------------------------------------------------------------------------------------
# PVS-Studio
#-------------------------------------------------------------------------------------------

# Run PVS-Studio
if(RUN_PVS_STUDIO)
    pvs_studio_add_target(
        TARGET pvs.analysis
        ALL
        COMPILE_COMMANDS
        OUTPUT
        HIDE_HELP
        MODE GA:1,2,3 OP:1,2,3
        CONFIG "${CMAKE_SOURCE_DIR}/PVS-Studio.cfg"
        ARGS --threads ${NCORES} --ignore-ccache --exclude-path "${CMAKE_SOURCE_DIR}/libs"
        CONVERTER_ARGS --excludedCodes V1042
    )
endif()
