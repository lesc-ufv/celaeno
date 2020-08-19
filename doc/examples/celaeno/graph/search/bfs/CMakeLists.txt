######################################################################
# @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
# @file        : CMakeLists
# @created     : sunday jul 26, 2020 13:56:50 -03
######################################################################

cmake_minimum_required(VERSION 3.1..3.16)

project(examples
  VERSION 0.1
  DESCRIPTION "Celaeno - Examples"
  LANGUAGES CXX
)

# Conan
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})

# Find celaeno
if(NOT celaeno_FOUND)
  find_package(celaeno REQUIRED)
endif()

add_executable(main main.cpp)
target_link_libraries(main PRIVATE celaeno::celaeno fmt::fmt)
target_compile_options(main PRIVATE -std=c++2a -Wall -Wextra -Weffc++)
