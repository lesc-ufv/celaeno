# vim: set ts=2 sw=2 tw=0 et :

# @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
# @file        : CMakeLists
# @created     : Wednesday Aug 14, 2019 14:24:21 -03

# BSD 2-Clause License

# Copyright (c) 2020, Ruan Evangelista Formigoni
# All rights reserved.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:

# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.

# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 3.7..3.15)
cmake_policy(VERSION 3.7..3.15)

project(
  celaeno
  DESCRIPTION "C++ Algorithms Collection"
  LANGUAGES CXX
)

# Conan Package Manager
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

#
# Compiler Configuration
#
message(STATUS "CXX compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "CXX compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CXX flags Debug: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CXX flags Release: ${CMAKE_CXX_FLAGS_RELEASE}")

#
# External Projects
#
find_package(taygete)
find_package(asterope)

#
# Source files
#
add_subdirectory("include")

#
# Tests
#
add_subdirectory("test")

#
# Install
#
include(GNUInstallDirs)

# Install prefix
set(CMAKE_INSTALL_PREFIX "/usr/local/")
message("${C_BLUE} * ${C_RESET}Install prefix: ${CMAKE_INSTALL_PREFIX}")

# Create celaeno-config.cmake
include(CMakePackageConfigHelpers)
set(cmakeModulesDir cmake)
configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/celaeno-config.cmake.in"
  "${PROJECT_BINARY_DIR}/celaeno-config.cmake"
  INSTALL_DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/celaeno
)

# Export targets
install(
  TARGETS celaeno
  EXPORT celaeno-targets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

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

# Packaging
set(CPACK_PACKAGE_NAME                celaeno)
set(CPACK_PACKAGE_VENDOR              "Ruan")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++ Data Visualization Tool")
set(CPACK_PACKAGE_INSTALL_DIRECTORY   ${CPACK_PACKAGE_NAME})
set(CPACK_GENERATOR                   "DEB"  )
set(CPACK_DEBIAN_PACKAGE_MAINTAINER   "Ruan")
include(CPack)
