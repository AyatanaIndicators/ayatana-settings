cmake_minimum_required (VERSION 3.13)
project (ayatana-settings VERSION 24.10.1 LANGUAGES C)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    SET (CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "..." FORCE)
endif (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# Options

option (ENABLE_WERROR "Treat all build warnings as errors" OFF)

set(CMAKE_BUILD_TYPE "Release")

if (ENABLE_WERROR)
    add_definitions ("-Werror")
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_definitions ("-Weverything")
else ()
    add_definitions ("-Wall")
endif ()

# Prerequisites

include (GNUInstallDirs)
find_package (PkgConfig REQUIRED)
include (FindPkgConfig)
pkg_check_modules (DEPS REQUIRED gtk4>=4.14 gmodule-export-2.0)

# Build

add_subdirectory (src)
add_subdirectory (data)
add_subdirectory (po)

# Info

message (STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message (STATUS "Build with -Werror: ${ENABLE_WERROR}")
