# vim: set ts=2 sw=2 tw=0 et :

# @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
# @file        : CMakeLists
# @created     : Wednesday Aug 14, 2019 15:30:53 -03

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

# External libraries
if(NOT Threads_FOUND)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
endif()

# Options
option(DEBUG_SVG_HPP "Enables logging for svg.hpp" OFF)
option(DEBUG_DISABLE_OUTPUT "Disables file output" OFF)
option(DEBUG_SHOW_ALG "Displays executed algorithms orderly" OFF)
option(DEBUG_SHOW_HDL "Displays read HDL expressions" OFF)
option(OPT_SEQ "Single threaded mode" OFF)

# Test generator
function(add_applet target)
  add_executable(${target} ${ARGN})

  target_compile_options(${target}
    PRIVATE
      -std=c++23
      $<IF:$<CONFIG:Debug>,
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Weffc++
        ,
        -DNDEBUG
        -O3
      >
  )

  target_compile_definitions(${target}
    PRIVATE
      $<$<BOOL:${DEBUG_SVG_HPP}>:DEBUG_SVG_HPP>
      $<$<BOOL:${DEBUG_DISABLE_OUTPUT}>:DEBUG_DISABLE_OUTPUT>
      $<$<BOOL:${DEBUG_SHOW_ALG}>:DEBUG_SHOW_ALG>
      $<$<BOOL:${DEBUG_SHOW_HDL}>:DEBUG_SHOW_HDL>
      $<$<BOOL:${OPT_SEQ}>:OPT_SEQ>
  )

  target_link_options(${target} PRIVATE -static)

  target_link_libraries(${target}
    PRIVATE
      celaeno
      argparse::argparse
      fmt::fmt
      spdlog::spdlog
      doctest::doctest
      FunctionalPlus::fplus
      Threads::Threads
      cppcoro::cppcoro
      magic_enum::magic_enum
  )

  target_include_directories(${target}
    PRIVATE
      $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/test/include>
  )

endfunction(add_applet)

# celaeno placement and routing
add_applet(mapper "mapper.cpp")
