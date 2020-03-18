# vim: set ts=2 sw=2 tw=0 et :
######################################################################
# @company     : Universidade Federal de Viçosa - Florestal
# @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
# @file        : CMakeLists
# @created     : Wednesday Aug 14, 2019 15:16:27 -03
# @license     : MIT
# @description : C++ Algorithms Collection
######################################################################


include(FetchContent)

# Override to show current download information
function(Fetch args)
  foreach(arg IN LISTS args)
    message("${C_GREEN} * ${C_RESET}Populating ${arg} Library")
    FetchContent_MakeAvailable(${arg})
  endforeach()
endfunction()


#
# Submodules
#

# Avoid installation of subproject
function(install)
  if( ${PROJECT_NAME} STREQUAL ${CMAKE_PROJECT_NAME} )
    _install(${ARGV})
  endif()
endfunction(install)

function(export)
  if( ${PROJECT_NAME} STREQUAL ${CMAKE_PROJECT_NAME} )
    _export(${ARGV})
  endif()
endfunction(export)


# Fetch
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  FetchContent_Declare(
    taygete
    GIT_REPOSITORY git@gitlab.com:formigoni/taygete.git
  )

  FetchContent_Declare(
    asterope
    GIT_REPOSITORY git@gitlab.com:formigoni/asterope.git
    GIT_TAG origin/refactor
  )
endif()

# Populate Libraries
if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
  Fetch("taygete;asterope")
endif()
