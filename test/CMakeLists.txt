# vim: set ts=2 sw=2 tw=0 et :
######################################################################
# @company     : Universidade Federal de Viçosa - Florestal
# @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
# @file        : CMakeLists
# @created     : Wednesday Aug 14, 2019 15:30:53 -03
# @license     : MIT
# @description : C++ Algorithms Collection
######################################################################

if( ${PROJECT_NAME}_tests )
  add_executable(${PROJECT_NAME}_tests tests.cpp)
  target_link_libraries(${PROJECT_NAME}_tests celaeno taygete asterope Catch2)
  target_compile_features(${PROJECT_NAME}_tests PRIVATE cxx_std_20)
endif()
