# vim: set ts=2 sw=2 tw=0 et :
######################################################################
# @company     : Universidade Federal de Viçosa - Florestal
# @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
# @file        : CMakeLists
# @created     : Wednesday Aug 14, 2019 14:24:21 -03
# @license     : MIT
# @description : C++ Algorithms Collection
######################################################################

cmake_minimum_required(VERSION 3.7..3.15)
cmake_policy(VERSION 3.7..3.15)

project(
  celaeno
  DESCRIPTION "C++ Algorithms Collection"
  LANGUAGES CXX
)

#
# Compiler Configuration
#
message(STATUS "CXX compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "CXX compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CXX flags Debug: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CXX flags Release: ${CMAKE_CXX_FLAGS_RELEASE}")

#
# CMake Modules
#
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(colors)
define_colors()

#
# Build Type
#
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build Type" FORCE)
endif()

message(
  STATUS
    "${C_MAGENTA}Build Type:${C_RESET} "
    "${C_BLUE}${CMAKE_BUILD_TYPE}${C_RESET}"
)

#
# Source files
#
add_subdirectory("include")

#
# External Libraries
#
add_subdirectory("extern")

#
# Tests
#
if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
  add_subdirectory("test")
else()
  message(STATUS "${C_MAGENTA} * ${C_RESET}Tests disabled")
endif()

#
# Install
#
include(GNUInstallDirs)

# Create celaeno-config.cmake
set(cmakeModulesDir cmake)
configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/celaeno-config.cmake.in"
  "${PROJECT_BINARY_DIR}/celaeno-config.cmake"
  INSTALL_DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/celaeno
)

# Export targets
install(
  TARGETS celaeno range-v3 range-v3-meta range-v3-concepts
  EXPORT celaeno-targets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install prefix
set(CMAKE_INSTALL_PREFIX "/usr/local/")
message("${C_BLUE} * ${C_RESET}Install prefix: ${CMAKE_INSTALL_PREFIX}")

# Install exported targets
install(EXPORT celaeno-targets
  FILE celaeno-targets.cmake
  NAMESPACE celaeno::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/celaeno
)

# Install celaeno-config.cmake
install(
  FILES
    "${PROJECT_BINARY_DIR}/celaeno-config.cmake"
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/celaeno
)

# Install headers
install(
  DIRECTORY
    ${PROJECT_SOURCE_DIR}/include/celaeno
  DESTINATION
    ${CMAKE_INSTALL_PREFIX}/include
)
