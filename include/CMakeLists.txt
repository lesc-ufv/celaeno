# vim: set ts=2 sw=2 tw=0 et :
######################################################################
# @company     : Universidade Federal de Viçosa - Florestal
# @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
# @file        : CMakeLists
# @created     : Wednesday Aug 14, 2019 14:23:03 -03
# @license     : MIT
# @description : C++ Algorithms Collection
######################################################################

add_library(celaeno INTERFACE)
target_include_directories(celaeno INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(celaeno INTERFACE cxx_std_20)
target_link_libraries(celaeno INTERFACE range-v3)
