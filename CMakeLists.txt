# vim: set ts=2 sw=2 tw=0 et :
######################################################################
# @company     : Universidade Federal de Viçosa - Florestal
# @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
# @file        : CMakeLists
# @created     : Wednesday Aug 14, 2019 14:24:21 -03
# @license     : MIT
# @description : C++ Algorithms Collection
######################################################################

cmake_minimum_required(VERSION 3.1..3.15)

project(
  celaeno
  DESCRIPTION "C++ Algorithms Collection"
  LANGUAGES CXX
  )


# Modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(colors)
define_colors()

# Source files
add_subdirectory("include")

# External Libraries
add_subdirectory("extern")

# Tests
if( ${PROJECT_NAME}_tests )
add_subdirectory("test")
endif()
