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

# Test generator
function(add_test target)
  add_executable(${target} ${ARGN})
  target_compile_options(${target}
    PRIVATE
      -std=c++2a
      -O3
      $<IF:$<CONFIG:Debug>,
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Weffc++
        ,
        -DNDEBUG
      >
  )
  target_link_libraries(${target}
    PRIVATE
      celaeno
      spdlog::spdlog
      doctest::doctest
      functionalplus::functionalplus
      Threads::Threads
  )
  target_include_directories(${target}
    PRIVATE
      $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/test/include>
  )
endfunction(add_test)

# Tests

# ## Search
# add_test(g_search_bfs "include/celaeno/test/graph/search/bfs.cpp")
# add_test(g_search_dfs "include/celaeno/test/graph/search/dfs.cpp")
# add_test(g_search_kahn "include/celaeno/test/graph/search/kahn.cpp")
#
# ## Views
# add_test(g_views_depth "include/celaeno/test/graph/views/depth.cpp")
# add_test(g_views_proximity "include/celaeno/test/graph/views/proximity.cpp")
#
# ## Operations
# ### Query
# add_test(g_operations_count_crossings "include/celaeno/test/graph/operations/count/crossings.cpp")
# add_test(g_operations_incidence_barycenter "include/celaeno/test/heuristics/barycenter.cpp")
# ### Modifiers
# add_test(g_operations_balance_paths "include/celaeno/test/graph/operations/balance/paths.cpp")
# add_test(g_operations_minimize_crossings "include/celaeno/test/graph/operations/minimize/crossings.cpp")
#
# ## Representations
# add_test(g_representations_incidence "include/celaeno/test/graph/representations/incidence.cpp")

## Drawing

add_test(g_draw "include/celaeno/test/graph/draw/svg.cpp")

#add_test(test_a_star "include/celaeno/graph/a-star.cpp")
