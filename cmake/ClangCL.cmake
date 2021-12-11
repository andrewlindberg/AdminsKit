#-------------------------------------------------------------------------------------------
# Clang-CL compiler options
#-------------------------------------------------------------------------------------------

target_compile_options(${PROJECT_NAME} PRIVATE
    # Common
    /W4                   # Warning level
    /std:c17              # Specify C language standard version
    #/arch:SSE2           # Minimum CPU architecture
    /fp:precise           # Specify floating-point behavior
    /diagnostics:column   # Compiler diagnostic options
    /Gy                   # Enable function-level linking
    /GF                   # Eliminate duplicate strings
    /Gd                   # Calling convention
    /nologo               # Suppress startup banner
    /Zc:alignedNew        # Enable C++17 aligned allocation functions
    /Zc:sizedDealloc      # Enable global sized deallocation functions
    /Zc:threadSafeInit-   # Thread-safe initialization of static variables
    /Zc:dllexportInlines- # dllexport/dllimport inline member functions of dllexport/import classes
    /Qvec                 # Enable the loop vectorization passes
    -fcf-protection=none  # Instrument control-flow architecture protection

    # Build type Debug
    $<$<CONFIG:Debug>:
        /Od               # Optimize code
        /Ob0              # Inline function expansion
        /GS               # Buffer security check
        /Zi               # Debug information format
        /Oy-              # Frame-pointer omission
        /WX-              # Treat linker warnings as errors
        /Gw-              # Optimize global data
    >

    # Build type Release, MinSizeRel, RelWithDebInfo
    $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>,$<CONFIG:RelWithDebInfo>>:
        /WX               # Treat linker warnings as errors
        /Oy               # Frame-pointer omission
        /Oi               # Generate intrinsic functions
        /Ot               # Favor fast code
        /GL               # Whole program optimization
        /GS-              # Buffer security check
        /fp:except-       # Enable floating-point exceptions
        /Gw               # Optimize global data
    >

    # Build type Release
    $<$<CONFIG:Release>:
        /O2               # Optimize code
        /Ob2              # Inline function expansion
    >

    # Build type MinSizeRel
    $<$<CONFIG:MinSizeRel>:
        /O1               # Optimize code
        /Ob1              # Inline function expansion
    >

    # Build type RelWithDebInfo
    $<$<CONFIG:RelWithDebInfo>:
        /O2               # Optimize code
        /Ob1              # Inline function expansion
        /Zi               # Debug information format
    >

    # Additional flags
    -march=x86-64 -mmmx -msse -msse2 -msse3 -mssse3

    # Diagnostic flags
    -Wextra -Wpedantic -Weffc++
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
    -Wno-error
    -Wno-sign-conversion
    -Wno-non-virtual-dtor
)

# RTTI
if(ENABLE_RTTI)
    target_compile_options(${PROJECT_NAME} PRIVATE /GR)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE /GR-)
endif()

# Exceptions
if(ENABLE_EXCEPTIONS)
    target_compile_options(${PROJECT_NAME} PRIVATE /EHsc)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE _HAS_EXCEPTIONS=0)
endif()

# LTO
set(ENABLE_LTO OFF)

get_target_property(LTO_DEBUG ${PROJECT_NAME} INTERPROCEDURAL_OPTIMIZATION_DEBUG)
get_target_property(LTO_RELEASE ${PROJECT_NAME} INTERPROCEDURAL_OPTIMIZATION_RELEASE)
get_target_property(LTO_MINSIZEREL ${PROJECT_NAME} INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL)
get_target_property(LTO_RELWITHDEBINFO ${PROJECT_NAME} INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" AND LTO_DEBUG)
    set(ENABLE_LTO ON)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release" AND LTO_RELEASE)
    set(ENABLE_LTO ON)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel" AND LTO_MINSIZEREL)
    set(ENABLE_LTO ON)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" AND LTO_RELWITHDEBINFO)
    set(ENABLE_LTO ON)
endif()

if(ENABLE_LTO)
    target_compile_options(${PROJECT_NAME} PRIVATE
        -flto
        -fwhole-program-vtables
        -fvirtual-function-elimination
    )
endif()

#-------------------------------------------------------------------------------------------
# Link options
#-------------------------------------------------------------------------------------------

# Standard dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE odbc32.lib odbccp32.lib)

# Linker flags
target_link_options(${PROJECT_NAME} PRIVATE
    # Common
    /MACHINE:X86          # Specify target platform
    /SUBSYSTEM:CONSOLE    # Specify subsystem
    /LARGEADDRESSAWARE:NO # Handle large addresses
    /MANIFESTUAC:NO       # Embeds UAC information in manifest
    /MANIFEST             # Create side-by-side assembly manifest
    /NXCOMPAT             # Compatible with data execution prevention
    /NOLOGO               # Suppress startup banner
    /WX                   # Treat linker warnings as errors

    # Build type Debug
    $<$<CONFIG:Debug>:
        /DEBUG:FULL       # Generate debug info
        /INCREMENTAL      # Enable incremental linking
    >

    # Build type Release, MinSizeRel, RelWithDebInfo
    $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>,$<CONFIG:RelWithDebInfo>>:
        /OPT:REF          # Eliminates functions and data that are never referenced
        /OPT:ICF          # Enable COMDAT folding
        /LTCG             # Link-time code generation
        /INCREMENTAL:NO   # Enable incremental linking
    >

    # Build type Release, MinSizeRel
    $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>:
        /DEBUG:NONE       # Generate debug info
    >

    # Build type RelWithDebInfo
    $<$<CONFIG:RelWithDebInfo>:
        /DEBUG:FASTLINK   # Generate debug info
    >
)

#-------------------------------------------------------------------------------------------
# Visual Studio
#-------------------------------------------------------------------------------------------

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_GLOBAL_UseDebugLibraries true
        VS_GLOBAL_LinkIncremental true
    )
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_GLOBAL_UseDebugLibraries false
        VS_GLOBAL_LinkIncremental false
    )
endif()

if(RUN_CLANG_TIDY)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_GLOBAL_RunCodeAnalysis true
        VS_GLOBAL_EnableClangTidyCodeAnalysis true
    )
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_GLOBAL_EnableClangTidyCodeAnalysis false
    )
endif()
