include(CMakeFindDependencyMacro)
find_dependency(SPIRV-Tools)
include(${CMAKE_CURRENT_LIST_DIR}/SPIRV-Tools-linkTargets.cmake)
set(SPIRV-Tools-link_LIBRARIES SPIRV-Tools-link)
get_target_property(SPIRV-Tools-link_INCLUDE_DIRS SPIRV-Tools-link INTERFACE_INCLUDE_DIRECTORIES)
