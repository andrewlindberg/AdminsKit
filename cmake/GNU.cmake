#-------------------------------------------------------------------------------------------
# GCC compiler options
#-------------------------------------------------------------------------------------------

# Diagnostic flags
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -Wpedantic
    -Wcast-align
    -Wlogical-op
    -Wnull-dereference
    -Wredundant-decls
    -Wdouble-promotion

    $<$<COMPILE_LANGUAGE:CXX>:
        -Weffc++
        -Wuseless-cast
        -Wold-style-cast
        -Wcast-qual
        -Wctor-dtor-privacy
        -Woverloaded-virtual
        -Wsign-promo
        -Wduplicated-branches
        -Wduplicated-cond
        -Wfloat-equal
        -Wconversion
        -Wshadow=compatible-local
        -Wzero-as-null-pointer-constant
        -Wextra-semi

        # Suppress warnings
        -Wno-non-virtual-dtor
    >
)

# Compiler flags
target_compile_options(${PROJECT_NAME} PRIVATE
    -pipe -m32 -march=x86-64 -mtune=generic -mmmx -msse -msse2 -msse3 -mssse3 -mfpmath=sse

    # Build type Debug
    $<$<CONFIG:Debug>:
        -Og -ggdb
    >

    # Build type Release
    $<$<CONFIG:Release>:
        -O2
    >

    # Build type MinSizeRel
    $<$<CONFIG:MinSizeRel>:
        -Os
    >

    # Build type RelWithDebInfo
    $<$<CONFIG:RelWithDebInfo>:
        -O2 -ggdb
    >

    # Build type Release, MinSizeRel
    $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>:
        -g0 -fno-stack-protector
    >

    -fno-threadsafe-statics -ffunction-sections -fdata-sections
)

# Optional flags
if(GCC_ANALYZER)
    target_compile_options(${PROJECT_NAME} PRIVATE -fanalyzer -Wno-analyzer-possible-null-dereference)
endif()

if(NOT ENABLE_RTTI)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)
endif()

if(NOT ENABLE_EXCEPTIONS)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>)
endif()

#-------------------------------------------------------------------------------------------
# Link options
#-------------------------------------------------------------------------------------------

# Linker flags
if(LINKER_GOLD)
    target_link_options(${PROJECT_NAME} PRIVATE -fuse-ld=gold -m32
        # Warnings
        -Wl,--warn-common
        -Wl,--warn-execstack
        -Wl,--warn-drop-version
        -Wl,--warn-shared-textrel
        -Wl,--warn-search-mismatch
        -Wl,--detect-odr-violations
        -Wl,--unresolved-symbols=report-all

        # Common
        -Wl,-O3
        -Wl,--relax
        -Wl,--as-needed
        -Wl,--gc-sections
        -Wl,--icf=safe
        -Wl,--icf-iterations=5
        -Wl,--no-undefined
        -Wl,--no-incremental
        -Wl,--no-whole-archive

        # Build type RelWithDebInfo
        $<$<CONFIG:RelWithDebInfo>:
            -Wl,--discard-all
            -Wl,--compress-debug-sections=zlib
        >

        # Build type Release, MinSizeRel
        $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>:
            -Wl,--no-ld-generated-unwind-info
            -Wl,--discard-all
            -Wl,--strip-all
        >

        # Print information
        #-Wl,--stats
        #-Wl,--print-gc-sections
        #-Wl,--print-icf-sections
    )
else()
    target_link_options(${PROJECT_NAME} PRIVATE -m32
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
endif()

# Libraries linking
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
