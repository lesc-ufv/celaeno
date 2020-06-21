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

#
# External libraries
#
if(NOT maia_FOUND)
  find_package(maia REQUIRED)
endif()
if(NOT taygete_FOUND)
  find_package(taygete REQUIRED)
endif()
if(NOT asterope_FOUND)
  find_package(asterope REQUIRED)
endif()
if(NOT Catch2_FOUND)
  find_package(Catch2 REQUIRED)
endif()
if(NOT Threads_FOUND)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
endif()

#
# Test generator
#
function(add_test target)
  add_executable(${target} ${ARGN})
  target_compile_options(${target}
    PRIVATE
      -std=c++2a
      -Wall
      -Wextra
      -Wpedantic
      -O0
      -Wshadow
      -fconcepts-diagnostics-depth=2
      -Weffc++
    )
  target_link_libraries(${target}
    PRIVATE
      celaeno
      spdlog
      Threads::Threads
      taygete::taygete
      maia::maia
  )
endfunction(add_test)

#
# Tests
#
add_test(test_bfs "include/celaeno/graph/bfs.cpp")
add_test(test_dfs "include/celaeno/graph/dfs.cpp")
add_test(test_balance "include/celaeno/graph/balance.cpp")
add_test(test_kahn "include/celaeno/graph/kahn.cpp")
add_test(test_a_star "include/celaeno/graph/a-star.cpp")
add_test(test_m_real "include/celaeno/graph/matrix-realization.cpp")
add_test(test_barycenter "include/celaeno/graph/barycenter.cpp")
# add_test(test_views_depth "include/celaeno/graph/views/depth.cpp")
