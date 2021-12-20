#-------------------------------------------------------------------------------------------
# MSVC compiler options
#-------------------------------------------------------------------------------------------

# Disable the desired warning numbers; put numbers in a semi-colon delimited list
set(WARNINGS_DISABLE 4706;)

target_compile_options(${PROJECT_NAME} PRIVATE
    # Common
    /W4                 # Warning level
    #/std:c17           # Specify C language standard version
    /arch:SSE2          # Minimum CPU architecture
    /fp:precise         # Specify floating-point behavior
    /diagnostics:column # Compiler diagnostic options
    /permissive-        # Standards conformance
    /Gy                 # Enable function-level linking
    /GF                 # Eliminate duplicate strings
    /Gd                 # Calling convention
    /MP${NCORES}        # Build with multiple processes
    /Gm-                # Minimal rebuild
    /nologo             # Suppress startup banner
    /Zc:wchar_t         # Parse wchar_t as a built-in type according to the C++ standard
    /Zc:rvalueCast      # Enforce type conversion rules
    /Zc:inline          # Remove unreferenced COMDAT
    /Zc:forScope        # Force conformance in for loop scope
    /Zc:threadSafeInit- # Thread-safe initialization of static variables

    # Build type Debug
    $<$<CONFIG:Debug>:
        /WX-            # Treat linker warnings as errors
        /Od             # Optimize code
        /Oy-            # Frame-pointer omission
        /Ob0            # Inline function expansion
        /RTC1           # Run-time error checks
        /GS             # Buffer security check
        /sdl            # Additional security checks
        /Zi             # Debug information format
    >

    # Build type Release, MinSizeRel, RelWithDebInfo
    $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>,$<CONFIG:RelWithDebInfo>>:
        /WX             # Treat linker warnings as errors
        /Oy             # Frame-pointer omission
        /Oi             # Generate intrinsic functions
        /Ot             # Favor fast code
        /GL             # Whole program optimization
        /GS-            # Buffer security check
        /sdl-           # Additional security checks
        /guard:cf-      # Control flow guard
        /fp:except-     # Enable floating-point exceptions
        /Gw             # Optimize global data
    >

    # Build type Release
    $<$<CONFIG:Release>:
        /O2             # Optimize code
        /Ob2            # Inline function expansion
    >

    # Build type MinSizeRel
    $<$<CONFIG:MinSizeRel>:
        /O1             # Optimize code
        /Ob1            # Inline function expansion
    >

    # Build type RelWithDebInfo
    $<$<CONFIG:RelWithDebInfo>:
        /O2             # Optimize code
        /Ob1            # Inline function expansion
        /Zi             # Debug information format
    >
)

# Warnings
foreach(WARNING_NUM ${WARNINGS_DISABLE})
    target_compile_options(${PROJECT_NAME} PRIVATE /wd${WARNING_NUM})
endforeach()

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

# Auto-parallelizer
if(MSVC_AUTO_PARALLELIZER)
    target_compile_options(${PROJECT_NAME} PRIVATE /Qpar)
endif()

# Code analysis
if(MSVC_CODE_ANALYSIS)
    target_compile_options(${PROJECT_NAME} PRIVATE /analyze)
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

if(MSVC_CODE_ANALYSIS)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_GLOBAL_RunCodeAnalysis true
        VS_GLOBAL_EnableMicrosoftCodeAnalysis true
    )
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_GLOBAL_EnableMicrosoftCodeAnalysis false
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
