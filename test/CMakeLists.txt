# vim: set ts=2 sw=2 tw=0 et :
######################################################################
# @company     : Universidade Federal de Viçosa - Florestal
# @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
# @file        : CMakeLists
# @created     : Wednesday Aug 14, 2019 15:30:53 -03
# @license     : MIT
# @description : C++ Algorithms Collection
######################################################################

message(STATUS "${C_YELLOW} * ${C_RESET}Tests enabled")

add_executable(
  test_bfs
    "include/celaeno/graph/bfs.cpp"
)
target_link_libraries(test_bfs
  celaeno
  taygete
  asterope
)

add_executable(
  test_a_star
    "include/celaeno/graph/a-star.cpp"
)
target_link_libraries(test_a_star
  celaeno
  taygete
  asterope
)

add_executable(
  test_graph_balance
    "include/celaeno/graph/balance.cpp"
)
target_link_libraries(test_graph_balance
  celaeno
  taygete
  asterope
)

add_executable(
  test_hvm
    "include/celaeno/graph/hvm.cpp"
)
target_link_libraries(test_hvm
  celaeno
  taygete
  asterope
)

add_executable(
  test_cantor_pairing
    "include/celaeno/math/cantor-pairing.cpp"
)
target_link_libraries(test_cantor_pairing
  celaeno
  taygete
  asterope
)

target_compile_features(test_bfs PRIVATE cxx_std_20)
target_compile_features(test_a_star PRIVATE cxx_std_20)
target_compile_features(test_graph_balance PRIVATE cxx_std_20)
target_compile_features(test_hvm PRIVATE cxx_std_20)
target_compile_features(test_cantor_pairing PRIVATE cxx_std_20)
