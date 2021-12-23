#-------------------------------------------------------------------------------------------
# Intel LLVM compiler options
#-------------------------------------------------------------------------------------------

# Diagnostic flags
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -Wpedantic -Wdeprecated
    -Wcast-align
    -Wnull-dereference
    -Wredundant-decls
    -Wdouble-promotion
    -Wold-style-cast
    -Wcast-qual
    -Wctor-dtor-privacy
    -Woverloaded-virtual
    -Wsign-promo
    -Wfloat-equal
    -Wconversion
    -Wzero-as-null-pointer-constant
    -Wextra-semi

    # Suppress warnings
    -Wno-sign-conversion
    -Wno-non-virtual-dtor
)

# Compiler flags
target_compile_options(${PROJECT_NAME} PRIVATE
    -pipe $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics> -ffunction-sections -fdata-sections -pedantic
)

# Optional flags
if(CLANG_ANALYZER)
    target_compile_options(${PROJECT_NAME} PRIVATE --analyze --analyzer-output text)
endif()

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
target_link_options(${PROJECT_NAME} PRIVATE
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
if(NO_INTEL_LIB)
    target_link_options(${PROJECT_NAME} PRIVATE -no-intel-lib)
endif()

if(LINK_LIB_INTEL AND NOT NO_INTEL_LIB)
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
