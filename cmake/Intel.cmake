#-------------------------------------------------------------------------------------------
# Intel compiler options
#-------------------------------------------------------------------------------------------

# IPO flags
set(CMAKE_C_COMPILE_OPTIONS_IPO ${CMAKE_C_COMPILE_OPTIONS_IPO} -fno-fat-lto-objects)
set(CMAKE_CXX_COMPILE_OPTIONS_IPO ${CMAKE_CXX_COMPILE_OPTIONS_IPO} -fno-fat-lto-objects)

# Diagnostic flags
target_compile_options(${PROJECT_NAME} PRIVATE
    -w3 -Wall -Wremarks -Wcheck -Weffc++
    -Wuninitialized -Wdeprecated -Wpointer-arith

    # Suppress warnings
    -diag-disable=383,869,1011,1418,1419,2012,2013,2015,2021,2304,11074,11076
)

if(INTEL_ALL_DIAGNOSTIC_GROUPS)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:
        -diag-enable=thread,power,port-win,openmp,warn,error,remark,vec,par,cpu-dispatch
    >
)
endif()

# Compiler flags
target_compile_options(${PROJECT_NAME} PRIVATE
    -no-intel-extensions
    -m32 -march=x86-64 -mtune=generic -mmmx -msse -msse2 -msse3 -mssse3 -mfpmath=sse

    # Build type Debug
    $<$<CONFIG:Debug>:
        -O -g
    >

    # Build type Release
    $<$<CONFIG:Release>:
        -O3
    >

    # Build type MinSizeRel
    $<$<CONFIG:MinSizeRel>:
        -Os
    >

    # Build type RelWithDebInfo
    $<$<CONFIG:RelWithDebInfo>:
        -O2 -g
    >

    # Build type Release, MinSizeRel
    $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>:
        -g0 -fno-stack-protector
    >

    -fno-threadsafe-statics -ffunction-sections -fdata-sections
)

# Optional flags
if(NOT ENABLE_RTTI)
    target_compile_options(${PROJECT_NAME} PRIVATE -fno-rtti)
endif()

if(NOT ENABLE_EXCEPTIONS)
    target_compile_options(${PROJECT_NAME} PRIVATE -fno-exceptions)
endif()

#-------------------------------------------------------------------------------------------
# Link options
#-------------------------------------------------------------------------------------------

# Linker flags
target_link_options(${PROJECT_NAME} PRIVATE -m32 -no-intel-extensions
    # Warnings
    -Wl,--warn-common
    -Wl,--warn-alternate-em

    # Common
    -Wl,-O3
    -Wl,--relax
    -Wl,--as-needed
    -Wl,--gc-sections
    -Wl,--no-undefined
    -Wl,--no-allow-shlib-undefined
    -Wl,--check-sections

    # Build type RelWithDebInfo
    $<$<CONFIG:RelWithDebInfo>:
        -Wl,--discard-all
        -Wl,--compress-debug-sections=zlib
    >

    # Build type Release, MinSizeRel
    $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>:
        -Wl,--discard-all
        -Wl,--strip-all
    >
)

# Libraries linking
if(LINK_LIB_INTEL)
    target_link_libraries(${PROJECT_NAME} PRIVATE -static-intel)
endif()

if(LINK_LIB_GCC)
    target_link_libraries(${PROJECT_NAME} PRIVATE -static-libgcc)
endif()

if(LINK_LIB_STDCPP)
    target_link_libraries(${PROJECT_NAME} PRIVATE -static-libstdc++)
endif()

if(LINK_LIB_C)
    target_link_libraries(${PROJECT_NAME} PRIVATE c)
endif()

if(LINK_LIB_M)
    target_link_libraries(${PROJECT_NAME} PRIVATE m)
endif()

if(LINK_LIB_DL)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DL_LIBS})
endif()
