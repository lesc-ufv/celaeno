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


# Submodules
FetchContent_Declare(
  taygete
  GIT_REPOSITORY git@gitlab.com:formigoni/taygete.git
)

FetchContent_Declare(
  asterope
  GIT_REPOSITORY git@gitlab.com:formigoni/asterope.git
  GIT_TAG origin/refactor
)

FetchContent_Declare(
  catch
  GIT_REPOSITORY git@github.com:catchorg/Catch2.git
)

FetchContent_Declare(
  ranges
  GIT_REPOSITORY git@github.com:ericniebler/range-v3.git
)

# Populate Libraries
if(${PROJECT_NAME}_tests)
  Fetch("catch;taygete;asterope")
endif()
Fetch("ranges")
