cmake_minimum_required(VERSION 3.25)
project(NashaEngine-dev)

set(CMAKE_CXX_STANDARD 20)

message(STATUS "using ${CMAKE_GENERATOR}")

if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles") # Only for windows
    if (NOT MINGW_PATH)
        message(FATAL_ERROR "MINGW_PATH not set in .env.cmake")
    endif()
    set(USE_MINGW "True")
    set(CMAKE_C_COMPILER ${MINGW_PATH}/bin/gcc.exe)
    set(CMAKE_CXX_COMPILER  ${MINGW_PATH}/bin/g++.exe)
endif()

# Setting up Vulkan
set(VULKAN_SDK_PATH ${PROJECT_SOURCE_DIR}/Nasha/vendors/Vulkan)
if (DEFINED VULKAN_SDK_PATH)
    message(STATUS "Found Vulkan")
    set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include")
    set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/Lib")
    set(Vulkan_FOUND "True")
else()
    find_package(Vulkan REQUIRED) # throws error if could not find Vulkan
    message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")
endif()
if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Could not find Vulkan library!")
else()
    message(STATUS "Using vulkan lib at: ${Vulkan_LIBRARIES}")
endif()

# Setting up GLFW
set(GLFW_PATH ${PROJECT_SOURCE_DIR}/Nasha/vendors/GLFW)
if (DEFINED GLFW_PATH)
    message(STATUS "GLFW_PATH found")
    set(GLFW_INCLUDE_DIRS "${GLFW_PATH}/include")
    set(GLFW_LIB "${GLFW_PATH}/lib")
else()
    find_package(glfw3 3.3 REQUIRED) # throws error if could not find GLFW
    set(GLFW_LIB glfw)
    message(STATUS "Found GLFW")
endif()
if (NOT GLFW_LIB)
    message(FATAL_ERROR "Could not find glfw library!")
else()
    message(STATUS "Using glfw lib at: ${GLFW_LIB}")
endif()

# Setting up tinyObjLoader
set(TINY_OBJ_LOADER_PATH ${PROJECT_SOURCE_DIR}/Nasha/vendors/tinyobjloader)
if (DEFINED TINY_OBJ_LOADER_PATH)
    message(STATUS "tiny_obj_loader path found")
    set(TINY_OBJ_LOADER_INCLUDE_DIR "${TINY_OBJ_LOADER_PATH}")
else()
    find_package(tiny_obj_loader REQUIRED)
endif()

file(GLOB_RECURSE SOURCES
        ${PROJECT_SOURCE_DIR}/Nasha/src/*.cpp
        ${PROJECT_SOURCE_DIR}/Nasha/src/Nasha/*.cpp
        ${PROJECT_SOURCE_DIR}/Sandbox/src/Nasha/main.cpp)

add_executable(${PROJECT_NAME} ${SOURCES} Nasha/src/Nasha/Utils.h Nasha/src/Nasha/GameObject.cpp Nasha/src/Nasha/Buffer.cpp Nasha/src/Nasha/Buffer.h)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

set_property(TARGET ${PROJECT_NAME} PROPERTY DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

if (WIN32)
    message(STATUS "CREATING BUILD FOR WINDOWS")

    if (USE_MINGW)
        target_include_directories(${PROJECT_NAME} PUBLIC
                ${MINGW_PATH}/include
                )
        target_link_directories(${PROJECT_NAME} PUBLIC
                ${MINGW_PATH}/lib
                )
    endif()

    target_include_directories(${PROJECT_NAME} PUBLIC
            ${PROJECT_SOURCE_DIR}/src
            ${VULKAN_INCLUDE_DIRS}
            ${TINYOBJ_PATH}
            ${GLFW_INCLUDE_DIRS}
            ${GLM_PATH}
            )

    target_link_directories(${PROJECT_NAME} PUBLIC
            ${VULKAN_LIBRARIES}
            ${GLFW_LIB}
            )

    target_link_libraries(${PROJECT_NAME} glfw3 vulkan-1)
elseif (UNIX)
    message(STATUS "CREATING BUILD FOR UNIX")
    target_include_directories(${PROJECT_NAME} PUBLIC
            ${Vulkan_INCLUDE_DIRS}
            ${GLFW_INCLUDE_DIRS}
            ${TINY_OBJ_LOADER_INCLUDE_DIR}
            )
    target_link_libraries(${PROJECT_NAME}
            ${Vulkan_LIBRARIES}/libvulkan.1.dylib
            ${Vulkan_LIBRARIES}/libvulkan.1.3.250.dylib
            ${GLFW_LIB}/libglfw.3.3.dylib
            "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo")
endif()

############## Build SHADERS #######################

# Find all vertex and fragment sources within shaders directory
# taken from VBlancos vulkan tutorial
# https://github.com/vblanco20-1/vulkan-guide/blob/all-chapters/CMakeLists.txt
find_program(GLSL_VALIDATOR glslc HINTS
        ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
        /usr/bin
        /usr/local/bin
        ${VULKAN_SDK_PATH}/bin
        ${VULKAN_SDK_PATH}/Bin
        ${VULKAN_SDK_PATH}/Bin32
        $ENV{VULKAN_SDK}/Bin/
        $ENV{VULKAN_SDK}/Bin32/
        )
if (GLSL_VALIDATOR)
    message(STATUS "Found GLSL at: ${GLSL_VALIDATOR}")
endif ()

# get all .vert and .frag files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/Nasha/src/Nasha/shaders/*.frag"
        "${PROJECT_SOURCE_DIR}/Nasha/src/Nasha/shaders/*.vert"
        )

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${PROJECT_SOURCE_DIR}/Nasha/src/Nasha/shaders/${FILE_NAME}.spv")
#    add_custom_command(
#            OUTPUT ${SPIRV}
#            COMMAND ${GLSL_VALIDATOR} ${GLSL} -o ${SPIRV}
#            DEPENDS ${GLSL})
    execute_process(COMMAND ${GLSL_VALIDATOR} ${GLSL} -o ${SPIRV})
    message(STATUS "Ran Command: ${GLSL_VALIDATOR} ${GLSL} -o ${SPIRV}")
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
        Shaders
        DEPENDS ${SPIRV_BINARY_FILES}
)
