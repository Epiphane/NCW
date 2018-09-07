cmake_minimum_required(VERSION 3.1)
project(CubeWorld)

include(CheckCXXCompilerFlag)

if (WIN32)
    CHECK_CXX_COMPILER_FLAG("/std:c++17" COMPILER_SUPPORTS_CXX17)
else()
    CHECK_CXX_COMPILER_FLAG("-std=c++1z" COMPILER_SUPPORTS_CXX17)
endif()

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 -DGLEW_STATIC")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -DGLEW_STATIC -g")
endif()
set(CMAKE_MACOSX_RPATH 1)

set(SRC_DIR "${PROJECT_SOURCE_DIR}/source")
set(DEP_DIR "${PROJECT_SOURCE_DIR}/dependencies")
set(BUILD_DIR "${PROJECT_SOURCE_DIR}/build")

# GLFW
set(GLFW_DIR "${DEP_DIR}/glfw/3.1.0")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")
add_subdirectory(${GLFW_DIR})
list(APPEND INCLUDES
   ${GLFW_DIR}/include
)
list(APPEND LIBS
   glfw ${GLFW_LIBRARIES}
)

# glew
set(GLEW_DIR "${DEP_DIR}/glew/1.11.0")
list(APPEND INCLUDES
   ${GLEW_DIR}/include
)

add_library(glew STATIC ${GLEW_DIR}/src/glew.c )
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DGLEW_STATIC")
set_target_properties(glew PROPERTIES OUTPUT_NAME glew PREFIX lib)
list(APPEND LIBS
    glew
)

#glm
list(APPEND INCLUDES
   ${DEP_DIR}/glm/0.9.9.0
)

#noise
set(NOISE_DIR "${DEP_DIR}/libnoise/1.0.0")
set(NOISEUTILS_DIR "${DEP_DIR}/noiseutils/0.9.0.1")
add_subdirectory(${NOISE_DIR})
add_subdirectory(${NOISEUTILS_DIR})
list(APPEND INCLUDES
   ${NOISE_DIR}/src
   ${NOISEUTILS_DIR}
)
list(APPEND LIBS
   noise-static
   noiseutils-static
)

# FreeType
set(FREETYPE_DIR "${DEP_DIR}/freetype/2.9.1")
add_subdirectory(${FREETYPE_DIR})
list(APPEND INCLUDES
   ${FREETYPE_DIR}/include
)
list(APPEND LIBS
   freetype
)

# sqlite
set(SQLITE_DIR "${DEP_DIR}/sqlite")
add_subdirectory(${SQLITE_DIR})
list(APPEND INCLUDES
   ${SQLITE_DIR}/3.22.0
)
list(APPEND LIBS
   sqlite
)

# FMOD
# Find it in git's history - right now it just clutters up the stuff we actually use.

add_subdirectory(${SRC_DIR})

file(GLOB SOURCES
   "${SRC_DIR}/*.h"
   "${SRC_DIR}/*.cpp"
   "${SRC_DIR}/**/*.h"
   "${SRC_DIR}/**/*.cpp"
)

file(GLOB SHADERS
    "${SRC_DIR}/shaders/*.glsl"
)

include_directories(${INCLUDES})
