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

message(STATUS "${C_YELLOW} * ${C_RESET}Tests enabled")

add_executable(
  test_bfs
    "include/celaeno/graph/bfs.cpp"
)
target_link_libraries(test_bfs
  celaeno
  taygete::taygete
  asterope::asterope
)

add_executable(
  test_a_star
    "include/celaeno/graph/a-star.cpp"
)
target_link_libraries(test_a_star
  celaeno
  taygete::taygete
  asterope::asterope
)

add_executable(
  test_graph_balance
    "include/celaeno/graph/balance.cpp"
)
target_link_libraries(test_graph_balance
  celaeno
  taygete::taygete
  asterope::asterope
)

add_executable(
  test_hvm
    "include/celaeno/graph/hvm.cpp"
)
target_link_libraries(test_hvm
  celaeno
  taygete::taygete
  asterope::asterope
)

add_executable(
  test_cantor_pairing
    "include/celaeno/math/cantor-pairing.cpp"
)
target_link_libraries(test_cantor_pairing
  celaeno
  taygete::taygete
  asterope::asterope
)

add_executable(
  test_dfs
    "include/celaeno/graph/dfs.cpp"
)
target_link_libraries(test_dfs
  celaeno
  taygete::taygete
  asterope::asterope
)

target_compile_features(test_bfs PRIVATE cxx_std_20)
target_compile_features(test_a_star PRIVATE cxx_std_20)
target_compile_features(test_graph_balance PRIVATE cxx_std_20)
target_compile_features(test_hvm PRIVATE cxx_std_20)
target_compile_features(test_cantor_pairing PRIVATE cxx_std_20)
target_compile_features(test_dfs PRIVATE cxx_std_20)
