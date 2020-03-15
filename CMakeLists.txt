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

set(CMAKE_PREFIX_PATH "/prefix")
set(as_subproject range-v3)
macro(find_package)
  if(NOT "${ARGV}" IN_LIST as_subproject)
    _find_package("${ARGV}")
  endif()
endmacro()
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
endif()

#
# Install
#
include(GNUInstallDirs)

# Export targets
install(TARGETS celaeno range-v3 range-v3-concepts range-v3-meta
  EXPORT ${PROJECT_NAME}-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install exported targets
install(EXPORT ${PROJECT_NAME}-targets
  FILE ${PROJECT_NAME}-targets.cmake
  NAMESPACE celaeno::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/celaeno
)

# Create celaeno-config.cmake
set(cmakeModulesDir cmake)
configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  INSTALL_DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  PATH_VARS cmakeModulesDir
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# Install celaeno-config.cmake
install(
  FILES
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)


# Install headers
install(
  DIRECTORY
    ${PROJECT_SOURCE_DIR}/include
  DESTINATION
    ${CMAKE_INSTALL_PREFIX}
)
