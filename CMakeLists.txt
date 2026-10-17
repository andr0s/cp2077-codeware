cmake_minimum_required(VERSION 3.20)

# macOS (arm64) build of Codeware for Cyberpunk 2077 2.3.1 and the jackmaxwil RED4ext macOS loader.
# The Windows build stays on xmake (xmake.lua); this file is for macOS only. See macos/PORT_NOTES.md.

if(NOT APPLE)
    message(FATAL_ERROR "This CMake build is for macOS. Use xmake (xmake.lua) on Windows.")
endif()

# Release by default: the loader refuses a plugin whose binary contains an unverified address hash, and an unoptimized
# build keeps code paths (base-class vtables of the SDK types) that the optimizer drops.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "" FORCE)
# The game needs a recent macOS; 14.0 is the oldest target whose libc++ has every C++20 library feature Codeware uses.
set(CMAKE_OSX_DEPLOYMENT_TARGET "14.0" CACHE STRING "Minimum macOS version")

project(Codeware VERSION 1.20.5 LANGUAGES C CXX OBJCXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

message(STATUS "Building Codeware ${PROJECT_VERSION} for macOS")

add_compile_definitions(RED4EXT_PLATFORM_MACOS)

# The plugin resolves the RED4ext.SDK symbols it uses itself (header-only SDK); RED4ext provides only
# RED4ext_ResolveAddress, which the SDK looks up at runtime.
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup")

# Generate src/App/Project.hpp from config/Project.hpp.in (the same template xmake uses: ${NAME} etc.).
set(NAME "Codeware")
set(AUTHOR "psiberx")
set(VERSION "${PROJECT_VERSION}")
set(MODE "release")
set(DEBUG "")
configure_file("${CMAKE_SOURCE_DIR}/config/Project.hpp.in" "${CMAKE_SOURCE_DIR}/src/App/Project.hpp")

# Header-only vendor dependencies
add_library(nameof INTERFACE)
target_include_directories(nameof INTERFACE "${CMAKE_SOURCE_DIR}/vendor/nameof/include")

add_library(semver INTERFACE)
target_include_directories(semver INTERFACE "${CMAKE_SOURCE_DIR}/vendor/semver/include")

# RED4ext.SDK headers. RED4EXT_SDK_DIR is the SDK repository root (jackmaxwil/RED4ext.SDK-macos); it defaults to the
# vendor/RED4ext.SDK submodule. Pass -DRED4EXT_SDK_DIR=<path> to use another checkout.
set(RED4EXT_SDK_DIR "${CMAKE_SOURCE_DIR}/vendor/RED4ext.SDK" CACHE PATH "RED4ext.SDK repository root")
if(NOT EXISTS "${RED4EXT_SDK_DIR}/include/RED4ext/RED4ext.hpp")
    message(FATAL_ERROR "RED4ext.SDK not found at ${RED4EXT_SDK_DIR}. "
                        "Pass -DRED4EXT_SDK_DIR=<path to jackmaxwil/RED4ext.SDK-macos>.")
endif()
if(NOT EXISTS "${RED4EXT_SDK_DIR}/cyberpunk2077_addresses.json")
    message(FATAL_ERROR "${RED4EXT_SDK_DIR} is not the macOS RED4ext.SDK (no cyberpunk2077_addresses.json).")
endif()
message(STATUS "RED4ext.SDK: ${RED4EXT_SDK_DIR}")
add_library(RED4ext.SDK INTERFACE)
target_include_directories(RED4ext.SDK INTERFACE "${RED4EXT_SDK_DIR}/include")

# clip (clipboard), macOS backend only, text support only (as in the Windows build)
add_library(clip STATIC
    vendor/clip/clip.cpp
    vendor/clip/clip_osx.mm
)
target_include_directories(clip PUBLIC "${CMAKE_SOURCE_DIR}/vendor/clip")
target_compile_options(clip PRIVATE -fobjc-arc -fvisibility=hidden)
target_link_libraries(clip PRIVATE "-framework Cocoa")

# spdlog (with its bundled fmt), built from a pinned source as a static library, so the plugin needs nothing from
# Homebrew on the player's Mac. Same version and hash as the TweakXL and ArchiveXL macOS ports.
include(FetchContent)
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(spdlog
    URL https://github.com/gabime/spdlog/archive/refs/tags/v1.16.0.tar.gz
    URL_HASH SHA256=8741753e488a78dd0d0024c980e1fb5b5c85888447e309d9cb9d949bdb52aa3e)
FetchContent_MakeAvailable(spdlog)

file(GLOB_RECURSE CODEWARE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/lib/*.cpp"
)
# MinHook is Windows-only; hooks go through the RED4ext loader on macOS.
list(FILTER CODEWARE_SOURCES EXCLUDE REGEX ".*/lib/Support/MinHook/.*")

add_library(Codeware SHARED ${CODEWARE_SOURCES})

set_target_properties(Codeware PROPERTIES
    OUTPUT_NAME "Codeware"
    PREFIX ""
    SUFFIX ".dylib"
)

target_include_directories(Codeware PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/lib"
)

target_link_libraries(Codeware PRIVATE
    RED4ext.SDK
    nameof
    semver
    clip
    spdlog::spdlog
    "-framework Cocoa"
    "-framework CoreGraphics"
)

target_precompile_headers(Codeware PRIVATE src/pch.hpp)

target_compile_options(Codeware PRIVATE
    -fno-rtti
    -fexceptions
    -Wno-deprecated-declarations
    -fvisibility=hidden
    -fvisibility-inlines-hidden
)

# Install layout, relative to the game folder: cmake --install build --prefix "<Cyberpunk 2077 folder>"
#   red4ext/plugins/Codeware/Codeware.dylib, Scripts/ (redscript sources), Data/KnownHashes.txt, licenses
set(CODEWARE_PLUGIN_DIR "red4ext/plugins/Codeware")
install(TARGETS Codeware LIBRARY DESTINATION "${CODEWARE_PLUGIN_DIR}")
# These imports declare Windows-only game types that the macOS game does not have (input Win32/editor systems, Windows
# game services, FSR 3.0 data, Jira/OAuth tools, a removed menu state). The script validator would stop script loading
# with "Missing native class". No other script refers to them.
install(DIRECTORY scripts/ DESTINATION "${CODEWARE_PLUGIN_DIR}/Scripts"
    FILES_MATCHING PATTERN "*.reds"
    PATTERN "FSR30CustomData.reds" EXCLUDE
    PATTERN "gsmMenuState_LoadSession.reds" EXCLUDE
    PATTERN "inputInputSystemEditor.reds" EXCLUDE
    PATTERN "inputInputSystemWin32Base.reds" EXCLUDE
    PATTERN "inputInputSystemWin32Game.reds" EXCLUDE
    PATTERN "oauthAuthenticationToken.reds" EXCLUDE
    PATTERN "servicesGameServicesWin.reds" EXCLUDE
    PATTERN "toolsJiraService.reds" EXCLUDE)
install(DIRECTORY data/ DESTINATION "${CODEWARE_PLUGIN_DIR}/Data")
install(FILES LICENSE THIRD_PARTY_LICENSES DESTINATION "${CODEWARE_PLUGIN_DIR}")

message(STATUS "Codeware ${PROJECT_VERSION} configured")
